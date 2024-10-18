/**
 * @file chameleon.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Emulate tags using a Chameleon device
 * @version 0.1
 * @date 2024-10-09
 */

#include "chameleon.h"
#include "core/mykeyboard.h"
#include "core/display.h"

#define CMD_DELAY 500
#define MAX_DUMP_SIZE 160


uint8_t calculateLRC(const uint8_t *data, size_t length) {
    uint8_t lrc = 0;

    for (size_t i = 0; i < length; i++) {
        lrc += data[i];
    }
    lrc = 0x100 - (lrc & 0xff);

    return lrc;
}


class scanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (advertisedDevice->getName() == "ChameleonUltra") {
            Serial.print("Chameleon Device found: ");
            Serial.println(advertisedDevice->toString().c_str());
            NimBLEDevice::getScan()->stop();
        }
    }
};


void chameleonNotifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify){
    String str = (isNotify == true) ? "Notification" : "Indication";
    str += " value:";
    for (int i=0; i<length; i++) {
        str += pData[i] < 0x10 ? " 0" : " ";
        str += String(pData[i], HEX);

    }

    Serial.println(str);
}


Chameleon::Chameleon() {
    setup();
}


Chameleon::~Chameleon() {
    Serial.println("Killing Chameleon...");
    if (NimBLEDevice::getInitialized()) {
        Serial.println("Deiniting ble...");
        NimBLEDevice::deinit(true);
    }
}


void Chameleon::setup() {
    if (!openDumpFile()) return;

    if (!getEmulationTagType()) {
        displayError("Invalid tag type");
        delay(1000);
        return;
    }

    selectEmulationSlot();

    displayDumpInfo();
    padprintln("Searching Chameleon Device...");

    displayInfo("Turn on Chameleon device", true);

    if (!searchChameleonDevice()) {
        displayError("Chameleon not found");
        delay(1000);
        return;
    }

    if (!connectToChamelon()) {
        displayError("Chameleon connect error");
        delay(1000);
        return;
    }

    displayDumpInfo();
    padprintln("Chameleon Connected");
    padprintln("Sending commands...");

    if (!sendCommands()) {
        displayError("Chameleon communication error");
        delay(1000);
        return;
    }

    displaySuccess("Success");
    delay(1000);
}


void Chameleon::displayBanner() {
    drawMainBorderWithTitle("CHAMELEON");

    padprintln("          EMULATOR");
    padprintln("          --------");

    tft.setTextSize(FP);
    padprintln("");
}


void Chameleon::displayDumpInfo() {
    displayBanner();

    padprintln(printableUID.picc_type);
    padprintln("UID: " + printableUID.uid);
    padprintln("SAK: " + printableUID.sak);
    padprintln("");
}


bool Chameleon::openDumpFile() {
    String filepath;
    File file;
    FS *fs;

    if(!getFsStorage(fs)) {
        displayError("Storage error");
        delay(1000);
        return false;
    }

    filepath = loopSD(*fs, true, "RFID|NFC");
    file = fs->open(filepath, FILE_READ);

    if (!file) {
        displayError("Dump file error");
        delay(1000);
        return false;
    }

    String line;
    String strData;
    bool pageReadSuccess = true;
    strDump = "";

    while (file.available()) {
        line = file.readStringUntil('\n');
        strData = line.substring(line.indexOf(":") + 1);
        strData.trim();
        if(line.startsWith("Device type:"))  printableUID.picc_type = strData;
        if(line.startsWith("UID:"))          printableUID.uid = strData;
        if(line.startsWith("SAK:"))          printableUID.sak = strData;
        if(line.startsWith("ATQA:"))         printableUID.atqa = strData;
        if(line.startsWith("Pages read:"))   pageReadSuccess = false;
        if(line.startsWith("Page "))         strDump += strData;
    }

    file.close();
    delay(100);

    if (!pageReadSuccess) {
        displayError("Incomplete dump file");
        delay(1000);
        return false;
    }

    printableUID.uid.trim();
    printableUID.uid.replace(" ", "");
    printableUID.sak.trim();
    printableUID.sak.replace(" ", "");
    printableUID.atqa.trim();
    printableUID.atqa.replace(" ", "");
    strDump.trim();
    strDump.replace(" ", "");

    Serial.print("Uid: "); Serial.println(printableUID.uid);
    Serial.print("Sak: "); Serial.println(printableUID.sak);
    Serial.print("Data: "); Serial.println(strDump);
    Serial.print("Data len: "); Serial.println(strDump.length()/2);

    return true;
}


