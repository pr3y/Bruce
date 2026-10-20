// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rolling_code.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "protocols/rf_encoder.h"
#include "protocols/rf_secplus.h"
#include "rf_utils.h"
#include <Preferences.h>
#include <esp_system.h>
#include <limits.h>

namespace {
constexpr uint32_t PROFILE_MAGIC = 0x53500101;
constexpr int PROFILE_COUNT = 4;

void showMenu(std::vector<Option> &menu, const char *title) {
    // Serial/app navigation reads the global options list. Keep a copy there,
    // while the local vector owns callbacks throughout nested menu calls.
    options = menu;
    loopOptions(menu, MENU_TYPE_SUBMENU, title);
    options.clear();
}

struct Profile {
    uint64_t fixed;
    uint32_t magic;
    uint32_t next;
    uint32_t hz;
    uint8_t v2;
    char name[19];
};

String slotKey(int slot) { return "slot" + String(slot); }
bool valid(const Profile &p) {
    return p.magic == PROFILE_MAGIC && p.v2 <= 1 && p.name[0] && p.name[18] == 0 &&
           (p.hz == 310000000 || p.hz == 315000000 || p.hz == 315150000 ||
            p.hz == 390000000 || p.hz == 433920000) &&
           (p.v2 ? p.fixed < (1ULL << 40) && p.next <= 0x10000000 : p.fixed < 3486784401ULL);
}
bool readProfile(Preferences &prefs, int slot, Profile &p) {
    String key = slotKey(slot);
    return prefs.getBytesLength(key.c_str()) == sizeof(p) &&
           prefs.getBytes(key.c_str(), &p, sizeof(p)) == sizeof(p) && valid(p);
}
bool saveProfile(Preferences &prefs, int slot, const Profile &p) {
    String key = slotKey(slot);
    return prefs.putBytes(key.c_str(), &p, sizeof(p)) == sizeof(p);
}

bool deleteProfile(Preferences &prefs, int slot) {
    bool erase = false;
    std::vector<Option> confirm = {{"Cancel", [] {}}, {"Delete", [&] { erase = true; }}};
    showMenu(confirm, "Requires pairing again");
    if (!erase) return false;
    if (prefs.remove(slotKey(slot).c_str())) return true;
    displayError("Delete failed", true);
    return false;
}

void sendProfile(Preferences &prefs, int slot, Profile &p) {
    const uint32_t limit = p.v2 ? 0x0fffffff : UINT32_MAX - 2;
    if (p.next > limit) {
        displayError("Counter exhausted: pair a new remote", true);
        return;
    }
    std::vector<int> durations;
    if (!rf_secplus_durations(p.v2 != 0, p.next, p.fixed, durations)) {
        displayError("Invalid remote", true);
        return;
    }
    if (!initRfModule("", p.hz / 1000000.0f)) {
        deinitRfModule();
        return;
    }
    ELECHOUSE_cc1101.setModulation(2);
    ELECHOUSE_cc1101.setPktFormat(3);
    pinMode(bruceConfigPins.CC1101_bus.io0, OUTPUT);
    digitalWrite(bruceConfigPins.CC1101_bus.io0, LOW);
    ELECHOUSE_cc1101.setPA(12);

    // Reserve the next counter durably BEFORE enabling TX. A reset or a failed
    // transmission may skip a value, but must never reuse an emitted counter.
    Profile reserved = p;
    reserved.next += p.v2 ? 1 : 2;
    if (!saveProfile(prefs, slot, reserved)) {
        deinitRfModule();
        displayError("Counter save failed; nothing sent", true);
        return;
    }
    p = reserved;
    ioExpander.turnPinOnOff(IO_EXP_CC_RX, LOW);
    ioExpander.turnPinOnOff(IO_EXP_CC_TX, HIGH);
    ELECHOUSE_cc1101.SetTx();
    bool ok = rf_tx_durations(durations);
    deinitRfModule();
    if (ok) displaySuccess("Sent", true);
    else displayError("TX failed; counter advanced", true);
}

void remoteMenu(Preferences &prefs, int slot, Profile &p) {
    bool done = false;
    while (!done && !returnToMenu) {
        std::vector<Option> menu = {
            {"Send", [&] { done = false; sendProfile(prefs, slot, p); }},
            {"Pair / Learn", [&] {
                 done = false;
                 displayInfo("Press opener Learn, then select Send", true);
             }},
            {"Details", [&] {
                 done = false;
                 char text[150];
                 snprintf(text, sizeof(text), "%s  %.3f MHz  ID: %010llX  Next counter: %lu",
                          p.v2 ? "Security+ 2.0" : "Security+", p.hz / 1000000.0,
                          (unsigned long long)p.fixed, (unsigned long)p.next);
                 displayInfo(text, true);
             }},
            {"Delete remote", [&] {
                 done = deleteProfile(prefs, slot);
             }},
            {"Back", [&] { done = true; }},
        };
        // Escape leaves this screen, just like the explicit Back entry.
        done = true;
        showMenu(menu, p.name);
    }
}

void createProfile(Preferences &prefs, int slot) {
    Profile p = {};
    int protocol = -1;
    std::vector<Option> protocols = {
        {"Security+", [&] { protocol = 0; }},
        {"Security+ 2.0", [&] { protocol = 1; }},
        {"Cancel", [] {}},
    };
    showMenu(protocols, "New paired remote");
    if (protocol < 0 || returnToMenu) return;
    p.v2 = protocol;
    std::vector<Option> frequencies;
    const uint32_t hz[] = {310000000, 315000000, 315150000, 390000000, 433920000};
    for (uint32_t f : hz) frequencies.emplace_back(String(f / 1000000.0, 3) + " MHz", [&, f] { p.hz = f; });
    frequencies.emplace_back("Cancel", [] {});
    showMenu(frequencies, "Match opener frequency");
    if (!p.hz || returnToMenu) return;
    String name = keyboard("Garage", 18, "Remote name");
    if (name.isEmpty() || name == String((char)0x1b) || returnToMenu) return;
    name.toCharArray(p.name, sizeof(p.name));
    p.magic = PROFILE_MAGIC;
    p.next = 0;
    // A fresh transmitter identity. v1 id1=1 marks a remote, rather than a
    // keypad; v2 uses button 0 and a random 32-bit remote identity.
    p.fixed = p.v2 ? (uint64_t)esp_random() : (uint64_t)(esp_random() % 129140163U) * 27 + 10;
    if (!saveProfile(prefs, slot, p)) {
        displayError("Could not save remote", true);
        return;
    }
    displayInfo("Saved. Pair using opener Learn button", true);
}
} // namespace

