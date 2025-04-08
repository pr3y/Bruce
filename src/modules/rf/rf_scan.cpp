#include "rf_scan.h"
#include "core/sd_functions.h"
#include "core/type_convertion.h"
#include "rf_send.h"
#include <globals.h>
#include <sstream>

RFScan::RFScan() { setup(); }

RFScan::~RFScan() { deinitRfModule(); }

void RFScan::setup() {
    if (!initRfModule("rx", bruceConfig.rfFreq)) { return; }

    RCSwitch_Enable_Receive(rcswitch);

    if (bruceConfig.rfScanRange < 0 || bruceConfig.rfScanRange > 3) { bruceConfig.setRfScanRange(3); }
    if (bruceConfig.rfModule != CC1101_SPI_MODULE) { bruceConfig.setRfFxdFreq(1); }

    display_info(received, signals, ReadRAW, codesOnly, title);

    if (bruceConfig.rfFxdFreq) frequency = bruceConfig.rfFreq;

    // Clear cache for RAW signal
    rcswitch.resetAvailable();
    returnToMenu = false;
    restartScan = false;

    return loop();
}

void RFScan::loop() {
    while (1) {
        if (check(NextPress)) select_menu_option();
        if (check(EscPress) || returnToMenu) return;
        if (restartScan) return setup();

        if (bruceConfig.rfFxdFreq) frequency = bruceConfig.rfFreq;
        if (frequency <= 0) init_freqs();

        while (frequency <= 0) { // FastScan
            if (check(EscPress) || returnToMenu) return;
            if (check(NextPress)) select_menu_option();

            fast_scan();
        }

        if (rcswitch.available() && !ReadRAW) {
            read_rcswitch();
            if (autoSave) save_signal();
        }
        if (rcswitch.RAWavailable() && ReadRAW) {
            read_raw();
            if (autoSave) save_signal();
        }
    }
}

void RFScan::RCSwitch_Enable_Receive(RCSwitch rcswitch) {
    if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
        if (bruceConfig.CC1101_bus.io2 != GPIO_NUM_NC) rcswitch.enableReceive(bruceConfig.CC1101_bus.io2);
        else { rcswitch.enableReceive(bruceConfig.CC1101_bus.io0); }
    } else {
        rcswitch.enableReceive(bruceConfig.rfRx);
    }
}

void RFScan::init_freqs() {
    for (int i = 0; i < _MAX_TRIES; i++) {
        _freqs[i].freq = 433.92;
        _freqs[i].rssi = -75;
    }
    _try = 0;
}

void RFScan::fast_scan() {

    if (idx < range_limits[bruceConfig.rfScanRange][0] || idx > range_limits[bruceConfig.rfScanRange][1]) {
        idx = range_limits[bruceConfig.rfScanRange][0];
    }
    float checkFrequency = subghz_frequency_list[idx];
    setMHZ(checkFrequency);
    tft.drawPixel(0, 0, 0); // To make sure CC1101 shared with TFT works properly
    delay(5);
    rssi = ELECHOUSE_cc1101.getRssi();
    if (rssi > rssiThreshold) {
        _freqs[_try].freq = checkFrequency;
        _freqs[_try].rssi = rssi;
        _try++;
        if (_try >= _MAX_TRIES) {
            int max_index = 0;
            for (int i = 1; i < _MAX_TRIES; ++i) {
                if (_freqs[i].rssi > _freqs[max_index].rssi) { max_index = i; }
            }

            bruceConfig.setRfFreq(_freqs[max_index].freq, 0);
            frequency = _freqs[max_index].freq;
            setMHZ(frequency);
            Serial.println("Frequency Found: " + String(frequency));
            rcswitch.resetAvailable();
        }
    }
    ++idx;
}

void RFScan::read_rcswitch() {
    // Add decoded data only (if any) to the RCCode
    uint64_t decoded = rcswitch.getReceivedValue();

    if (decoded) { // if there is a value decoded by RCSwitch, show it
        Serial.println("RcSwitch signal captured");
        ++signals;
        found_freq = frequency;
        received.frequency = long(frequency * 1000000);
        received.key = decoded;
        received.preset = String(rcswitch.getReceivedProtocol());
        received.protocol = "RcSwitch";
        received.te = rcswitch.getReceivedDelay();
        received.Bit = rcswitch.getReceivedBitlength();
        received.filepath = "signal_" + String(signals);
        received.data = "";

        frequency = 0;
        display_info(received, signals, ReadRAW, codesOnly, title);
    }

    rcswitch.resetAvailable();
}

