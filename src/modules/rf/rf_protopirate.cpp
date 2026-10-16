// SPDX-License-Identifier: AGPL-3.0-or-later
#if !defined(LITE_VERSION)
#include "rf_protopirate.h"
#include "core/mykeyboard.h"
#include "core/scrollableTextArea.h"
#include "core/sd_functions.h"
#include "protocols/rf_presets.h"
#include "protocols/rf_protocol_engine.h"
#include "rf_scan.h"
#include <ArduinoJson.h>
#include <algorithm>
#include <climits>
#include <map>
#include <sstream>

namespace {
struct Entry {
    String name, text, data, raw;
    bool encoder = false;
    bool fieldEditing = true;
    size_t bytes() const { return name.length() + text.length() + data.length() + raw.length(); }
};
String field(const String &text, const char *name) {
    std::istringstream lines(text.c_str());
    std::string line;
    const std::string prefix = std::string(name) + ":";
    while (std::getline(lines, line))
        if (line.compare(0, prefix.size(), prefix) == 0) {
            String value = line.substr(prefix.size()).c_str();
            value.trim();
            return value;
        }
    return "";
}
String setField(const String &text, const char *name, const String &value) {
    std::istringstream lines(text.c_str());
    std::string line;
    String result;
    bool found = false;
    const std::string prefix = std::string(name) + ":";
    while (std::getline(lines, line)) {
        if (line.compare(0, prefix.size(), prefix) == 0) {
            if (!found) result += String(name) + ": " + value + "\n";
            found = true;
        } else result += String(line.c_str()) + "\n";
    }
    if (!found) result += String(name) + ": " + value + "\n";
    return result;
}
bool number(const String &text, uint32_t &value) {
    if (text.isEmpty() || text.length() > 10) return false;
    uint64_t n = 0;
    for (size_t i = 0; i < text.length(); i++) {
        if (text[i] < '0' || text[i] > '9') return false;
        n = n * 10 + text[i] - '0';
        if (n > UINT32_MAX) return false;
    }
    value = n;
    return true;
}
int choose(const String &title, const std::vector<String> &items, int initial = 0) {
    int selected = -1;
    std::vector<Option> choices;
    for (size_t i = 0; i < items.size(); i++) choices.emplace_back(items[i], [&, i]() { selected = (int)i; });
    returnToMenu = false;
    loopOptions(choices, MENU_TYPE_SUBMENU, title.c_str(), initial);
    returnToMenu = false;
    return selected;
}
void view(const String &title, const String &text) {
    ScrollableTextArea area(title);
    area.fromString(text);
    area.show(true);
    delay(170);
}
bool response(const String &json, JsonDocument &doc, bool quiet = false) {
    if (deserializeJson(doc, json.c_str()) || !(doc["ok"] | false)) {
        if (!quiet) displayError(doc["error"] | "Protocol engine failed", true);
        return false;
    }
    return true;
}
class ProtoPirate {
    std::vector<Entry> history;
    std::vector<float> frequencies = {315, 433.92f, 433.65f, 434.25f, 434.42f, 868.35f};
    const char *presets[4] = {"Ook650Async", "2FSKDev476Async", "Ook270Async", "2FSKDev238Async"};
    const char *labels[4] = {"AM650", "FM476", "AM270", "FM238"};
    int frequencyIndex = 1, presetIndex = -1;
    bool hopping = false, receiveRequested = false;
    size_t historyBytes = 0;
    void remember(const Entry &entry) {
        size_t bytes = entry.bytes();
        if (bytes > 200000) return;
        // Repeated reception of the same decoded record updates its RAW sample
        // and recency instead of filling the list with identical rows.
        if (!entry.data.isEmpty()) {
            for (auto it = history.begin(); it != history.end(); ++it) {
                if (it->data == entry.data) {
                    historyBytes -= it->bytes();
                    history.erase(it);
                    break;
                }
            }
        }
        while (!history.empty() && (history.size() >= 12 || historyBytes + bytes > 200000)) {
            historyBytes -= history.front().bytes();
            history.erase(history.begin());
        }
        history.push_back(entry);
        historyBytes += bytes;
    }
    void save(const String &data, const char *name) {
        if (data.isEmpty()) {
            displayError("No data to save", true);
            return;
        }
        FS *fs = nullptr;
        if (!getFsStorage(fs)) return;
        File file = createNewFile(fs, "/BruceRF/ProtoPirate", String(name) + ".sub");
        if (!file) {
            displayError("Cannot create file", true);
            return;
        }
        String path = file.path();
        size_t written = file.print(data);
        file.close();
        if (written != data.length()) {
            displayError("Incomplete write", true);
            return;
        }
        displaySuccess("Saved", false);
        view("Saved file", path);
    }
    bool transmit(const String &raw) {
        if (raw.isEmpty()) {
            displayError("No RAW waveform", true);
            return false;
        }
        if (field(raw, "Bruce_Buffer_Full") == "1") {
            displayError("Capture is truncated", true);
            return false;
        }
        if (choose("Transmit signal?", {"Cancel", "Transmit"}) != 1) return false;
        displayInfo("Transmitting...", false);
        if (!rf_protocol_transmit_raw(raw.c_str())) {
            displayError("Transmission failed", true);
            return false;
        }
        displaySuccess("Waveform sent", true);
        return true;
    }
    void emulate(Entry &entry) {
        if (!entry.encoder) {
            displayError("No compatible encoder", true);
            return;
        }
        const char *counterField = field(entry.data, "Cnt").isEmpty() ? "Counter" : "Cnt";
        const char *buttonField = field(entry.data, "Btn").isEmpty() ? "Button" : "Btn";
        uint32_t counter = 0, button = 0;
        bool hasCounter = entry.fieldEditing && number(field(entry.data, counterField), counter);
        bool hasButton = entry.fieldEditing && number(field(entry.data, buttonField), button);
        auto next = [&]() {
            uint32_t n = counter + 1;
            if (!strcmp(counterField, "Counter")) return n & 65535U;
            if (entry.name == "Kia V1" || entry.name == "Kia V2") return n & 4095U;
            if (entry.name == "Ford V0") return n & 524287U;
            if (entry.name == "Renault V0") return n & 255U;
            return n;
        };
        for (;;) {
            int action = choose(
                entry.name + " / Emulate",
                {"Back",
                 "Encode + transmit",
                 "Next counter: " + (hasCounter ? String(next()) : String("unavailable")),
                 "Button: " + (hasButton ? String(button) : String("unavailable")),
                 "Preview encoded waveform"}
            );
            if (action <= 0) return;
            if (action == 2) {
                if (hasCounter) counter = next();
                continue;
            }
            if (action == 3) {
                if (hasButton) button = (button + 1) & (entry.name == "Kia V1" ? 255U : 15U);
                continue;
            }
            String source = setField(entry.data, "Repeat", "3");
            if (hasCounter) source = setField(source, counterField, String(next()));
            if (hasButton) source = setField(source, buttonField, String(button));
            JsonDocument encoded;
            if (!response(rf_protocol_encode_json(source.c_str()), encoded)) continue;
            if (action == 4) {
                view(
                    "Encoder preview",
                    String(encoded["pulses"].as<unsigned>()) + " durations\n" +
                        String(encoded["updated"] | "")
                );
                continue;
            }
            if (!transmit(encoded["raw"] | "")) continue;
            entry.data = encoded["updated"] | "";
            number(field(entry.data, counterField), counter);
            JsonDocument refreshed;
            if (response(rf_protocol_decode_json(entry.data.c_str()), refreshed))
                entry.text = refreshed["frames"][0]["text"] | "";
        }
    }
    void timings(const String &raw) {
        if (raw.isEmpty()) {
            displayError("No RAW waveform", true);
            return;
        }
        std::istringstream lines(raw.c_str());
        std::string line;
        uint64_t total = 0;
        unsigned count = 0, high = 0, low = 0;
        int smallest = INT_MAX, largest = 0;
        std::map<int, unsigned> bins;
        while (std::getline(lines, line)) {
            if (line.compare(0, 9, "RAW_Data:") && line.compare(0, 9, "Data_RAW:")) continue;
            std::istringstream values(line.substr(9));
            long long pulse;
            while (values >> pulse) {
                if (!pulse || pulse < -1000000 || pulse > 1000000 || count >= 16384) {
                    displayError("Invalid RAW durations", true);
                    return;
                }
                int duration = pulse < 0 ? -pulse : pulse;
                total += duration;
                count++;
                if (pulse > 0) high++;
                else low++;
                smallest = std::min(smallest, duration);
                largest = std::max(largest, duration);
                int bucket = ((duration + 25) / 50) * 50;
                if (bins.count(bucket) || bins.size() < 128) bins[bucket]++;
            }
            if (!values.eof()) {
                displayError("Malformed RAW data", true);
                return;
            }
        }
        if (!count) {
            displayError("No RAW durations", true);
            return;
        }
        String text = "Durations: " + String(count) + "\nHIGH / LOW: " + String(high) + " / " + String(low) +
                      "\nTotal: " + String((unsigned long)(total / 1000)) +
                      " ms\nMin / max: " + String(smallest) + " / " + String(largest) +
                      " us\nMean: " + String((unsigned long)(total / count)) +
                      " us\n\nTiming groups (50 us):\n";
        std::vector<std::pair<int, unsigned>> groups(bins.begin(), bins.end());
        std::sort(groups.begin(), groups.end(), [](const auto &a, const auto &b) {
            return a.second > b.second;
        });
        for (size_t i = 0; i < groups.size() && i < 10; i++)
            text += String(groups[i].first) + " us: " + String(groups[i].second) + "\n";
        view("Timing analyzer", text);
    }
    void matching(const Entry &entry) {
        const String &data = entry.raw.isEmpty() ? entry.data : entry.raw;
        uint32_t hz = 0;
        if (!number(field(data, "Frequency"), hz)) {
            displayError("Missing frequency", true);
            return;
        }
        if (!((hz >= 300000000 && hz <= 348000000) || (hz >= 387000000 && hz <= 464000000) ||
              (hz >= 779000000 && hz <= 928000000))) {
            displayError("Invalid frequency", true);
            return;
        }
        String presetName = field(data, "Bruce_RX_Preset");
        if (presetName.isEmpty()) presetName = field(data, "Preset");
        const auto *preset = rf_find_preset(presetName);
        int pi = -1;
        for (int i = 0; preset && i < 4; i++)
            if (String(preset->name) == presets[i]) pi = i;
        if (pi < 0) {
            displayError("Unsupported receive preset", true);
            return;
        }
        if (frequencies.size() > 6) frequencies.pop_back();
        frequencies.push_back(hz / 1000000.0f);
        frequencyIndex = frequencies.size() - 1;
        presetIndex = pi;
        hopping = false;
        receiveRequested = true;
    }
    void result(Entry &entry) {
        for (;;) {
            int action = choose(
                entry.name,
                {"View decoded fields",
                 "Replay original RAW",
                 "Emulate / encode",
                 "Save original RAW",
                 "Save decoded .sub",
                 "Timing analyzer",
                 "Receive matching signal",
                 "Back"}
            );
            if (action < 0 || action == 7) return;
            if (action == 0) view(entry.name, entry.text + "\n\n" + entry.data);
            else if (action == 1) transmit(entry.raw);
            else if (action == 2) emulate(entry);
            else if (action == 3) save(entry.raw, "RAW");
            else if (action == 4) save(entry.data, "decoded");
            else if (action == 5) timings(entry.raw);
            else if (action == 6) matching(entry);
            if (receiveRequested) return;
        }
    }
    // Live unmatched captures never interrupt the receive loop with a menu.
    bool analyze(const String &input, bool live, Entry &unknown) {
        JsonDocument decoded;
        String raw = field(input, "Protocol") == "RAW" ? input : String();
        if (!response(rf_protocol_decode_json(input.c_str()), decoded, live)) {
            // Keep the waveform and actual error so a failed live decode can
            // still be inspected or saved from History after leaving Receive.
            unknown = {"Capture decode failed", decoded["error"] | "Protocol engine failed", "", raw, false};
            return false;
        }
        auto frames = decoded["frames"].as<JsonArray>();
        if (frames.size() == 0) {
            unknown = {
                "Unrecognized signal",
                "No complete supported frame decoded. Check frequency/preset.",
                "",
                raw,
                false
            };
            if (field(input, "Bruce_Buffer_Full") == "1")
                unknown.text = "Capture buffer filled; waveform may be incomplete.";
            if (!live) {
                remember(unknown);
                result(unknown);
            }
            return false;
        }
        std::vector<Entry> entries;
        std::vector<String> names;
        size_t bytes = 0;
        for (JsonObject frame : frames) {
            Entry entry = {
                frame["protocol"] | "",
                frame["text"] | "",
                frame["data"] | "",
                raw,
                frame["encoder"] | false,
                frame["fieldEditing"] | true
            };
            if (bytes + entry.bytes() > 200000) break;
            bytes += entry.bytes();
            entries.push_back(entry);
            names.push_back(entry.name);
        }
        if (entries.empty()) return false;
        // Live reception stays on the capture list, as in RF Scan/Copy.
        // File decoding retains the JS app's immediate result/actions menu.
        if (!live) {
            int selected = entries.size() == 1 ? 0 : choose("Decoded frames", names);
            if (selected >= 0) result(entries[selected]);
        }
        for (const auto &entry : entries) remember(entry);
        return true;
    }
    String captureLabel(size_t index) const {
        const Entry &entry = history[index];
        String key = field(entry.data, "Key");
        key.replace(" ", "");
        String extra = field(entry.data, "Key_2");
        extra.replace(" ", "");
        if (!extra.isEmpty()) key += extra;
        const String bits = field(entry.data, "Bit");
        return String(index + 1) + ". " + entry.name +
               (bits.isEmpty() ? String() : " (" + bits + "b)") +
               (key.isEmpty() ? String() : " " + key);
    }
    void openHistoryEntry(size_t selected) {
        Entry entry = history[selected];
        result(entry);
        historyBytes -= history[selected].bytes();
        history[selected] = entry;
        historyBytes += entry.bytes();
    }
    void receive() {
        if (bruceConfigPins.rfModule != CC1101_SPI_MODULE) {
            displayError("Select CC1101 RF module", true);
            return;
        }
        Entry lastUnknown;
        unsigned misses = 0, cycle = 0;
        int selected = history.empty() ? 0 : int(history.size()) - 1;
        int scrollOffset = 0;
        uint32_t started = millis();
        rf_clear_nav_state();
        for (;;) {
#ifdef HAS_3_BUTTONS
            if (EscPress && PrevPress) EscPress = false;
#endif
            if (check(EscPress)) {
                if (!lastUnknown.raw.isEmpty()) remember(lastUnknown);
                return;
            }
            if (check(SelPress)) {
                if (history.empty()) settings();
                else openHistoryEntry(selected);
                rf_clear_nav_state();
                if (receiveRequested) return;
                cycle = 0;
                started = millis();
            }
            bool up = check(PrevPress);
            if (check(UpPress)) up = true;
            bool down = check(NextPress);
            if (check(DownPress)) down = true;
            if (!history.empty() && (up || down))
                selected = (selected + (up ? -1 : 1) + int(history.size())) % int(history.size());

            bool scanning = presetIndex < 0 || hopping;
            int pi = presetIndex < 0 ? cycle % 4 : presetIndex;
            int fi = hopping ? (cycle / (presetIndex < 0 ? 4 : 1)) % frequencies.size() : frequencyIndex;
            uint32_t elapsed = millis() - started;
            int timeout =
                scanning ? std::max(1, int((elapsed < 12000 ? 12000 - elapsed : 0) + 999) / 1000) : 30;
            std::vector<String> rows;
            for (size_t i = 0; i < history.size(); ++i) rows.push_back(captureLabel(i));
            rf_draw_capture_list(
                "ProtoPirate RX", String(frequencies[fi]) + "MHz " + labels[pi] + (scanning ? " Auto" : ""),
                "Unmatched: " + String(misses) + (history.empty() ? "  SEL: settings" : "  SEL: signal"),
                rows, selected, scrollOffset, 12
            );
            String raw = rfReceiveSignal(frequencies[fi], timeout, true, true, presets[pi], true);
            if (EscPress) continue; // preserve Back for the list's exit handler
            if (!raw.isEmpty()) {
                if (field(raw, "Bruce_RX_Preset") != presets[pi]) {
                    displayError("Receive profile mismatch", true);
                    return;
                }
                Entry unknown;
                if (analyze(raw, true, unknown)) {
                    started = millis();
                    selected = history.empty() ? 0 : int(history.size()) - 1;
                } else {
                    if (!unknown.raw.isEmpty()) lastUnknown = unknown;
                    misses++;
                }
            }
            if (scanning && millis() - started >= 12000) {
                cycle++;
                started = millis();
            }
            delay(10);
        }
    }
    void browse() {
        FS *fs = nullptr;
        if (!getFsStorage(fs)) return;
        String directory = "/";
        for (;;) {
            String path = loopSD(*fs, true, "SUB", directory);
            if (path.isEmpty()) return;
            directory = path.substring(0, path.lastIndexOf('/'));
            if (directory.isEmpty()) directory = "/";
            File file = fs->open(path, FILE_READ);
            if (!file || file.size() > 262144) {
                file.close();
                displayError("File exceeds 256 KiB or is missing", true);
                continue;
            }
            String input = file.readString();
            file.close();
            Entry unused;
            analyze(input, false, unused);
            if (receiveRequested) return;
        }
    }
    void settings() {
        for (;;) {
            int action = choose(
                "Receiver settings",
                {"Frequency: " + String(frequencies[frequencyIndex]) + " MHz",
                 "Preset: " + String(presetIndex < 0 ? "Auto" : labels[presetIndex]),
                 "Frequency hopping: " + String(hopping ? "ON" : "OFF"),
                 "Back"}
            );
            if (action < 0 || action == 3) return;
            if (action == 0) {
                std::vector<String> list;
                for (float f : frequencies) list.push_back(String(f) + " MHz");
                int n = choose("Frequency", list, frequencyIndex);
                if (n >= 0) frequencyIndex = n;
            } else if (action == 1) {
                int n = choose("Preset", {"Auto", "AM650", "FM476", "AM270", "FM238"}, presetIndex + 1);
                if (n >= 0) presetIndex = n - 1;
            } else hopping = !hopping;
        }
    }

public:
    void run() {
        for (;;) {
            int action = receiveRequested ? 0
                                          : choose(
                                                "ProtoPirate / Emulation ON",
                                                {"Receive signal",
                                                 "Sub Decode / files",
                                                 "Signal history",
                                                 "Receiver settings",
                                                 "Protocol catalogue",
                                                 "About / limitations",
                                                 "Exit"}
                                            );
            receiveRequested = false;
            if (action < 0 || action == 6) break;
            if (action == 0) receive();
            else if (action == 1) browse();
            else if (action == 2) {
                if (history.empty()) {
                    displayInfo("No captures this session", true);
                    continue;
                }
                std::vector<String> names;
                for (size_t i = history.size(); i > 0; --i) names.push_back(captureLabel(i - 1));
                int selected = choose("Signal history", names);
                if (selected >= 0) openHistoryEntry(history.size() - 1 - selected);
            } else if (action == 3) settings();
            else if (action == 4) {
                JsonDocument catalogue;
                if (!response(rf_protocol_catalog_json(), catalogue)) continue;
                String text = "Shared Bruce beta + ProtoPirate protocols\nEmulation enabled by default\n\n";
                for (JsonObject p : catalogue["protocols"].as<JsonArray>())
                    text += String(p["name"] | "") + ((p["encoder"] | false) ? ": RX + encoder\n" : ": RX\n");
                view("Protocol catalogue", text);
            } else
                view(
                    "About / limitations",
                    "Native Bruce UI and one shared RF engine.\nRocketGod ProtoPirate 4.0 / 4c9afe58\n\n"
                    "Same codecs are used by RF Scan, file handling and the JS app.\n"
                    "Sending always requires confirmation. Encoding does not prove receiver acceptance.\n"
                    "Custom radio profiles, encrypted ProtoPirate keystores, model database, remote analyzer "
                    "and brute-force recovery are not ported."
                );
        }
        deinitRfModule();
        returnToMenu = true;
    }
};
} // namespace

void rf_protopirate() {
    ProtoPirate app;
    app.run();
}
#endif