void rollingCodeMenu() {
    returnToMenu = false;
    if (bruceConfigPins.rfModule != CC1101_SPI_MODULE) {
        displayError("Select CC1101 in RF Config", true);
        return;
    }
    Preferences prefs;
    if (!prefs.begin("rolling-code", false)) {
        displayError("Remote storage unavailable", true);
        return;
    }
    bool again = true;
    while (again && !returnToMenu) {
        again = false;
        std::vector<Option> menu;
        int freeSlot = -1;
        for (int slot = 0; slot < PROFILE_COUNT; ++slot) {
            Profile p = {};
            if (readProfile(prefs, slot, p)) {
                menu.emplace_back(p.name, [&, slot, p]() mutable {
                    again = true;
                    remoteMenu(prefs, slot, p);
                });
            } else if (prefs.isKey(slotKey(slot).c_str())) {
                // Never silently replace an unreadable identity or reset its
                // counter. Let the user explicitly delete the damaged slot.
                menu.emplace_back("Unreadable remote " + String(slot + 1), [&, slot] {
                    again = true;
                    deleteProfile(prefs, slot);
                });
            } else if (freeSlot < 0) freeSlot = slot;
        }
        if (freeSlot >= 0) menu.emplace_back("New remote", [&, freeSlot] {
            again = true;
            createProfile(prefs, freeSlot);
        });
        menu.emplace_back("Back", [] {});
        showMenu(menu, "Rolling Code");
    }
    prefs.end();
}
