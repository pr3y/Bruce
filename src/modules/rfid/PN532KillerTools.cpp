#include "PN532KillerTools.h"
#include "PN532Killer.h"
#include "apdu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/scrollableTextArea.h"
#include "core/sd_functions.h"
#include "driver/uart.h"
#include <NimBLEDevice.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#if defined(ARDUINO_M5STICK_C_PLUS) || defined(ARDUINO_M5STICK_C_PLUS2)

#define TXD_PIN (GPIO_NUM_25)
#define RXD_PIN (GPIO_NUM_26)
#else
#define TXD_PIN (GPIO_NUM_1)
#define RXD_PIN (GPIO_NUM_2)
#endif
#define UART_BAUD_RATE 115200

extern BLEServer *pServer;
extern BLEService *pService;
extern BLECharacteristic *pTxCharacteristic;
extern BLECharacteristic *pRxCharacteristic;
extern bool bleDataTransferEnabled;

PN532KillerTools::PN532KillerTools() { setup(); }

PN532KillerTools::~PN532KillerTools() {
    _pn532Killer.close();
    disableBleDataTransfer();
    if (bleDataTransferEnabled) { disableBleDataTransfer(); }
}

void PN532KillerTools::setup() {
    Serial1.begin(UART_BAUD_RATE, SERIAL_8N1, RXD_PIN, TXD_PIN);
    setReaderMode();
    return loop();
}

void PN532KillerTools::displayBanner() {
    drawMainBorderWithTitle("PN532Killer");
    delay(200);
}

void PN532KillerTools::loop() {
    while (1) {
        if (check(EscPress)) { return; }
        if (_workMode == PN532KillerCmd::WorkMode::Reader) {
            if (check(NextPress)) { readTagUid(); }
        } else if (_workMode == PN532KillerCmd::WorkMode::Emulator) {
            if (check(NextPress)) { setEmulatorNextSlot(false, false); }
            if (check(PrevPress)) { setEmulatorNextSlot(true, false); }
        } else if (_workMode == PN532KillerCmd::WorkMode::Sniffer) {
            if (check(NextPress) && _snifferType == PN532KillerCmd::SnifferType::MFKey32v2) {
                setSnifferUid();
            }
        }

        if (check(SelPress)) { mainMenu(); }

        if (bleDataTransferEnabled) {
            std::vector<uint8_t> bleDataBuffer;
            while (Serial1.available()) {
                uint8_t data = Serial1.read();
                bleDataBuffer.push_back(data);
            }

            if (!bleDataBuffer.empty()) {
                pTxCharacteristic->setValue(bleDataBuffer.data(), bleDataBuffer.size());
                pTxCharacteristic->notify();
                Serial.print("UART > ");
                for (size_t i = 0; i < bleDataBuffer.size(); i++) {
                    if (bleDataBuffer[i] < 0x10) { Serial.print("0"); }
                    Serial.print(bleDataBuffer[i], HEX);
                    Serial.print(" ");
                }
                Serial.println();
            }
        }
        if (returnToMenu) {
            returnToMenu = false;
            break;
        }
    }
}

void PN532KillerTools::mainMenu() {
    options = {
        {"Reader",   [&]() { readerMenu(); }  },
        {"Emulator", [&]() { emulatorMenu(); }},
        {"Sniffer",  [&]() { snifferMenu(); } },
    };
    if (bleDataTransferEnabled) {
        options.push_back({"BLE:ON", [&]() { disableBleDataTransfer(); }});
    } else {
        options.push_back({"BLE:OFF", [&]() { enableBleDataTransfer(); }});
    }
    options.push_back({"Return", [&]() { returnToMenu = true; }});

    loopOptions(options);
}

void PN532KillerTools::readerMenu() {
    options = {
        {"Scan UID", [&]() { readTagUid(); }   },
        {"Return",   [&]() { setReaderMode(); }}
    };

    loopOptions(options);
}

void PN532KillerTools::emulatorMenu() {
    _pn532Killer.tagIndex = -1;
    options = {
        {"MFC1K",
         [&]() {
             _tagType = PN532KillerCmd::TagType::MFC1K;
             setEmulatorNextSlot(false, true);
         }                            },
        {"NTAG",
         [&]() {
             _tagType = PN532KillerCmd::TagType::NTAG;
             setEmulatorNextSlot(false, true);
         }                            },
        {"ISO15693",
         [&]() {
             _tagType = PN532KillerCmd::TagType::ISO15693;
             setEmulatorNextSlot(false, true);
         }                            },
        {"Return",   [&]() { return; }}
    };

    loopOptions(options);
}