void RFScan::read_raw() {
    // Add RAW data (& decoded data if any) to the RCCode
    delay(400); // wait for all the signal to be read
    found_freq = frequency;
    unsigned int *raw = rcswitch.getRAWReceivedRawdata();
    uint64_t decoded = rcswitch.getReceivedValue();
    int transitions = 0;
    String _data = "";
    std::vector<int> durations;
    std::vector<int> indexed_durations;
    uint64_t result = 0;
    uint8_t repetition = 0;

    received.te = 0;
    for (transitions = 0; transitions < RCSWITCH_RAW_MAX_CHANGES; transitions++) {
        if (raw[transitions] == 0) break;
        if (transitions > 0) _data += " ";
        signed int sign = (transitions % 2 == 0) ? 1 : -1;

        int duration = sign * (int)raw[transitions];
        if (duration < -5000 && repetition < 2) { repetition += 1; }
        _data += String(duration);
        if (received.te == 0 && duration > 0) received.te = duration;

        if (!decoded && repetition == 1 && duration >= -5000) {
            int index = find_pulse_index(indexed_durations, duration);
            if (index == -1) {
                indexed_durations.push_back(abs(duration));
                index = indexed_durations.size() - 1;
            }
            durations.push_back(index); // Store indexes for CRC calculation
        }
    }

    received.data = _data;
    received.filepath = "signal_" + String(signals);
    received.frequency = long(frequency * 1000000);

    // if there is a value decoded by RCSwitch, show it
    if (decoded) {
        Serial.println("RcSwitch signal captured");
        ++signals;
        received.key = decoded;
        received.preset = String(rcswitch.getReceivedProtocol());
        received.protocol = "RcSwitch";
        received.indexed_durations = {};
        received.te = rcswitch.getReceivedDelay();
        received.Bit = rcswitch.getReceivedBitlength();
        frequency = 0;
        display_info(received, signals, ReadRAW, codesOnly, title);
    }
    // if there is no value decoded by RCSwitch, but we calculated a CRC, show it
    else if (repetition >= 2 && !durations.empty()) {
        Serial.println("Raw signal captured");
        ++signals;
        received.preset = "0";
        received.protocol = "RAW";
        received.key = crc64_ecma(durations); // Calculate CRC-64
        received.indexed_durations = indexed_durations;
        received.Bit = durations.size();
        frequency = 0;
        display_info(received, signals, ReadRAW, codesOnly, title);
    }
    // If there is no decoded value and no CRC calculated, only show the data when specified
    else if (!codesOnly) {
        Serial.println("Raw data captured");
        ++signals;
        received.preset = "0";
        received.protocol = "RAW";
        received.key = 0;
        received.indexed_durations = {};
        received.Bit = 0;
        frequency = 0;
        display_info(received, signals, ReadRAW, codesOnly, title);
    }

    rcswitch.resetAvailable();
}

void RFScan::select_menu_option() {
#ifndef T_EMBED_1101
    rcswitch.disableReceive(); // it is causing T-Embed to restart
#endif

    options = {};

    if (received.protocol != "") options.emplace_back("Replay", [=]() { set_option(REPLAY); });
    if (received.data != "" && received.protocol != "RAW")
        options.emplace_back("Replay as RAW", [=]() { set_option(REPLAY_RAW); });

    if (received.protocol != "") options.emplace_back("Save Signal", [=]() { set_option(SAVE); });
    if (received.data != "" && received.protocol != "RAW")
        options.emplace_back("Save as RAW", [=]() { set_option(SAVE_RAW); });

    if (received.protocol != "") options.emplace_back("Reset Signal", [=]() { set_option(RESET); });

    if (bruceConfig.rfModule == CC1101_SPI_MODULE)
        options.emplace_back("Range", [=]() { set_option(RANGE); });
    if (bruceConfig.rfModule == CC1101_SPI_MODULE && !bruceConfig.rfFxdFreq)
        options.emplace_back("Threshold", [=]() { set_option(THRESHOLD); });

    if (ReadRAW)
        options.emplace_back("Mode = Decode", [&]() {
            ReadRAW = false;
            set_option(CLOSE_MENU);
        });
    else
        options.emplace_back("Mode = RAW", [&]() {
            ReadRAW = true;
            set_option(CLOSE_MENU);
        });

    if (ReadRAW && codesOnly)
        options.emplace_back("Filter = All", [&]() {
            codesOnly = false;
            set_option(CLOSE_MENU);
        });
    else if (ReadRAW)
        options.emplace_back("Filter = Code", [&]() {
            codesOnly = true;
            set_option(CLOSE_MENU);
        });

    if (autoSave)
        options.emplace_back("Save = Manual", [&]() {
            autoSave = false;
            set_option(CLOSE_MENU);
        });
    else
        options.emplace_back("Save = Auto", [&]() {
            autoSave = true;
            set_option(CLOSE_MENU);
        });

    options.emplace_back("Close Menu", [=]() { set_option(CLOSE_MENU); });
    options.emplace_back("Main Menu", [=]() { set_option(MAIN_MENU); });

    loopOptions(options);
}