bool Chameleon::getEmulationTagType() {
    byte sak = strtoul(printableUID.sak.c_str(), NULL, 16);
    int dataLen = strDump.length() / 2;

    switch (sak) {
        case 0x09:
            tagType = MIFARE_Mini;
            break;

        case 0x08:
            tagType = MIFARE_1024;
            break;

        case 0x24:
            tagType = MIFARE_4096;
            break;

        case 0x0:  // MIFARE_UL
            if (strDump.substring(0,8) == strDump.substring(strDump.length()-8)) {
                strDump = strDump.substring(0,strDump.length()-8);
            }
            dataLen = strDump.length() / 2;

            switch (dataLen) {
                case 180:
                    tagType = NTAG_213;
                    break;

                case 540:
                    tagType = NTAG_215;
                    break;

                case 924:
                    tagType = NTAG_216;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    Serial.print("Tag type: "); Serial.println(tagType);
    return tagType != UNDEFINED;
}


void Chameleon::selectEmulationSlot() {
    options = {
        {"1", [&]() { emulationSlot=1; }},
        {"2", [&]() { emulationSlot=2; }},
        {"3", [&]() { emulationSlot=3; }},
        {"4", [&]() { emulationSlot=4; }},
        {"5", [&]() { emulationSlot=5; }},
        {"6", [&]() { emulationSlot=6; }},
        {"7", [&]() { emulationSlot=7; }},
        {"8", [&]() { emulationSlot=8; }},
    };
    delay(200);
    loopOptions(options,false,true,"Set Emulation Slot");
}


bool Chameleon::searchChameleonDevice() {
    NimBLEDevice::init("");

    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new scanCallbacks());
    pScan->setActiveScan(true);

    BLEScanResults foundDevices = pScan->start(5);
    bool chameleonFound = false;

    for (int i=0; i<foundDevices.getCount(); i++) {
        NimBLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);

        if (advertisedDevice.getName() == "ChameleonUltra") {
            chameleonFound = true;
            chameleonDevice = advertisedDevice;
        }
    }

    pScan->clearResults();

    return chameleonFound;
}


bool Chameleon::connectToChamelon() {
    NimBLEClient *pClient = NimBLEDevice::createClient();
    bool chrFound = false;

    if (!pClient->connect(&chameleonDevice)) return false;

    Serial.print("Connected to: ");
    Serial.println(pClient->getPeerAddress().toString().c_str());

    std::vector<NimBLERemoteService *> * pSvcs = pClient->getServices(true);
    Serial.print(pSvcs->size()); Serial.println(" services found");

    for (NimBLERemoteService* pSvc : *pSvcs) {
        Serial.print("Service ");Serial.println(pSvc->toString().c_str());

        std::vector<NimBLERemoteCharacteristic *> * pChrs = pSvc->getCharacteristics(true);
        Serial.print(pChrs->size()); Serial.println(" characteristics found");

        if (pChrs->empty()) continue;

        for (NimBLERemoteCharacteristic* pChr : *pChrs) {
            if (pChr->canWrite() && pChr->getUUID().toString().length() > 30) {
                Serial.print("Found Characteristic ");Serial.println(pChr->toString().c_str());
                writeChr = pChr;
                chrFound = true;
            }

            if(pChr->canNotify() && pChr->getUUID().toString().length() > 30) {
                pChr->subscribe(true, chameleonNotifyCB);
            }
        }

        if (chrFound) return true;
    }

    return false;
}


bool Chameleon::chamelonServiceDiscovery() {
    NimBLEClient *pClient = NimBLEDevice::createClient();

    if (!pClient->connect(&chameleonDevice)) return false;

    Serial.print("Connected to: ");
    Serial.println(pClient->getPeerAddress().toString().c_str());

    std::vector<NimBLERemoteService *> * pSvcs = pClient->getServices(true);
    Serial.print(pSvcs->size()); Serial.println(" services found");

    for (NimBLERemoteService* pSvc : *pSvcs) {
        Serial.println(pSvc->toString().c_str());

        std::vector<NimBLERemoteCharacteristic *> * pChrs = pSvc->getCharacteristics(true);
        Serial.print(pChrs->size()); Serial.println(" characteristics found");

        if (pChrs->empty()) continue;

        for (NimBLERemoteCharacteristic* pChr : *pChrs) {
            Serial.println(pChr->toString().c_str());
            Serial.print("UID size: ");Serial.println(pChr->getUUID().toString().length());
            Serial.print("Value? ");Serial.println(pChr->getValue());
            Serial.print("Can read? ");Serial.println(pChr->canRead());
            Serial.print("Can write? ");Serial.println(pChr->canWrite());
            Serial.print("Can write no response? ");Serial.println(pChr->canWriteNoResponse());
            Serial.print("Can notify? ");Serial.println(pChr->canNotify());
            Serial.print("Can indicate? ");Serial.println(pChr->canIndicate());
            Serial.print("Can broadcast? ");Serial.println(pChr->canBroadcast());


            std::vector<NimBLERemoteDescriptor *> * pDscs = pChr->getDescriptors(true);
            Serial.print(pDscs->size()); Serial.println(" descriptors found");
            for (NimBLERemoteDescriptor* pDsc : *pDscs) {
                Serial.println(pDsc->toString().c_str());
            }
        }

    }

    return true;
}


bool Chameleon::sendCommands() {
    return (
        cmdEnableSlotHF()
        && cmdChangeActiveSlot()
        && cmdChangeSlotType()

        && cmdUploadDumpData()
        && cmdSetEmulationConfig()

        && cmdSetEmulationMode()
        && cmdChangeHFSlotNickName()
    );
}