void PN532KillerTools::snifferMenu() {
    options = {
        {"MFKey32v2",
         [&]() {
             _snifferType = PN532KillerCmd::SnifferType::MFKey32v2;
             setSnifferMode();
         }                             },
        {"MFKey64",
         [&]() {
             _snifferType = PN532KillerCmd::SnifferType::MFKey64;
             setSnifferMode();
         }                             },
        {"Return",    [&]() { return; }}
    };

    loopOptions(options);
}

void PN532KillerTools::setSnifferMode() {
    _workMode = PN532KillerCmd::WorkMode::Sniffer;
    displayBanner();
    printSubtitle("Sniffer Mode");
    // _pn532Killer.switchEmulatorMifareSlot(0x11); // Firmware issues, remove this line if fixed in future
    _pn532Killer.setSnifferMode(_snifferType);
    String tagType = "MFC 1K";
    String snifferType = "";
    if (_snifferType == PN532KillerCmd::SnifferType::MFKey32v2) {
        drawMfkey32Icon(tftWidth / 4 - 40, (tftHeight) / 2 - 5);
        snifferType = "MFKey32v2";
        printCenterFootnote("Press Next to set UID");
    } else {
        drawMfkey64Icon(tftWidth / 4 - 40, (tftHeight) / 2 - 5);
        snifferType = "MFKey64";
    }
    tft.setTextSize(FM);
    tft.setCursor(tftWidth / 2 - 20, tftHeight / 2 + 5);
    tft.print(tagType);
    tft.setCursor(tftWidth / 2 - 20, tftHeight / 2 + FM * 10 + 5);
    tft.print(snifferType);
}

void PN532KillerTools::setSnifferUid() {
    displayInfo("Scanning UID...");
    _pn532Killer.setNormalMode();
    TagTechnology::Iso14aTagInfo hf14aTagInfo = _pn532Killer.hf14aScan();
    if (!hf14aTagInfo.uid.empty()) {
        displaySuccess(String(hf14aTagInfo.uid_hex.c_str()));
        delay(100);
        _pn532Killer.setSnifferUid(hf14aTagInfo.uid_hex);
        setSnifferMode();
        return;
    }
    displayError("No tag found");
}

void PN532KillerTools::setReaderMode() {
    displayBanner();
    drawCreditCard(tftWidth / 4 - 40, (tftHeight) / 2 - 10);
    tft.setTextSize(FM);
    tft.setCursor(tftWidth / 2 - 20, tftHeight / 2);
    tft.print("ISO14443");
    tft.setCursor(tftWidth / 2 - 20, tftHeight / 2 + FM * 10);
    tft.print("ISO15693");

    printCenterFootnote("Press OK to select mode");
}

void PN532KillerTools::readTagUid() {
    _workMode = PN532KillerCmd::WorkMode::Reader;
    _pn532Killer.setNormalMode();
    displayBanner();
    printSubtitle("UID Reader");
    printCenterFootnote("Scanning ISO14443...");
    TagTechnology::Iso14aTagInfo hf14aTagInfo = _pn532Killer.hf14aScan();
    bool tagFound = false;
    if (!hf14aTagInfo.uid.empty()) {
        printUid("ISO14443", hf14aTagInfo.uid_hex.c_str());
        tagFound = true;
    }
    if (!tagFound) {
        printCenterFootnote("Scanning ISO15693...");
        PN532KillerTagTechnology::Iso15693TagInfo hf15TagInfo = _pn532Killer.hf15Scan();
        if (!hf15TagInfo.uid.empty()) {
            printUid("ISO15693", hf15TagInfo.uid_hex.c_str());
            tagFound = true;
        }
    }
    if (tagFound) {
        printCenterFootnote("Press Next/Down to scan again");
        return;
    }
    displayError("No tag found");
    return;
}

void PN532KillerTools::printUid(const char *protocol, const char *uid) {
    tft.setTextSize(FM);
    tft.setCursor((tftWidth - strlen(protocol) * 6 * FM) / 2, tftHeight / 2);
    tft.println(protocol);
    tft.setCursor((tftWidth - strlen(uid) * 6 * FM) / 2, tft.getCursorY() + 2);
    tft.println(uid);
    tft.setTextSize(FP);
}