void RFScan::set_option(RFMenuOption option) {
    switch (option) {
        case REPLAY:
        case REPLAY_RAW: replay_signal(option == REPLAY_RAW); break;

        case SAVE:
        case SAVE_RAW: save_signal(option == SAVE_RAW); break;

        case RANGE: set_range(); break;
        case RESET: reset_signals(); break;
        case THRESHOLD: set_threshold(); break;

        case CLOSE_MENU: break;

        case MAIN_MENU: returnToMenu = true; return;
    }

    restartScan = true;
    deinitRfModule();
    delay(1000);
}

void RFScan::replay_signal(bool asRaw) {
    String actualProtocol = received.protocol;
    if (asRaw) { received.protocol = "RAW"; }
    sendRfCommand(received);
    addToRecentCodes(received);
    received.protocol = actualProtocol;
}

void RFScan::save_signal(bool asRaw) {
    asRaw = asRaw || received.protocol == "RAW";
    Serial.println(asRaw ? "RCSwitch_SaveSignal RAW true" : "RCSwitch_SaveSignal RAW false");
    decimalToHexString(received.key, hexString);
    RCSwitch_SaveSignal(found_freq, received, asRaw, hexString, autoSave);
}

void RFScan::reset_signals() {
    received.Bit = 0;
    received.data = "";
    received.key = 0;
    received.preset = "";
    received.protocol = "";
    signals = 0;
}

void RFScan::set_threshold() {
    options = {
        {"(-55) More Accurate", [&]() { rssiThreshold = -55; }},
        {"(-60)",               [&]() { rssiThreshold = -60; }},
        {"(-65) Default ",      [&]() { rssiThreshold = -65; }},
        {"(-70)",               [&]() { rssiThreshold = -70; }},
        {"(-75)",               [&]() { rssiThreshold = -75; }},
        {"(-80) Less Accurate", [&]() { rssiThreshold = -80; }},
    };
    loopOptions(options);
}

void RFScan::set_range() {
    bool chooseFixedOpt = false;

    options = {
        {String("Fxd [" + String(bruceConfig.rfFreq) + "]").c_str(),
         [=]() { bruceConfig.setRfScanRange(bruceConfig.rfScanRange, 1); }                                   },
        {"Choose Fxd",                                               [&]() { chooseFixedOpt = true; }        },
        {subghz_frequency_ranges[0],                                 [=]() { bruceConfig.setRfScanRange(0); }},
        {subghz_frequency_ranges[1],                                 [=]() { bruceConfig.setRfScanRange(1); }},
        {subghz_frequency_ranges[2],                                 [=]() { bruceConfig.setRfScanRange(2); }},
        {subghz_frequency_ranges[3],                                 [=]() { bruceConfig.setRfScanRange(3); }},
    };

    loopOptions(options);

    if (chooseFixedOpt) { // Range
        options.clear();
        int ind = 0;
        int arraySize = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
        for (int i = 0; i < arraySize; i++) {
            String tmp = String(subghz_frequency_list[i], 2) + "Mhz";
            options.emplace_back(tmp.c_str(), [=]() { bruceConfig.rfFreq = subghz_frequency_list[i]; });
            if (int(frequency * 100) == int(subghz_frequency_list[i] * 100)) ind = i;
        }
        loopOptions(options, ind);
        options.clear();
        bruceConfig.setRfScanRange(bruceConfig.rfScanRange, 1);
    }

    if (bruceConfig.rfFxdFreq) displayTextLine("Scan freq set to " + String(bruceConfig.rfFreq));
    else displayTextLine("Range set to " + String(subghz_frequency_ranges[bruceConfig.rfScanRange]));
}

