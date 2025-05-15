#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"
#include "freertos/FreeRTOS.h"

#include "core/display.h"
#include "core/sd_functions.h"
#include "openhaystack.h"
#include <globals.h>

/** Callback function for BT events */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/** Random device address */
static esp_bd_addr_t rnd_addr = {0xFF, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

uint8_t public_key_decoded[28];

/** Advertisement payload */
static uint8_t adv_data[31] = {
    0x1e,       /* Length (30) */
    0xff,       /* Manufacturer Specific Data (type 0xff) */
    0x4c, 0x00, /* Company ID (Apple) */
    0x12, 0x19, /* Offline Finding type and length */
    0x00,       /* State */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* First two bits */
    0x00,                                                             /* Hint (0x00) */
};

/* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv420esp_ble_adv_params_t
 */
static esp_ble_adv_params_t ble_adv_params = {
    // Advertising min interval:
    // Minimum advertising interval for undirected and low duty cycle
    // directed advertising. Range: 0x0020 to 0x4000 Default: N = 0x0800
    // (1.28 second) Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec
    .adv_int_min = 0x0640, // 1s
    // Advertising max interval:
    // Maximum advertising interval for undirected and low duty cycle
    // directed advertising. Range: 0x0020 to 0x4000 Default: N = 0x0800
    // (1.28 second) Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec
    .adv_int_max = 0x0C80, // 2s
    // Advertisement type
    .adv_type = ADV_TYPE_NONCONN_IND,
    // Use the random address
    .own_addr_type = BLE_ADDR_TYPE_RANDOM,
    // All channels
    .channel_map = ADV_CHNL_ALL,
    // Allow both scan and connection requests from anyone.
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// Flag to track if openhaystack is active
bool isOpenHaystackActive = false;

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    esp_err_t err;

    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT: esp_ble_gap_start_advertising(&ble_adv_params); break;

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            // adv start complete event to indicate adv start successfully or failed
            if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
                // ESP_LOGE(LOG_TAG, "advertising start failed: %s", esp_err_to_name(err));
            } else {
                // ESP_LOGI(LOG_TAG, "advertising has started.");
                Serial.print("advertising has started\n");
            }
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
                // ESP_LOGE(LOG_TAG, "adv stop failed: %s", esp_err_to_name(err));
                Serial.printf("adv stop failed: %s\n", esp_err_to_name(err));
            } else {
                // ESP_LOGI(LOG_TAG, "stop adv successfully");
                Serial.println("stop adv successfully\n");
            }
            break;
        default: break;
    }
}

void set_addr_from_key(esp_bd_addr_t addr, uint8_t *public_key_decoded) {
    addr[0] = public_key_decoded[0] | 0b11000000;
    addr[1] = public_key_decoded[1];
    addr[2] = public_key_decoded[2];
    addr[3] = public_key_decoded[3];
    addr[4] = public_key_decoded[4];
    addr[5] = public_key_decoded[5];
}

void set_payload_from_key(uint8_t *payload, uint8_t *public_key_decoded) {
    // copy last 22 bytes //
    memcpy(&payload[7], &public_key_decoded[6], 22);
    // append two bits of public key //
    payload[29] = public_key_decoded[0] >> 6;
}

void drawErrorMessage(esp_err_t status, const char *text) {
    Serial.printf("%s: %s\n", text, esp_err_to_name(status));
    tft.setCursor(0, 60);
    tft.setTextColor(TFT_RED, bruceConfig.bgColor);
    tft.printf("%s: %s\n", text, esp_err_to_name(status));
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
}

void stopOpenHaystack() {
    isOpenHaystackActive = false;
    esp_ble_gap_stop_advertising();
    Serial.println("OpenHaystack stopped");
}

bool openHaystackSelectionMenu() {
    tft.fillScreen(bruceConfig.bgColor);
    drawMainBorderWithTitle("OpenHaystack Menu");

    std::vector<Option> options = {
        {"Return to OpenHaystack", []() {}},
        {"Exit to Main Menu",      []() {}}
    };

    // loopOptions returns the selected index
    int selected = loopOptions(options, 0);
    options.clear();

    // If user selected "Exit to Main Menu" (index 1), return true
    return (selected == 1);
}

void drawOpenHaystackScreen() {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
    tft.println("Running openhaystack");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    tft.setCursor(0, 20);
    tft.println("using device:");
    tft.printf(
        "%02x %02x %02x %02x %02x %02x\n",
        rnd_addr[0],
        rnd_addr[1],
        rnd_addr[2],
        rnd_addr[3],
        rnd_addr[4],
        rnd_addr[5]
    );

#if defined(HAS_TOUCH)
    TouchFooter();
#endif

    tft.setTextColor(TFT_RED);
    tft.drawCentreString("press Esc to stop", tftWidth / 2, tftHeight - 15, 1);
    tft.setTextColor(bruceConfig.priColor);
}