void PN532KillerTools::setEmulatorNextSlot(bool reverse, bool redrawTypeName) {
    _workMode = PN532KillerCmd::WorkMode::Emulator;
    if (reverse) {
        if (_pn532Killer.tagIndex <= 0) {
            _pn532Killer.tagIndex = 7;
        } else {
            _pn532Killer.tagIndex--;
        }
    } else {
        if (_pn532Killer.tagIndex >= 7) {
            _pn532Killer.tagIndex = 0;
        } else {
            _pn532Killer.tagIndex++;
        }
    }
    if (_tagType == PN532KillerCmd::TagType::MFC1K) {
        _pn532Killer.switchEmulatorMifareSlot(_pn532Killer.tagIndex);
    } else if (_tagType == PN532KillerCmd::TagType::NTAG) {
        _pn532Killer.switchEmulatorNtagSlot(_pn532Killer.tagIndex);
    } else if (_tagType == PN532KillerCmd::TagType::ISO15693) {
        _pn532Killer.switchEmulatorIso15693Slot(_pn532Killer.tagIndex);
    } else if (_tagType == PN532KillerCmd::TagType::EM4100) {
        _pn532Killer.switchEmulatorEm4100Slot(_pn532Killer.tagIndex);
    }

    if (redrawTypeName) {
        displayBanner();
        printSubtitle("Emulator Mode");
        drawCreditCard(tftWidth / 4 - 40, (tftHeight) / 2 - 5);
        tft.setTextSize(FM);
        String typeName;
        switch (_tagType) {
            case PN532KillerCmd::TagType::MFC1K: typeName = "MFC 1K"; break;
            case PN532KillerCmd::TagType::NTAG: typeName = "NTAG"; break;
            case PN532KillerCmd::TagType::ISO15693: typeName = "ISO15693"; break;
            case PN532KillerCmd::TagType::EM4100: typeName = "EM4100"; break;
            default: typeName = "Unknown"; break;
        }
        tft.setCursor(tftWidth / 2 - 20, tftHeight / 2 + 5);
        tft.print(typeName);
    }

    String slotText = String(_pn532Killer.tagIndex + 1) + "/8";
    int slotLabelX = tftWidth / 2 - 20;
    int slotLabelY = tftHeight / 2 + FM * 10 + 5;
    tft.setTextSize(FM);
    tft.setCursor(slotLabelX, slotLabelY);
    tft.print("Slot: ");
    int slotTextX = tft.getCursorX();
    int slotTextY = slotLabelY;

    tft.fillRect(slotTextX, slotTextY, 40, FM * 10, TFT_BLACK);
    tft.setCursor(slotTextX, slotTextY);
    tft.print(slotText);
}

class RxCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
        std::string value = pCharacteristic->getValue();
        if (!value.empty()) { Serial1.write((uint8_t *)value.data(), value.length()); }
        Serial.print("BLE > ");
        for (size_t i = 0; i < value.length(); i++) {
            if (value[i] < 0x10) { Serial.print("0"); }
            Serial.print(value[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        if (!BLEConnected) {
            BLEConnected = true;
            drawStatusBar();
        }
    }
};

bool PN532KillerTools::enableBleDataTransfer() {
    if (bleDataTransferEnabled) return true;

    BLEDevice::init("BRUCE-PN532-BLE");
    pServer = BLEDevice::createServer();
    pService = pServer->createService("0000fff0-0000-1000-8000-00805f9b34fb");

    pTxCharacteristic =
        pService->createCharacteristic("0000fff1-0000-1000-8000-00805f9b34fb", NIMBLE_PROPERTY::NOTIFY);

    pRxCharacteristic =
        pService->createCharacteristic("0000fff2-0000-1000-8000-00805f9b34fb", NIMBLE_PROPERTY::WRITE);

    pRxCharacteristic->setCallbacks(new RxCharacteristicCallbacks());

    pService->start();
    pServer->getAdvertising()->start();

    bleDataTransferEnabled = true;
    displayInfo("BLE Enabled");
    delay(100);
    return true;
}

bool PN532KillerTools::disableBleDataTransfer() {
    if (!bleDataTransferEnabled) return true;
    pServer = nullptr;
    pService = nullptr;
    pTxCharacteristic = nullptr;
    pRxCharacteristic = nullptr;

    bleDataTransferEnabled = false;
    BLEConnected = false;
    displayInfo("BLE Disabled");
    delay(100);
    return true;
}
