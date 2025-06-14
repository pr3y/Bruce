#include "rf_bruteforce.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"

#include "protocols/Ansonic.h"
#include "protocols/Came.h"
#include "protocols/Chamberlain.h"
#include "protocols/Holtek.h"
#include "protocols/Liftmaster.h"
#include "protocols/Linear.h"
#include "protocols/NiceFlo.h"
#include "protocols/protocol.h"
#include "rf_utils.h"
#include <SPIFFS.h>

float brute_frequency = 433.92;
String brute_protocol = "Nice 12 Bit";
int brute_repeats = 1;
bool is_paused = false;
uint32_t current_code = 0;
std::vector<SavedFrequency> saved_frequencies;

void rf_brute_frequency() {
    options = {};
    int ind = 0;
    int arraySize = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
    for (int i = 0; i < arraySize; i++) {
        String tmp = String(subghz_frequency_list[i], 2) + "Mhz";
        options.push_back({tmp.c_str(), [=]() { brute_frequency = subghz_frequency_list[i]; }});
    }
    loopOptions(options, ind);
    options.clear();
}

void rf_brute_protocol() {
    const String protocol_list[] = {
        "Came 12 Bit",
        "Nice 12 Bit",
        "Ansonic 12 Bit",
        "Holtek 12 Bit",
        "Linear 12 Bit",
        "Liftmaster 12 Bit",
        "Chamberlain 12 Bit",
    };

    options = {};
    int ind = 0;
    int arraySize = sizeof(protocol_list) / sizeof(protocol_list[0]);
    for (int i = 0; i < arraySize; i++) {
        String tmp = protocol_list[i];
        options.push_back({tmp.c_str(), [=]() {
                               brute_protocol = protocol_list[i];
                               current_code = 0; // Reset code when changing protocol
                           }});
    }
    loopOptions(options, ind);
    options.clear();
}

void rf_brute_repeats() {
    const int protocol_list[] = {1, 2, 3, 4, 5};

    options = {};
    int ind = 0;
    int arraySize = sizeof(protocol_list) / sizeof(protocol_list[0]);
    for (int i = 0; i < arraySize; i++) {
        int tmp = protocol_list[i];
        options.push_back({String(tmp).c_str(), [=]() { brute_repeats = protocol_list[i]; }});
    }
    loopOptions(options, ind);
    options.clear();
}

void rf_save_frequency() {
    FS *fs = nullptr;

    // Ask for a custom name for this frequency
    String custom_name = keyboard("Signal", 30, "Enter name for this signal:");

    // Choose storage
    bool sdCardAvailable = setupSdCard();
    bool littleFsAvailable = checkLittleFsSize();

    if (sdCardAvailable && littleFsAvailable) {
        // ask to choose one
        options = {
            {"SD Card",  [&]() { fs = &SD; }      },
            {"LittleFS", [&]() { fs = &LittleFS; }},
        };

        loopOptions(options);
    } else if (sdCardAvailable) {
        fs = &SD;
    } else if (littleFsAvailable) {
        fs = &LittleFS;
    }

    if (!fs) {
        displayError("No storage available", true);
        return;
    }

    if (!fs->exists("/BruceRF")) {
        if (!fs->mkdir("/BruceRF")) {
            displayError("Error creating directory", true);
            return;
        }
    }

    if (!fs->exists("/BruceRF/bruteforce")) {
        if (!fs->mkdir("/BruceRF/bruteforce")) {
            displayError("Error creating directory", true);
            return;
        }
    }

    SavedFrequency freq;
    freq.code = current_code;
    freq.frequency = brute_frequency;
    freq.protocol = brute_protocol;
    freq.timestamp = millis();
    freq.custom_name = custom_name; // Save the custom name
    saved_frequencies.push_back(freq);

    // Save to frequencies.txt
    File indexFile = fs->open("/BruceRF/bruteforce/frequencies.txt", FILE_APPEND);
    if (indexFile) {
        // Save in CSV format: custom_name,code,frequency,protocol,timestamp
        indexFile.printf(
            "%s,%u,%.2f,%s,%lu\n",
            custom_name.c_str(),
            freq.code,
            freq.frequency,
            freq.protocol.c_str(),
            freq.timestamp
        );
        indexFile.close();
        displaySuccess("Signal saved as: " + custom_name, true);
    } else {
        displayError("Error saving signal", true);
    }
}

