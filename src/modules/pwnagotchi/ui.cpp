/*
Thanks to thoses developers for their projects:
* @7h30th3r0n3 : https://github.com/7h30th3r0n3/Evil-M5Core2 and https://github.com/7h30th3r0n3/PwnGridSpam
* @viniciusbo : https://github.com/viniciusbo/m5-palnagotchi
* @sduenasg : https://github.com/sduenasg/pio_palnagotchi

Thanks to @bmorcelli for his help doing a better code.
*/

#include <globals.h>
#include <LittleFS.h>
#include <SD.h>
#include "ui.h"
#include "pwnagotchi.h"
#include "../wifi/sniffer.h"
#include <set> // Need to include set header

// Global variable declaration - defined in pwnagotchi.cpp
extern bool skipDeauthOnCapturedHS;

// Global variable definition from main.cpp
extern uint16_t beacon_delay_ms;

#define ROW_SIZE 40
#define PADDING 10

int32_t display_w;
int32_t display_h;
int32_t canvas_h;
int32_t canvas_center_x;
int32_t canvas_top_h;
int32_t canvas_bot_h;
int32_t canvas_peers_menu_h;
int32_t canvas_peers_menu_w;

uint8_t menu_current_cmd = 0;
uint8_t menu_current_opt = 0;

// Settings menu items
const char* SETTING_ITEMS[] = {
    "Beacon Delay",
    "Back"
};
const int NUM_SETTINGS = 2;
int currentSettingItem = 0;

// Off-screen buffer object
static TFT_eSprite settingsSprite(&tft);

// Number of files displayed per page in the list
#define HS_PER_PAGE 4

// Declare Handshake list off-screen buffer object
static TFT_eSprite handshakeSprite(&tft);

void initUi() {
    tft.setTextSize(1);
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextColor(bruceConfig.priColor);

    display_w = tftWidth;
    display_h = tftHeight;
    canvas_h = display_h * .8;
    canvas_center_x = display_w / 2;
    canvas_top_h = display_h * .1;
    canvas_bot_h = display_h * .9;
    canvas_peers_menu_h = display_h * .8;
    canvas_peers_menu_w = display_w * .8;
}

String getRssiBars(signed int rssi) {
    String rssi_bars = "";

    if (rssi != -1000) {
        if (rssi >= -67) {
            rssi_bars = "[####]";
        } else if (rssi >= -70) {
            rssi_bars = "[### ]";
        } else if (rssi >= -80) {
            rssi_bars = "[##  ]";
        } else {
            rssi_bars = "[#   ]";
        }
    }

    return rssi_bars;
}

void drawTime() {
    tft.drawPixel(0, 0, 0);
    tft.fillRect(80, 0, display_w, canvas_top_h - 3, bruceConfig.bgColor);
    tft.setTextDatum(TR_DATUM);
    unsigned long ellapsed = millis() / 1000;
    int8_t h = ellapsed / 3600;
    int sr = ellapsed % 3600;
    int8_t m = sr / 60;
    int8_t s = sr % 60;
    char right_str[50] = "UPS 0%  UP 00:00:00";
    sprintf(right_str, "UPS %i%% UP %02d:%02d:%02d", getBattery(), h, m, s);
    tft.drawString(right_str, display_w, 3);
}