void openhaystack_loop() {
    // Set up the device address and payload from the public key
    set_addr_from_key(rnd_addr, public_key_decoded);
    set_payload_from_key(adv_data, public_key_decoded);

    drawOpenHaystackScreen();

    Serial.printf(
        "using device address: %02x %02x %02x %02x %02x %02x\n",
        rnd_addr[0],
        rnd_addr[1],
        rnd_addr[2],
        rnd_addr[3],
        rnd_addr[4],
        rnd_addr[5]
    );

    esp_err_t status;
    // register the scan callback function to the gap module
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        drawErrorMessage(status, "gap register error");
        return;
    }

    if ((status = esp_ble_gap_set_rand_addr(rnd_addr)) != ESP_OK) {
        drawErrorMessage(status, "couldn't set random address");
        return;
    }

    if ((status = esp_ble_gap_config_adv_data_raw((uint8_t *)&adv_data, sizeof(adv_data))) != ESP_OK) {
        drawErrorMessage(status, "couldn't configure BLE adv");
        return;
    }

    isOpenHaystackActive = true;

    bool exitRequested = false;
    while (isOpenHaystackActive && !exitRequested) {
        // Check for escape key
        if (check(EscPress)) {
            stopOpenHaystack(); // Temporarily stop advertising

            // Show selection menu and get user's choice
            bool shouldExit = openHaystackSelectionMenu();

            if (shouldExit) {
                // User wants to exit to main menu
                exitRequested = true;
                Serial.println("Exiting OpenHaystack");
            } else {
                // User wants to continue - restart advertising
                isOpenHaystackActive = true;
                drawOpenHaystackScreen();

                // Restart advertising
                if ((status = esp_ble_gap_set_rand_addr(rnd_addr)) != ESP_OK) {
                    drawErrorMessage(status, "couldn't set random address");
                    return;
                }

                if ((status = esp_ble_gap_config_adv_data_raw((uint8_t *)&adv_data, sizeof(adv_data))) !=
                    ESP_OK) {
                    drawErrorMessage(status, "couldn't configure BLE adv");
                    return;
                }

                Serial.println("Returned to OpenHaystack");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void openhaystack_setup() {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
    tft.println("Setting up openhaystack");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t status;

    // Check if Bluetooth is already initialized
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
        Serial.println("Bluetooth controller already initialized");
    } else {
        // Initialize the controller
        if ((status = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
            drawErrorMessage(status, "BT controller init failed");
            return;
        }
        Serial.println("BT controller initialized");

        // Enable the controller
        if ((status = esp_bt_controller_enable(ESP_BT_MODE_BLE)) != ESP_OK) {
            drawErrorMessage(status, "BT controller enable failed");
            return;
        }
        Serial.println("BT controller enabled");
    }

    // Initialize Bluedroid stack
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED) {
        Serial.println("Bluedroid already initialized and enabled");
    } else {
        if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
            if ((status = esp_bluedroid_init()) != ESP_OK) {
                drawErrorMessage(status, "Bluedroid init failed");
                return;
            }
            Serial.println("Bluedroid initialized");
        }

        if ((status = esp_bluedroid_enable()) != ESP_OK) {
            drawErrorMessage(status, "Bluedroid enable failed");
            return;
        }
        Serial.println("Bluedroid enabled");
    }

    // Wait for Bluetooth to fully initialize
    vTaskDelay(500 / portTICK_PERIOD_MS); // Increased delay to ensure stack initialization

    // Verify the stack is ready
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        drawErrorMessage(ESP_FAIL, "Bluedroid not fully initialized");
        return;
    }

    // Register the callback after ensuring Bluetooth is ready
    if ((status = esp_ble_gap_register_callback(esp_gap_cb)) != ESP_OK) {
        drawErrorMessage(status, "GAP register error");
        return;
    }
    Serial.println("GAP callback registered");

    // Load public key
    File file;

    if (setupSdCard()) {
        file = SD.open("/pub.key");
    } else {
        LittleFS.begin();
        file = LittleFS.open("/pub.key");
    }

    if (!file) {
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        Serial.println("Failed to open file");
        tft.println("No pub.key file\nfound on\nthe SD");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        delay(5000);
        return;
    }

    Serial.println("Got public key from file");

    size_t bytes_read = file.read(public_key_decoded, 28);
    file.close();

    if (bytes_read != 28) {
        Serial.println("Error: Public key isn't a valid format!");
        return;
    }

    Serial.printf("Application was initialized\n");

    openhaystack_loop();
}