bool Chameleon::cmdEnableSlotHF() {
    Serial.printf("Enable HF on slot %d\n", emulationSlot);
    uint8_t cmd[13] = {
        0x11, 0xef, 0x03, 0xee, 0x00, 0x00, 0x00, 0x03, 0x0c, 0x00, 0x02, 0x01, 0x00
    };
    cmd[9] = emulationSlot-1;
    cmd[12] = calculateLRC(cmd+9, cmd[7]);

    return submitCommand(cmd, sizeof(cmd));
}


bool Chameleon::cmdChangeActiveSlot() {
    Serial.printf("Change active slot to %d\n", emulationSlot);
    uint8_t cmd[11] = {
        0x11, 0xef, 0x03, 0xeb, 0x00, 0x00, 0x00, 0x01, 0x11, 0x00, 0x00
    };
    cmd[9] = emulationSlot-1;
    cmd[10] = calculateLRC(cmd+9, cmd[7]);

    return submitCommand(cmd, sizeof(cmd));
}


bool Chameleon::cmdChangeSlotType() {
    Serial.printf("Change slot %d type\n", emulationSlot);
    uint8_t cmd[13] = {
        0x11, 0xef, 0x03, 0xec, 0x00, 0x00, 0x00, 0x03, 0x0e, 0x00, 0x00, 0x00, 0x00
    };
    cmd[9] = emulationSlot-1;
    cmd[10] = (tagType >> 8) & 0xFF;
    cmd[11] = tagType & 0xFF;
    cmd[12] = calculateLRC(cmd+9, cmd[7]);

    return submitCommand(cmd, sizeof(cmd));
}


bool Chameleon::cmdUploadDumpData() {
    Serial.println("Upload dump data");
    uint8_t cmd[507] = {0x0};

    cmd[0] = 0x11;
    cmd[1] = 0xef;
    cmd[2] = 0x0f;
    cmd[3] = 0x0a;
    cmd[4] = 0x00;
    cmd[5] = 0x00;

    int index = 0;
    int block = 0;
    for (size_t i = 0; i < strDump.length(); i += 2) {
        cmd[10 + index++] = strtoul(strDump.substring(i, i + 2).c_str(), NULL, 16);

        if (index == MAX_DUMP_SIZE || i+2 ==  strDump.length()) {
            cmd[6] = ((index+1) >> 8) & 0xFF;
            cmd[7] = (index+1) & 0xFF;
            cmd[8] = calculateLRC(cmd+2, 6);
            cmd[9] = block;

            cmd[10 + index] = calculateLRC(cmd+9, index+2);

            if (!submitCommand(cmd, 10+index+1)) return false;

            block += index / 16;
            index = 0;
        }
    }

    return true;
}


bool Chameleon::cmdSetEmulationConfig() {
    Serial.println("Set emulation config");

    uint8_t cmd[22] = {
        0x11, 0xef, 0x0f, 0xa1, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    cmd[7] = 5 + (printableUID.uid.length() / 2);
    cmd[8] = calculateLRC(cmd, 8);
    cmd[9] = printableUID.uid.length() / 2;

    for (size_t i = 0; i < printableUID.uid.length(); i += 2) {
        cmd[10 + (i / 2)] = strtoul(printableUID.uid.substring(i, i + 2).c_str(), NULL, 16);
    }

    int index = 10 + (printableUID.uid.length() / 2);
    cmd[index++] = strtoul(printableUID.atqa.substring(2, 4).c_str(), NULL, 16);
    cmd[index++] = strtoul(printableUID.atqa.substring(0, 2).c_str(), NULL, 16);
    cmd[index++] = strtoul(printableUID.sak.c_str(), NULL, 16);
    cmd[index++] = 0x00;  // ats
    cmd[index++] = calculateLRC(cmd+9, index-9);

    return submitCommand(cmd, index);
}


bool Chameleon::cmdSetEmulationMode() {
    Serial.println("Set emulation mode");
    uint8_t cmd[11] = {
        0x11, 0xef, 0x03, 0xe9, 0x00, 0x00, 0x00, 0x01, 0x13, 0x00, 0x00
    };

    return submitCommand(cmd, sizeof(cmd));
}


bool Chameleon::cmdChangeHFSlotNickName() {
    Serial.println("Change slot nick name to Bruce");
    uint8_t cmd[17] = {
        0x11, 0xef, 0x03, 0xef, 0x00, 0x00, 0x00, 0x07, 0x07,
        0x00, 0x02, 0x42, 0x72, 0x75, 0x63, 0x65, 0x00
    };
    cmd[9] = emulationSlot-1;
    cmd[16] = calculateLRC(cmd+9, cmd[7]);

    return submitCommand(cmd, sizeof(cmd));
}


bool Chameleon::submitCommand(uint8_t *data, size_t length) {
    Serial.print("Cmd:");
    for (int i=0; i<length; i++) {
        Serial.print(data[i] < 0x10 ? " 0" : " ");
        Serial.print(data[i], HEX);
    }
    Serial.println("");

    bool res = writeChr->writeValue(data, length, true);

    delay(CMD_DELAY);

    return res;
}