void rf_load_frequencies() {
    FS *fs = nullptr;
    if (!getFsStorage(fs)) {
        displayError("No storage available", true);
        return;
    }

    // Clear existing frequencies
    saved_frequencies.clear();

    // Check if file exists
    if (!fs->exists("/BruceRF/bruteforce/frequencies.txt")) {
        displayWarning("No saved frequencies found", true);
        return;
    }

    File file = fs->open("/BruceRF/bruteforce/frequencies.txt", FILE_READ);
    if (!file) {
        displayError("Error opening file", true);
        return;
    }

    // Read each line and parse frequencies
    while (file.available()) {
        String line = file.readStringUntil('\n');
        SavedFrequency freq;

        // Parse CSV format: custom_name,code,frequency,protocol,timestamp
        int firstComma = line.indexOf(',');
        int secondComma = line.indexOf(',', firstComma + 1);
        int thirdComma = line.indexOf(',', secondComma + 1);
        int fourthComma = line.indexOf(',', thirdComma + 1);

        if (firstComma != -1 && secondComma != -1 && thirdComma != -1 && fourthComma != -1) {
            freq.custom_name = line.substring(0, firstComma);
            freq.code = strtoul(line.substring(firstComma + 1, secondComma).c_str(), nullptr, 10);
            freq.frequency = line.substring(secondComma + 1, thirdComma).toFloat();
            freq.protocol = line.substring(thirdComma + 1, fourthComma);
            freq.timestamp = strtoul(line.substring(fourthComma + 1).c_str(), nullptr, 10);
            saved_frequencies.push_back(freq);
        }
    }

    file.close();
}

void rf_save_frequencies() {
    FS *fs = nullptr;
    if (!getFsStorage(fs)) {
        displayError("No storage available", true);
        return;
    }

    File file = fs->open("/BruceRF/bruteforce/frequencies.txt", FILE_WRITE);
    if (!file) {
        displayError("Error opening file", true);
        return;
    }

    for (const auto &freq : saved_frequencies) {
        file.printf("%u,%.2f,%s,%lu\n", freq.code, freq.frequency, freq.protocol.c_str(), freq.timestamp);
    }

    file.close();
    displaySuccess("Frequencies saved", true);
}

void rf_replay_frequency() {
    if (saved_frequencies.empty()) {
        rf_load_frequencies();
        if (saved_frequencies.empty()) {
            displayWarning("No saved frequencies found", true);
            return;
        }
    }

    options.clear();
    for (const auto &freq : saved_frequencies) {
        // Use the custom name for display
        options.push_back({freq.custom_name.c_str(), [freq]() {
                               brute_frequency = freq.frequency;
                               brute_protocol = freq.protocol;
                               current_code = freq.code;
                               is_paused = true; // Start paused to show the loaded signal
                               rf_brute_start(); // Start the bruteforcer immediately
                           }});
    }
    options.push_back({"Menu", []() {}});

    loopOptions(options);
    options.clear();
}

void rf_step_forward() {
    if (current_code < ((1 << 12) - 1)) { // Assuming 12-bit max
        current_code++;
    }
}

void rf_step_backward() {
    if (current_code > 0) { current_code--; }
}

void rf_toggle_pause() { is_paused = !is_paused; }