void display_info(RfCodes received, int signals, bool ReadRAW, bool codesOnly, String title) {
    if (title != "") drawMainBorderWithTitle(title);
    else drawMainBorder();

    if (received.protocol != "") display_signal_data(received);

    tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);

    if (!ReadRAW) padprintln("Recording: Only RCSwitch codes.");
    else if (codesOnly) padprintln("Recording: RAW with CRC or RCSwitch codes");
    else padprintln("Recording: Any RAW signal.");

    if (bruceConfig.rfFxdFreq) padprintln("Scanning: " + String(bruceConfig.rfFreq) + " MHz");
    else padprintln("Scanning: " + String(subghz_frequency_ranges[bruceConfig.rfScanRange]));

    padprintln("Total signals found: " + String(signals));

    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    padprintln("");
    padprintln("Press [NEXT] for options.");
}

void display_signal_data(RfCodes received) {
    std::string txt = received.data.c_str();
    std::stringstream ss(txt);
    std::string palavra;
    int transitions = 0;
    char hexString[64];

    while (ss >> palavra) transitions++;

    if (received.preset != "")
        padprintln("Protocol: " + String(received.protocol) + "(" + received.preset + ")");
    else padprintln("Protocol: " + String(received.protocol));

    if (received.key > 0) {
        decimalToHexString(received.key, hexString);
        if (received.protocol == "RAW") {
            padprintln("Lenght: " + String(received.Bit) + " transitions");
            // tft.setCursor(tft.getCursorX(), tft.getCursorY() + 2);
            padprintln("Record length: " + String(transitions) + " transitions");
        } else {
            padprintln("Lenght: " + String(received.Bit) + " bits");
            const char *b = dec2binWzerofill(received.key, min(received.Bit, 40));
            // tft.setCursor(tft.getCursorX(), tft.getCursorY() + 2);
            padprintln("Binary: " + String(b));
        }
    } else {
        strcpy(hexString, "No code identified");
        padprintln("Lenght: No code identified");
        padprintln("Record length: " + String(transitions) + " transitions");
    }

    if (received.protocol == "RAW") padprintln("CRC: " + String(hexString));
    else padprintln("Key: " + String(hexString));

    // if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
    //     int rssi = ELECHOUSE_cc1101.getRssi();
    //     tft.drawPixel(0, 0, 0);
    //     padprintln("Rssi: " + String(rssi));
    // }

    // if (!received.indexed_durations.empty()) {
    //     padprint("PulseLenghts: ");
    //     for (int i = 0; i < received.indexed_durations.size(); i++) {
    //         if (i < received.indexed_durations.size() - 1)
    //             tft.print(String(received.indexed_durations[i]) + "us, ");
    //         else tft.println(String(received.indexed_durations[i]) + "us");
    //     }
    // } else if (received.te) padprintln("PulseLenght: " + String(received.te) + "us");
    // else padprintln("PulseLenght: unknown");

    // padprintln("Frequency: " + String(received.frequency) + " Hz");
    padprintln("");
}

bool RCSwitch_SaveSignal(float frequency, RfCodes codes, bool raw, char *key, bool autoSave) {
    FS *fs;
    String filename = "";

    if (!getFsStorage(fs)) {
        displayError("No space left on device", true);
        return false;
    }

    if (!codes.key && codes.data == "") {
        Serial.println("Empty signal, it was not saved.");
        return false;
    }

    String subfile_out = "Filetype: Bruce SubGhz File\nVersion 1\n";
    subfile_out += "Frequency: " + String(int(frequency * 1000000)) + "\n";
    if (!raw) {
        subfile_out += "Preset: " + String(codes.preset) + "\n";
        subfile_out += "Protocol: RcSwitch\n";
        subfile_out += "Bit: " + String(codes.Bit) + "\n";
        subfile_out += "Key: " + String(key) + "\n";
        subfile_out += "TE: " + String(codes.te) + "\n";
        filename = "rcs.sub";
        // subfile_out += "RAW_Data: " + codes.data;
    } else {
        // save as raw
        if (codes.preset == "1") {
            codes.preset = "FuriHalSubGhzPresetOok270Async";
        } else if (codes.preset == "2") {
            codes.preset = "FuriHalSubGhzPresetOok650Async";
        }

        subfile_out += "Preset: " + String(codes.preset) + "\n";
        subfile_out += "Protocol: RAW\n";
        subfile_out += "RAW_Data: " + codes.data;
        filename = "raw.sub";
    }

    String filepath = "/BruceRF";
    if (autoSave) filepath += "/autoSaved";
    File file = createNewFile(fs, filepath, filename);

    if (file) {
        file.println(subfile_out);
        if (!autoSave) displaySuccess(file.path());
    } else {
        displayError("Error saving file", true);
    }

    file.close();
    return true;
}