void drawFooterData(uint8_t friends_run, uint8_t friends_tot, String last_friend_name, signed int rssi) {
    tft.drawPixel(0, 0, 0);
    tft.fillRect(0, canvas_bot_h + 1, display_w - 50, canvas_bot_h + 10, bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.setTextColor(bruceConfig.priColor);
    tft.setTextDatum(TL_DATUM);

    String rssi_bars = getRssiBars(rssi);
    String stats = "FRND 0 (0)";
    if (friends_run > 0) {
        stats = "FRND " + String(friends_run) + " (" + String(friends_tot) + ")" + " [" +
                last_friend_name.substring(0, 13) + "] " + rssi_bars;
    }

    tft.drawString(stats, 0, canvas_bot_h + 5);
}

void updateUi(bool show_toolbars) {
    uint8_t mood_id = getCurrentMoodId();
    String mood_face = getCurrentMoodFace();
    String mood_phrase = getCurrentMoodPhrase();
    bool mood_broken = isCurrentMoodBroken();

    // Draw header and footer
    if (show_toolbars) {
        drawTopCanvas();
        drawTime();
        drawFooterData(
            getPwngridRunTotalPeers(),
            getPwngridTotalPeers(),
            getPwngridLastFriendName(),
            getPwngridClosestRssi()
        );
    }

    // Draw mood
    drawMood(mood_face, mood_phrase, mood_broken);

#if defined(HAS_TOUCH)
    TouchFooter();
#endif
}

void drawTopCanvas() {
    tft.drawPixel(0, 0, 0);
    tft.fillRect(0, 0, display_w, canvas_top_h, bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.setTextColor(bruceConfig.priColor);
    tft.setTextDatum(TL_DATUM);
    char buffer[32];
    sprintf(buffer, "CH %02d, HS %d", ch, num_HS);
    tft.drawString(buffer, 0, 3);

    tft.drawLine(0, canvas_top_h - 1, display_w, canvas_top_h - 1, bruceConfig.priColor);
}

void drawBottomCanvas() {
    tft.drawPixel(0, 0, 0);
    tft.fillRect(0, canvas_bot_h, display_w, canvas_bot_h + 10, bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.setTextColor(bruceConfig.priColor);

    tft.setTextDatum(TR_DATUM);
    tft.drawString("NOT AI", display_w, canvas_bot_h + 5);
    tft.drawLine(0, canvas_bot_h, display_w, canvas_bot_h, bruceConfig.priColor);
}

void drawMood(String face, String phrase, bool broken) {
    tft.drawPixel(0, 0, 0);
    tft.setTextColor(bruceConfig.priColor);

    tft.setTextSize(4);
    tft.setTextDatum(MC_DATUM);
    tft.fillRect(0, canvas_top_h + 10, display_w, canvas_bot_h - 40, bruceConfig.bgColor);
    tft.drawCentreString(face, canvas_center_x, canvas_h / 3, SMOOTH_FONT);
    tft.setTextDatum(BC_DATUM);
    tft.setTextSize(1);
    tft.drawCentreString(phrase, canvas_center_x, canvas_h - 30, SMOOTH_FONT);
}

void showPwnagotchiSettings() {
    Serial.println("Entering showPwnagotchiSettings..."); // Debug Start
    tft.fillScreen(bruceConfig.bgColor);
    settingsSprite.createSprite(display_w, display_h);
    settingsSprite.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    settingsSprite.setTextSize(2);
    settingsSprite.fillRect(0, 0, display_w, 40, bruceConfig.bgColor);
    settingsSprite.setCursor(10, 10);
    settingsSprite.print("Pwnagotchi Settings");

    int currentOption = 0; // 0: Beacon Delay, 1: Skip Deauth, 2: Deauth
    int totalOptions = 3;
    bool optionSelected = false; // Track if option is selected for editing

    auto drawMenu = [&]() {
        Serial.println("  Drawing settings menu..."); // Debug Draw
        settingsSprite.fillRect(0, 40, display_w, display_h - 40, bruceConfig.bgColor);
        settingsSprite.setTextSize(1.5);

        // Highlight current option with different color and marker
        settingsSprite.setCursor(20, 50);
        if (currentOption == 0) {
            settingsSprite.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
        } else {
            settingsSprite.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        }
        settingsSprite.printf("%sBeacon Delay: %u ms",
            (currentOption == 0 && optionSelected) ? "* " :
            (currentOption == 0) ? "> " : "  ", beacon_delay_ms);

        settingsSprite.setCursor(20, 80);
        if (currentOption == 1) {
            settingsSprite.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
        } else {
            settingsSprite.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        }
        settingsSprite.printf("%sSkip Deauth Captured APs: %s",
            (currentOption == 1 && optionSelected) ? "* " :
            (currentOption == 1) ? "> " : "  ", skipDeauthOnCapturedHS ? "ON" : "OFF");

        settingsSprite.setCursor(20, 110);
        if (currentOption == 2) {
            settingsSprite.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
        } else {
            settingsSprite.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        }
        settingsSprite.printf("%sDeauth: %s",
            (currentOption == 2 && optionSelected) ? "* " :
            (currentOption == 2) ? "> " : "  ", deauthEnabled ? "ON" : "OFF");

        // Reset text color
        settingsSprite.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

        // Add navigation hints
        settingsSprite.setCursor(20, 140);
        if (optionSelected) {
            settingsSprite.printf("LEFT/RIGHT: adjust, SELECT: confirm");
        } else {
            settingsSprite.printf("UP/DOWN: select, SELECT: edit, ESC: exit");
        }

        Serial.printf("    -> Beacon Delay: %u, Skip Deauth: %s, Deauth: %s\n",
            beacon_delay_ms, skipDeauthOnCapturedHS ? "ON" : "OFF", deauthEnabled ? "ON" : "OFF"); // Debug Values
    };

    drawMenu();
    Serial.println("  Pushing initial settings sprite..."); // Debug Push
    settingsSprite.pushSprite(0, 0);
    Serial.println("  Initial push done."); // Debug Push

    while (true) {
        Serial.printf("Settings Loop - currentOption: %d, selected: %d\n", currentOption, optionSelected); // Debug Loop
        bool updated = false;

        // Check all button states
        bool prev = check(PrevPress); // UP and LEFT according to your hardware
        bool next = check(NextPress); // DOWN and RIGHT according to your hardware
        bool sel = check(SelPress);   // SELECT
        bool esc = check(EscPress);   // ESCAPE

        if(prev || next || sel || esc) {
            Serial.printf("  Button Check: Prev(Up/Left)=%d, Next(Down/Right)=%d, Sel=%d, Esc=%d\n",
                prev, next, sel, esc); // Debug Buttons
        }

        // Navigation logic - different behavior based on edit mode
        if (!optionSelected) {
            // Navigation mode - use prev/next to navigate menu items
            if (prev) {  // UP key
                if (currentOption > 0) {
                    Serial.println("  Option moving UP."); // Debug Nav
                    currentOption--;
                    updated = true;
                }
            }
            if (next) {  // DOWN key
                if (currentOption < totalOptions - 1) {
                    Serial.println("  Option moving DOWN."); // Debug Nav
                    currentOption++;
                    updated = true;
                }
            }

            // Enter edit mode when select is pressed
            if (sel) {
                optionSelected = true;
                updated = true;
                Serial.println("  Option selected for editing."); // Debug Edit
            }
        } else {
            // Edit mode - use prev/next to adjust values
            switch (currentOption) {
                case 0: // Beacon Delay
                    if (next) { // RIGHT key
                        Serial.printf("  Adjusting Beacon Delay (Current: %u) +100\n", beacon_delay_ms);
                        if (beacon_delay_ms < 2000) beacon_delay_ms += 100;
                        else beacon_delay_ms = 100;
                        updated = true;
                    } else if (prev) { // LEFT key
                        Serial.printf("  Adjusting Beacon Delay (Current: %u) -100\n", beacon_delay_ms);
                        if (beacon_delay_ms > 100) beacon_delay_ms -= 100;
                        else beacon_delay_ms = 2000;
                        updated = true;
                    }
                    break;

                case 1: // Skip Deauth Captured APs
                    if (next || prev) { // LEFT or RIGHT key to toggle
                        Serial.printf("  Toggling Skip Deauth (Current: %s)\n", skipDeauthOnCapturedHS ? "ON" : "OFF");
                        skipDeauthOnCapturedHS = !skipDeauthOnCapturedHS;
                        updated = true;
                    }
                    break;

                case 2: // Deauth
                    if (next || prev) { // LEFT or RIGHT key to toggle
                        Serial.printf("  Toggling Deauth (Current: %s)\n", deauthEnabled ? "ON" : "OFF");
                        deauthEnabled = !deauthEnabled;
                        updated = true;
                    }
                    break;
            }

            // Exit edit mode when select is pressed again
            if (sel) {
                optionSelected = false;
                updated = true;
                Serial.println("  Exiting edit mode."); // Debug Edit
            }
        }

        if (updated) {
            Serial.println("  Settings updated, redrawing menu..."); // Debug Update
            drawMenu();
            Serial.println("  Pushing updated settings sprite..."); // Debug Push
            settingsSprite.pushSprite(0, 0);
            Serial.println("  Update push done."); // Debug Push
        }

        if (esc) {
            // If in edit mode, exit edit mode first
            if (optionSelected) {
                optionSelected = false;
                updated = true;
                Serial.println("  Exiting edit mode due to ESC."); // Debug Edit
                drawMenu();
                settingsSprite.pushSprite(0, 0);
            } else {
                Serial.println("Exiting showPwnagotchiSettings..."); // Debug Exit
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    settingsSprite.deleteSprite();
    tft.fillScreen(bruceConfig.bgColor);
    updateUi(true);
}

// Helper function to format MAC address
String formatMac(const String& rawMac) {
    String formatted = "";
    for (int i = 0; i < 12; i += 2) {
        formatted += rawMac.substring(i, i + 2);
        if (i < 10) {
            formatted += ":";
        }
    }
    return formatted;
}

void showCapturedHandshakes() {
    Serial.println("Entering showCapturedHandshakes..."); // Debug Start
    tft.fillScreen(bruceConfig.bgColor); // Always clear the screen first
    bool useSD = setupSdCard();
    Serial.printf("  Using SD Card: %s\n", useSD ? "Yes" : "No (LittleFS)"); // Debug FS Type
    FS* fs_ptr = useSD ? (FS*)&SD : (FS*)&LittleFS;
    const char* dirPath = "/BrucePCAP/handshakes";
    Serial.printf("  Attempting to open directory: %s\n", dirPath); // Debug Dir Open
    File dir = fs_ptr->open(dirPath);

    if (!dir || !dir.isDirectory()) {
        Serial.println("  Error opening directory or not a directory."); // Debug Dir Error
        tft.fillScreen(bruceConfig.bgColor); // Ensure background is cleared
        Serial.println("  Calling displayInfo for directory error."); // Debug Info Call
        displayInfo(useSD ? "SD HS Dir Error" : "FS HS Dir Error");
        if(dir) dir.close();
        Serial.println("Exiting showCapturedHandshakes due to dir error."); // Debug Exit
        return;
    }

    Serial.println("  Directory opened successfully. Reading files..."); // Debug Dir OK
    std::set<String> capturedMacs;
    File entry;
    while ((entry = dir.openNextFile())) {
        String entryName = entry.name(); // Store name for debug
        if (!entry.isDirectory()) {
            String name = entryName;
            int p = name.lastIndexOf('/');
            if (p >= 0) name = name.substring(p + 1);
            //Serial.printf("    Found file: %s, checking format...\n", name.c_str()); // Debug File Found (potentially verbose)
            if (name.startsWith("HS_") && name.endsWith(".pcap") && name.length() >= 20) {
                String rawMac = name.substring(3, 3 + 12);
                bool validHex = true;
                for (char c : rawMac) { if (!isxdigit(c)) { validHex = false; break; } }
                if (validHex) {
                    String formattedMac = formatMac(rawMac);
                    //Serial.printf("      -> Valid HS file found for MAC: %s\n", formattedMac.c_str()); // Debug Valid HS (potentially verbose)
                    capturedMacs.insert(formattedMac);
                } else {
                   // Serial.printf("      -> Invalid hex in MAC part: %s\n", rawMac.c_str()); // Debug Invalid Hex (potentially verbose)
                }
            } else {
                //Serial.printf("      -> Filename format mismatch.\n"); // Debug Format Mismatch (potentially verbose)
            }
        }
        entry.close();
    }
    dir.close();
    Serial.printf("  Finished reading directory. Found %d unique handshakes.\n", capturedMacs.size()); // Debug Count

    if (capturedMacs.empty()) {
        Serial.println("  No valid handshakes found."); // Debug Empty
        tft.fillScreen(bruceConfig.bgColor); // Ensure background is cleared
        Serial.println("  Calling displayInfo for no handshakes."); // Debug Info Call
        displayInfo("No handshakes");
        Serial.println("Exiting showCapturedHandshakes because none found."); // Debug Exit
        return;
    }

    Serial.println("  Handshakes found, preparing display..."); // Debug Found HS
    std::vector<String> macList(capturedMacs.begin(), capturedMacs.end());
    int page = 0;
    int pageCount = (macList.size() + HS_PER_PAGE - 1) / HS_PER_PAGE;
    Serial.printf("  Total pages: %d\n", pageCount); // Debug Pages
    handshakeSprite.createSprite(display_w, display_h);

    auto drawPage = [&]() {
        // Debug: Print colors
        Serial.printf("  Drawing handshake page %d / %d - BGColor: 0x%04X, PriColor: 0x%04X\n",
                      page + 1, pageCount, bruceConfig.bgColor, bruceConfig.priColor);

        handshakeSprite.fillScreen(bruceConfig.bgColor);

        // --- Start Simple Drawing Test ---
        // handshakeSprite.drawRect(20, 40, 100, 50, TFT_WHITE); // Draw a simple white rectangle // Remove this line
        // Serial.println("    Drew simple test rectangle."); // Remove this line
        // --- End Simple Drawing Test ---

        /* --- Temporarily Disabled Original Drawing --- */ // Remove this line
        handshakeSprite.setTextSize(2);
        handshakeSprite.setTextColor(bruceConfig.priColor); // Use configured primary color
        handshakeSprite.setCursor(10, 10);
        handshakeSprite.print("Captured MACs");
        handshakeSprite.setTextSize(1);
        handshakeSprite.drawLine(0, 35, display_w, 35, bruceConfig.priColor); // Use configured primary color
        for (int i = 0; i < HS_PER_PAGE; i++) {
            int idx = page * HS_PER_PAGE + i;
            if (idx >= macList.size()) break;
            Serial.printf("    Displaying MAC: %s\n", macList[idx].c_str()); // Debug Display MAC
            handshakeSprite.setCursor(20, 50 + i * 20);
            handshakeSprite.print(macList[idx]);
        }
        // Fix: Clear the area before drawing the page number to prevent overlap
        int pageNumX = display_w - 70;
        int pageNumY = display_h - 15;
        int pageNumW = 70; // Estimated width for the text area
        int pageNumH = 15; // Estimated height for the text area
        handshakeSprite.fillRect(pageNumX, pageNumY, pageNumW, pageNumH, bruceConfig.bgColor);

        // Draw the page number
        handshakeSprite.setCursor(pageNumX, pageNumY);
        handshakeSprite.printf("%d/%d", page + 1, pageCount);
        /* */ // Remove this line // --- End Temporarily Disabled Original Drawing --- */ // Remove this line

        Serial.println("    Pushing handshake sprite..."); // Updated log message
        handshakeSprite.pushSprite(0, 0);
        Serial.println("    Handshake push done."); // Debug Push
    };

    drawPage();
    vTaskDelay(pdMS_TO_TICKS(50)); // Keep a small delay
    drawPage();

    while (true) {
        //Serial.println("Handshake Loop..."); // Debug Loop (optional)
        bool updated = false;
        bool next = check(NextPress);
        bool prev = check(PrevPress);
        bool esc = check(EscPress);

        if(next || prev || esc) {
             Serial.printf("  Button Check: Next=%d, Prev=%d, Esc=%d\n", next, prev, esc); // Debug Buttons
        }

        if (next) {
             if (page + 1 < pageCount) {
                 Serial.println("  Going to next page."); // Debug Nav
                 page++; updated = true;
             }
        }
        if (prev) {
            if (page > 0) {
                Serial.println("  Going to previous page."); // Debug Nav
                page--; updated = true;
            }
        }
        if (updated) {
            Serial.println("  Page changed, redrawing..."); // Debug Update
            drawPage();
        }
        if (esc) {
             Serial.println("Exiting showCapturedHandshakes..."); // Debug Exit
             break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    handshakeSprite.deleteSprite();
    tft.fillScreen(bruceConfig.bgColor);
    updateUi(true);
}

// Use the same file handling method as in showCapturedHandshakes to update the SavedHS display
void updateSavedHSList() {
    bool useSD = setupSdCard();
    FS* fs_ptr = useSD ? (FS*)&SD : (FS*)&LittleFS;
    const char* dirPath = "/BrucePCAP/handshakes";
    File dir = fs_ptr->open(dirPath);
    SavedHS.clear();  // Fix: Rename savedHSList to SavedHS (assuming SavedHS is the correct global list)

    if (dir && dir.isDirectory()) {
        File entry;
        while ((entry = dir.openNextFile())) {
            if (!entry.isDirectory()) {
                String name = entry.name();
                int p = name.lastIndexOf('/');
                if (p >= 0) name = name.substring(p + 1);

                if (name.startsWith("HS_") && name.endsWith(".pcap") && name.length() >= 20) {
                    String rawMac = name.substring(3, 3 + 12);
                    bool validHex = true;
                    for (char c : rawMac) {
                        if (!isxdigit(c)) {
                            validHex = false;
                            break;
                        }
                    }

                    if (validHex) {
                        String formattedMac = formatMac(rawMac);
                        SavedHS.insert(formattedMac); // Fix: Use insert() for std::set instead of push_back()
                    }
                }
            }
            entry.close();
        }
        dir.close();
    }

    Serial.printf("Updated SavedHS list, now contains %d items\n", SavedHS.size()); // Fix: Use SavedHS
}

// TODO: Include necessary header for keyboard library (e.g., M5Cardputer.h or similar)
// TODO: Ensure 'keyboard' object is declared (e.g., extern Keyboard keyboard;) if defined globally elsewhere
// TODO: Ensure KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_ESC constants are defined in included headers

// Check if the function check() properly determines button presses
bool check(uint8_t button_id) { // Rename parameter to avoid conflict with potential variable name
    static uint32_t lastPressTime[10] = {0}; // Use a more descriptive name
    const uint32_t debounceDelay = 200; // Use a constant for debounce time

    // Check debounce time
    if (millis() - lastPressTime[button_id] < debounceDelay) {
        return false;
    }

    bool pressed = false;

    // Fix: Replace switch with if-else if structure because case labels must be constant expressions
    // We assume button_id maps to the intended button check logic.
    // The mapping from button_id to PrevPress, NextPress etc. needs clarification.
    // Assuming button_id corresponds to the constants defined elsewhere (e.g., 0 for PrevPress, 1 for NextPress, etc.)
    // For now, using a placeholder check based on the original logic structure. This needs verification.

    #if defined(HAS_KEYBOARD)
    // This section needs the 'keyboard' object and key constants to be defined.
    // Example structure (replace with actual button_id mapping logic):
    if (button_id == 0 /* Placeholder for PrevPress ID */) {
         // pressed = keyboard.isKeyPressed(KEY_UP) || keyboard.isKeyPressed(KEY_LEFT); // Needs KEY_UP, KEY_LEFT, keyboard
         Serial.println("PrevPress check placeholder"); // Placeholder
    } else if (button_id == 1 /* Placeholder for NextPress ID */) {
         // pressed = keyboard.isKeyPressed(KEY_DOWN) || keyboard.isKeyPressed(KEY_RIGHT); // Needs KEY_DOWN, KEY_RIGHT, keyboard
         Serial.println("NextPress check placeholder"); // Placeholder
    } else if (button_id == 2 /* Placeholder for SelPress ID */) {
         // pressed = keyboard.isKeyPressed(KEY_ENTER); // Needs KEY_ENTER, keyboard
         Serial.println("SelPress check placeholder"); // Placeholder
    } else if (button_id == 3 /* Placeholder for EscPress ID */) {
         // pressed = keyboard.isKeyPressed(KEY_ESC); // Needs KEY_ESC, keyboard
         Serial.println("EscPress check placeholder"); // Placeholder
    }
    // ... add other button checks as needed
    #else
    // Fallback or alternative input method if no keyboard
    Serial.println("Warning: HAS_KEYBOARD not defined, button check inactive.");
    #endif


    if (pressed) {
        lastPressTime[button_id] = millis(); // Update last press time
        return true;
    }

    return false;
}