bool rf_brute_start() {
    int txpin;

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
        txpin = bruceConfigPins.CC1101_bus.io0;
        if (!initRfModule("tx", brute_frequency)) return false;
    } else {
        txpin = bruceConfig.rfTx;
        if (!initRfModule("tx")) return false;
    }

    c_rf_protocol *protocol = nullptr;
    int bits = 0;

    if (brute_protocol == "Nice 12 Bit") {
        protocol = new protocol_nice_flo();
        bits = 12;
    } else if (brute_protocol == "Came 12 Bit") {
        protocol = new protocol_came();
        bits = 12;
    } else if (brute_protocol == "Ansonic 12 Bit") {
        protocol = new protocol_ansonic();
        bits = 12;
    } else if (brute_protocol == "Holtek 12 Bit") {
        protocol = new protocol_holtek();
        bits = 12;
    } else if (brute_protocol == "Linear 12 Bit") {
        protocol = new protocol_linear();
        bits = 12;
    } else if (brute_protocol == "Liftmaster 12 Bit") {
        protocol = new protocol_ansonic();
        bits = 12;
    } else if (brute_protocol == "Chamberlain 12 Bit") {
        protocol = new protocol_ansonic();
        bits = 12;
    } else {
        deinitRfModule();
        return false;
    }

    pinMode(txpin, OUTPUT);
    setMHZ(brute_frequency);

    // Function to transmit a specific code
    auto transmit_code = [&](uint32_t code, int repeat_count) {
        for (int r = 0; r < repeat_count; ++r) {
            for (int j = bits - 1; j >= 0; --j) {
                bool high = (code >> j) & 1;
                if (high) {
                    digitalWrite(txpin, HIGH);
                    delayMicroseconds(protocol->timing_high);
                    digitalWrite(txpin, LOW);
                    delayMicroseconds(protocol->timing_low);
                } else {
                    digitalWrite(txpin, HIGH);
                    delayMicroseconds(protocol->timing_low);
                    digitalWrite(txpin, LOW);
                    delayMicroseconds(protocol->timing_high);
                }
            }
            delay(30);
        }
    };

    // Create sprite for double buffering
    TFT_eSprite *sprite = new TFT_eSprite(&tft);
    sprite->createSprite(tftWidth - 20, tftHeight - 40);
    sprite->setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    enum MenuButton { BTN_ARROWS, BTN_START, BTN_SAVE, BTN_EMULATE, BTN_EMULATE_X5, BTN_COUNT };
    MenuButton selected_button = BTN_START;
    bool in_code_edit = false;

    // Function to draw a button box with text
    auto draw_button = [&](const String &text,
                           int x,
                           int y,
                           int width,
                           int height,
                           bool is_selected,
                           bool is_centered = true,
                           uint16_t custom_color = 0) {
        uint16_t text_color = custom_color ? custom_color : bruceConfig.priColor;

        // Draw box - simplified highlighting for better performance
        if (is_selected) {
            // Single thicker border for selected items
            sprite->drawRoundRect(x - 2, y - 2, width + 4, height + 4, 3, text_color);
        }
        sprite->drawRoundRect(x, y, width, height, 3, text_color);

        // Set text properties
        sprite->setTextDatum(is_centered ? MC_DATUM : ML_DATUM);
        sprite->setTextColor(text_color, bruceConfig.bgColor);

        // Calculate text position (add small padding)
        int text_x = is_centered ? x + width / 2 : x + 8;
        int text_y = y + height / 2;

        // Draw text
        sprite->drawString(text, text_x, text_y);

        // Reset text properties
        sprite->setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    };

    // Function to display status
    auto display_status = [&](bool is_paused,
                              bool sending = false,
                              int repeat_count = 0,
                              const String &message = "") {
        const int SCREEN_WIDTH = tftWidth - 20;   // Available width
        const int SCREEN_HEIGHT = tftHeight - 40; // Available height

        sprite->fillSprite(bruceConfig.bgColor);
        sprite->drawRoundRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 5, bruceConfig.priColor);

        // Protocol info at the top
        sprite->setTextDatum(TL_DATUM);
        sprite->setTextSize(1);
        sprite->drawString("Protocol:", 10, 10);
        sprite->drawString(brute_protocol, 10, 25);

        // Frequency at top right
        sprite->setTextDatum(TR_DATUM);
        sprite->drawString(String(brute_frequency, 2) + " MHz", SCREEN_WIDTH - 10, 10);

        // Start/Pause button - centered and higher
        const int START_BTN_WIDTH = 50;
        const int BTN_HEIGHT = 18;
        bool is_start_selected = is_paused && selected_button == BTN_START;
        draw_button(
            is_paused ? "Start" : "Pause",
            (SCREEN_WIDTH - START_BTN_WIDTH) / 2,
            25, // x, y - moved higher
            START_BTN_WIDTH,
            BTN_HEIGHT, // width, height - smaller
            is_start_selected
        ); // is_selected

        // Code counter in center with arrows
        sprite->setTextSize(2);
        const int centerY = SCREEN_HEIGHT / 2;
        const int ARROW_SIZE = 22;
        const int CODE_DISPLAY_WIDTH = 120; // Approximate width needed for code display

        // Calculate positions to center the entire code display + arrows
        const int totalWidth =
            CODE_DISPLAY_WIDTH + (2 * (ARROW_SIZE + 15)); // 15 pixels padding between elements
        const int startX = (SCREEN_WIDTH - totalWidth) / 2;

        // Left arrow
        uint16_t arrow_color = in_code_edit                                   ? TFT_GREEN
                               : (is_paused && selected_button == BTN_ARROWS) ? bruceConfig.priColor
                                                                              : bruceConfig.priColor;

        draw_button(
            "<",
            startX,
            centerY - 11, // x, y
            ARROW_SIZE,
            ARROW_SIZE,                                      // width, height
            (selected_button == BTN_ARROWS || in_code_edit), // is_selected
            true,                                            // is_centered
            arrow_color
        ); // custom_color

        // Code
        sprite->setTextDatum(MC_DATUM);
        sprite->drawString(String(current_code) + "/" + String((1 << bits) - 1), SCREEN_WIDTH / 2, centerY);

        // Right arrow
        draw_button(
            ">",
            startX + totalWidth - ARROW_SIZE,
            centerY - 11, // x, y
            ARROW_SIZE,
            ARROW_SIZE,                                      // width, height
            (selected_button == BTN_ARROWS || in_code_edit), // is_selected
            true,                                            // is_centered
            arrow_color
        ); // custom_color

        sprite->setTextSize(1);

        // Status message (restored to original position)
        sprite->setTextDatum(MC_DATUM);
        if (!message.isEmpty()) {
            sprite->setTextColor(TFT_GREEN, bruceConfig.bgColor);
            sprite->drawString(message, SCREEN_WIDTH / 2, centerY + 25);
        } else if (sending) {
            sprite->setTextColor(TFT_GREEN, bruceConfig.bgColor);
            sprite->drawString("Sending x" + String(repeat_count), SCREEN_WIDTH / 2, centerY + 25);
        } else {
            sprite->drawString(is_paused ? "PAUSED" : "Running...", SCREEN_WIDTH / 2, centerY + 25);
        }
        sprite->setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

        // Bottom buttons (only if paused)
        if (is_paused) {
            const int BOTTOM_BTN_WIDTH = 55;  // Increased from 45 to 55
            const int EMULATE_BTN_WIDTH = 65; // Already increased for Emulate x5
            const int BOTTOM_Y = SCREEN_HEIGHT - 25;

            // Save button - left aligned
            draw_button(
                "Save",
                10,
                BOTTOM_Y, // x, y
                BOTTOM_BTN_WIDTH,
                BTN_HEIGHT, // width, height
                selected_button == BTN_SAVE
            ); // is_selected

            // Emulate buttons - right aligned, side by side
            draw_button(
                "Resend x5",
                SCREEN_WIDTH - (EMULATE_BTN_WIDTH + BOTTOM_BTN_WIDTH + 15),
                BOTTOM_Y, // x, y
                EMULATE_BTN_WIDTH,
                BTN_HEIGHT, // width, height
                selected_button == BTN_EMULATE_X5
            );

            draw_button(
                "Resend",
                SCREEN_WIDTH - BOTTOM_BTN_WIDTH - 5,
                BOTTOM_Y, // x, y
                BOTTOM_BTN_WIDTH,
                BTN_HEIGHT, // width, height
                selected_button == BTN_EMULATE
            );
        }

        sprite->pushSprite(10, 20);
    };

    // Function to send signal
    auto send_signal = [&](int repeat_count) {
        display_status(true, true, repeat_count);
        transmit_code(current_code, repeat_count);
        display_status(true, false, 0, repeat_count > 1 ? String("Sent x") + repeat_count : "Signal sent");
    };

    // Start from current_code if it's set, otherwise start from 0
    uint32_t i = current_code > 0 ? current_code : 0;

    // Always start paused
    is_paused = true;
    selected_button = BTN_START;

    // Initial display - force immediate update
    display_status(is_paused);
    bool needs_display_update = true;

    // Improved encoder handling
    static unsigned long last_encoder_time = 0;
    static unsigned long last_press_time = 0;   // Track time of last press
    static int consecutive_direction = 0;       // Count of consecutive presses in same direction
    const unsigned long ENCODER_DEBOUNCE = 10;  // Reduced from 50ms to 10ms
    const unsigned long DIRECTION_TIMEOUT = 80; // Much stricter - reset after 80ms
    const int ACCELERATION_THRESHOLD = 22;      // Increment by 22

    // Add variables for periodic display update and message timeout
    unsigned long last_display_update = millis();
    unsigned long message_start_time = 0;
    const unsigned long DISPLAY_UPDATE_INTERVAL = 100; // Update every 100ms
    const unsigned long MESSAGE_TIMEOUT = 2000;        // Messages disappear after 2 seconds

    while (i < (1 << bits)) {
        current_code = i;

        // Handle pause state and controls
        while (is_paused) {
            bool needs_display_update = false;
            unsigned long current_time = millis();

            // Force periodic display update
            if (current_time - last_display_update >= DISPLAY_UPDATE_INTERVAL) {
                needs_display_update = true;
                last_display_update = current_time;
            }

            // Handle encoder rotation for menu navigation or code editing
            if (current_time - last_encoder_time >= ENCODER_DEBOUNCE) {
                bool next_pressed = check(NextPress);
                bool prev_pressed = check(PrevPress);

                // Reset consecutive count if too much time passed since last press
                if (current_time - last_press_time > DIRECTION_TIMEOUT) { consecutive_direction = 0; }

                if (next_pressed || prev_pressed) {
                    if (in_code_edit) {
                        // Determine current direction and update consecutive count
                        int current_direction = next_pressed ? 1 : -1;

                        // If we're continuing in the same direction and within timeout
                        if (current_time - last_press_time <= DIRECTION_TIMEOUT) {
                            if ((current_direction == 1 && consecutive_direction >= 0) ||
                                (current_direction == -1 && consecutive_direction <= 0)) {
                                consecutive_direction += current_direction;
                            } else {
                                consecutive_direction = current_direction;
                            }
                        } else {
                            consecutive_direction = current_direction;
                        }

                        // Calculate step size based on consecutive presses
                        int step = 1;
                        if (abs(consecutive_direction) >=
                            ACCELERATION_THRESHOLD) { // After ACCELERATION_THRESHOLD consecutive presses
                            step = 22;                // Increment by 22
                        }

                        // Edit code value
                        if (next_pressed) {
                            if (current_code <= ((1 << bits) - 1 - step)) {
                                current_code += step;
                                i = current_code;
                                needs_display_update = true;
                            } else {
                                current_code = (1 << bits) - 1; // Set to max value
                                i = current_code;
                                needs_display_update = true;
                            }
                        }
                        if (prev_pressed) {
                            if (current_code >= step) {
                                current_code -= step;
                                i = current_code;
                                needs_display_update = true;
                            } else {
                                current_code = 0; // Set to min value
                                i = current_code;
                                needs_display_update = true;
                            }
                        }

                        last_press_time = current_time;
                    } else {
                        // Navigate menu
                        if (next_pressed) {
                            selected_button = (MenuButton)((selected_button + 1) % BTN_COUNT);
                            needs_display_update = true;
                        }
                        if (prev_pressed) {
                            selected_button = (MenuButton)((selected_button - 1 + BTN_COUNT) % BTN_COUNT);
                            needs_display_update = true;
                        }
                    }
                    last_encoder_time = current_time;
                }
            }

            // Handle button actions when paused
            if (check(SelPress)) {
                if (in_code_edit) {
                    in_code_edit = false;
                } else {
                    switch (selected_button) {
                        case BTN_ARROWS: in_code_edit = true; break;
                        case BTN_START: is_paused = false; break;
                        case BTN_SAVE:
                            rf_save_frequency();
                            message_start_time = millis(); // Start message timeout
                            break;
                        case BTN_EMULATE: send_signal(1); break;
                        case BTN_EMULATE_X5: send_signal(5); break;
                        default: break; // Handle BTN_COUNT and any future additions
                    }
                }
                needs_display_update = true;
            }

            // Check for exit
            if (check(EscPress)) {
                sprite->deleteSprite();
                delete sprite;
                deinitRfModule();
                delete protocol;
                return true;
            }

            // Clear message after timeout
            if (message_start_time > 0 && current_time - message_start_time >= MESSAGE_TIMEOUT) {
                message_start_time = 0;
                needs_display_update = true;
            }

            // Update display only if needed
            if (needs_display_update) {
                display_status(true);
                needs_display_update = false;
            }

            delay(5);
        }

        // Check for pause during operation
        if (check(SelPress)) {
            is_paused = true;
            selected_button = BTN_START;
            display_status(true);
            continue;
        }

        // Normal transmission during bruteforce
        display_status(false, true, brute_repeats);
        transmit_code(current_code, brute_repeats);

        if (check(EscPress)) break;

        i++;
        delay(10);
    }

    sprite->deleteSprite();
    delete sprite;
    deinitRfModule();
    delete protocol;
    return true;
}

void rf_bruteforce() {
    // Initialize display
    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorder();

    options = {
        {"Frequency", rf_brute_frequency },
        {"Protocol",  rf_brute_protocol  },
        {"Repeats",   rf_brute_repeats   },
        {"Start",
         [&]() {
             // First check if SEL is being held
             unsigned long hold_start = millis();
             bool hold_detected = false;

             while (check(SelPress)) {
                 if (millis() - hold_start > 500) {
                     hold_detected = true;
                     is_paused = true;
                     break;
                 }
                 delay(10);
             }

             if (!hold_detected) {
                 rf_brute_start();
             } else {
                 rf_brute_start();
             }
         }                               },
        {"Replay",    rf_replay_frequency},
    };

    int ind = 0;
    loopOptions(options, ind);
    options.clear();
}

String get_filename_from_keyboard() { return keyboard("frequencies", 30, "Enter filename:"); }