String rf_scan(float start_freq, float stop_freq, int max_loops) {
    // derived from https://github.com/mcore1976/cc1101-tool/blob/main/cc1101-tool-esp32.ino#L480

    if (bruceConfig.rfModule != CC1101_SPI_MODULE) {
        displayError("rf scanning is available with CC1101 only", true);
        return ""; // only CC1101 is supported for this
    }
    if (!initRfModule("rx", start_freq)) return "";

    ELECHOUSE_cc1101.setRxBW(256);

    float settingf1 = start_freq;
    float settingf2 = stop_freq;
    float freq = 0;
    long compare_freq = 0;
    float mark_freq;
    int rssi;
    int mark_rssi = -100;
    String out = "";

    while (max_loops || !check(EscPress)) {
        delay(1);
        max_loops -= 1;

        setMHZ(freq);

        rssi = ELECHOUSE_cc1101.getRssi();
        if (rssi > -75) {
            if (rssi > mark_rssi) {
                mark_rssi = rssi;
                mark_freq = freq;
            };
        };

        freq += 0.01;

        if (freq > settingf2) {
            freq = settingf1;

            if (mark_rssi > -75) {
                long fr = mark_freq * 100;
                if (fr == compare_freq) {
                    Serial.print(F("\r\nSignal found at  "));
                    Serial.print(F("Freq: "));
                    Serial.print(mark_freq);
                    Serial.print(F(" Rssi: "));
                    Serial.println(mark_rssi);
                    mark_rssi = -100;
                    compare_freq = 0;
                    mark_freq = 0;
                    out += String(mark_freq) + ",";
                } else {
                    compare_freq = mark_freq * 100;
                    freq = mark_freq - 0.10;
                    mark_freq = 0;
                    mark_rssi = -100;
                };
            };
        }; // end of IF freq>stop frequency
    }; // End of While

    deinitRfModule();
    return out;
}

String RCSwitch_Read(float frequency, int max_loops, bool raw) {
    RCSwitch rcswitch = RCSwitch();
    RfCodes received;

    if (!frequency) frequency = bruceConfig.rfFreq; // default from config

    char hexString[64];

RestartRec:
    drawMainBorder();
    tft.setCursor(10, 28);
    tft.setTextSize(FP);
    tft.println("Waiting for a " + String(frequency) + " MHz " + "signal.");

    // init receive
    if (!initRfModule("rx", frequency)) return "";
    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        if (bruceConfig.CC1101_bus.io2 != GPIO_NUM_NC) rcswitch.enableReceive(bruceConfig.CC1101_bus.io2);
        else rcswitch.enableReceive(bruceConfig.CC1101_bus.io0);
        Serial.println("CC1101 enableReceive()");

    } else {
        rcswitch.enableReceive(bruceConfig.rfRx);
    }
    while (!check(EscPress)) {
        if (rcswitch.available()) {
            // Serial.println("Available");
            long value = rcswitch.getReceivedValue();
            // Serial.println("getReceivedValue()");
            if (value) {
                // Serial.println("has value");
                unsigned int *_raw = rcswitch.getReceivedRawdata();
                received.frequency = long(frequency * 1000000);
                received.key = rcswitch.getReceivedValue();
                received.protocol = "RcSwitch";
                received.preset = rcswitch.getReceivedProtocol();
                received.te = rcswitch.getReceivedDelay();
                received.Bit = rcswitch.getReceivedBitlength();
                received.filepath = "unsaved";
                // Serial.println(received.te*2);
                //  derived from https://github.com/sui77/rc-switch/tree/master/examples/ReceiveDemo_Advanced
                received.data = "";
                int sign = +1;
                // if(received.preset.invertedSignal) sign = -1;
                for (int i = 0; i < received.Bit * 2; i++) {
                    if (i > 0) received.data += " ";
                    if (i % 2 == 0) sign = +1;
                    else sign = -1;
                    received.data += String(sign * (int)_raw[i]);
                }
                // Serial.println(received.protocol);
                // Serial.println(received.data);
                decimalToHexString(received.key, hexString);

                display_info(received, 1, raw, false);
            }
            rcswitch.resetAvailable();
        }
        if (raw && rcswitch.RAWavailable()) {
            // if no value were decoded, show raw data to be saved
            delay(100); // give it time to process and store all signal

            unsigned int *_raw = rcswitch.getRAWReceivedRawdata();
            int transitions = 0;
            signed int sign = 1;
            for (transitions = 0; transitions < RCSWITCH_RAW_MAX_CHANGES; transitions++) {
                if (_raw[transitions] == 0) break;
                if (transitions > 0) received.data += " ";
                if (transitions % 2 == 0) sign = +1;
                else sign = -1;
                received.data += String(sign * (int)_raw[transitions]);
            }
            if (transitions > 20) {
                received.frequency = long(frequency * 1000000);
                received.protocol = "RAW";
                received.preset = "0"; // ????
                received.filepath = "unsaved";
                received.data = "";

                display_info(received, 1, raw, false);
            }
            // ResetSignal:
            rcswitch.resetAvailable();
        }

        if (received.key > 0 ||
            received.data.length() > 20) { // RAW data does not have "key", 20 is more than 5 transitions
#ifndef HAS_SCREEN
            // switch to raw mode if decoding failed
            if (received.preset == 0) {
                Serial.println("signal decoding failed, switching to RAW mode");
                // displayWarning("signal decoding failed, switching to RAW mode", true);
                raw = true;
                // TODO: show a dialog/warning?
                // raw = yesNoDialog("decoding failed, save as RAW?");
            }
            String subfile_out = "Filetype: Bruce SubGhz File\nVersion 1\n";
            subfile_out += "Frequency: " + String(int(frequency * 1000000)) + "\n";
            if (!raw) {
                subfile_out += "Preset: " + String(received.preset) + "\n";
                subfile_out += "Protocol: RcSwitch\n";
                subfile_out += "Bit: " + String(received.Bit) + "\n";
                subfile_out += "Key: " + String(hexString) + "\n";
                subfile_out += "TE: " + String(received.te) + "\n";
            } else {
                // save as raw
                if (received.preset == "1") received.preset = "FuriHalSubGhzPresetOok270Async";
                else if (received.preset == "2") received.preset = "FuriHalSubGhzPresetOok650Async";
                subfile_out += "Preset: " + String(received.preset) + "\n";
                subfile_out += "Protocol: RAW\n";
                subfile_out += "RAW_Data: " + received.data;
            }
            // headless mode
            return subfile_out;
#endif

            if (check(SelPress)) {
                int chosen = 0;
                options = {
                    {"Replay signal", [&]() { chosen = 1; }},
                    {"Save signal",   [&]() { chosen = 2; }},
                };
                loopOptions(options);
                if (chosen == 1) {
                    rcswitch.disableReceive();
                    sendRfCommand(received);
                    addToRecentCodes(received);
                    goto RestartRec;
                } else if (chosen == 2) {
                    decimalToHexString(received.key, hexString);
                    RCSwitch_SaveSignal(frequency, received, raw, hexString);

                    delay(2000);
                    drawMainBorder();
                    tft.setCursor(10, 28);
                    tft.setTextSize(FP);
                    tft.println("Waiting for a " + String(frequency) + " MHz " + "signal.");
                }
            }
        }
        // #ifndef HAS_SCREEN
        if (max_loops > 0) {
            // headless mode, quit if nothing received after max_loops
            max_loops -= 1;
            delay(1000);
            if (max_loops == 0) {
                Serial.println("timeout");
                return "";
            }
        }
        // #endif
    }
Exit:
    delay(1);

    deinitRfModule();

    return "";
}
