/**
 * @file amiibolink.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Emulate amiibo using an amiibolink
 * @version 0.1
 * @date 2024-10-11
 */

#include "amiibolink.h"
#include "core/mykeyboard.h"
#include "core/display.h"

#define CMD_DELAY 500
#define MAX_DUMP_SIZE 20


class scanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (advertisedDevice->getName() == "amiibolink") {
            Serial.print("Amiibolink found: ");
            Serial.println(advertisedDevice->toString().c_str());
            NimBLEDevice::getScan()->stop();
        }
    }
};


class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    };
    /** Called before notification or indication is sent,
     *  the value can be changed here before sending if desired.
     */
    void onNotify(NimBLECharacteristic* pCharacteristic) {
        Serial.println("Sending notification to clients");
    };


    /**
     *  The value returned in code is the NimBLE host return code.
     */
    void onStatus(NimBLECharacteristic* pCharacteristic, int code) {
        String str = ("Notification/Indication return code: ");
        str += code;
        str += ", ";
        str += NimBLEUtils::returnCodeToString(code);
        Serial.println(str);
    };

    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) {
        String str = "Client ID: ";
        str += connInfo.getConnHandle();
        str += " Address: ";
        str += connInfo.getAddress().toString().c_str();
        if(subValue == 0) {
            str += " Unsubscribed to ";
        }else if(subValue == 1) {
            str += " Subscribed to notfications for ";
        } else if(subValue == 2) {
            str += " Subscribed to indications for ";
        } else if(subValue == 3) {
            str += " Subscribed to notifications and indications for ";
        }
        str += std::string(pCharacteristic->getUUID()).c_str();

        Serial.println(str);
    };
};


void amiibolinkNotifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify){
    String str = (isNotify == true) ? "Notification" : "Indication";
    str += " value:";
    for (int i=0; i<length; i++) {
        str += pData[i] < 0x10 ? " 0" : " ";
        str += String(pData[i], HEX);

    }

    Serial.println(str);
}


Amiibolink::Amiibolink() {
    setup();
}


Amiibolink::~Amiibolink() {
    Serial.println("Killing Amiibolink...");
    if (NimBLEDevice::getInitialized()) {
        Serial.println("Deiniting ble...");
        NimBLEDevice::deinit(true);
    }
}


void Amiibolink::setup() {
    if (!openDumpFile()) return;

    if (!checkEmulationTagType()) {
        displayError("Invalid tag type");
        delay(1000);
        return;
    }

    displayDumpInfo();
    padprintln("Searching Amiibolink Device...");

    displayInfo("Turn on Amiibolink device", true);

    if (!searchDevice()) {
        displayError("Amiibolink not found");
        delay(1000);
        return;
    }

    if (!connectToDevice()) {
        displayError("Amiibolink connect error");
        delay(1000);
        return;
    }

    displayDumpInfo();
    padprintln("Amiibolink Connected");
    padprintln("Sending commands...");

    if (!sendCommands()) {
        displayError("Amiibolink communication error");
        delay(1000);
        return;
    }

    displaySuccess("Success");
    delay(1000);
}


void Amiibolink::displayBanner() {
    drawMainBorderWithTitle("AMIIBOLINK");

    padprintln("            EMULATOR");
    padprintln("            --------");

    tft.setTextSize(FP);
    padprintln("");
}


void Amiibolink::displayDumpInfo() {
    displayBanner();

    padprintln(printableUID.picc_type);
    padprintln("UID: " + printableUID.uid);
    padprintln("SAK: " + printableUID.sak);
    padprintln("");
}


bool Amiibolink::openDumpFile() {
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


bool Amiibolink::checkEmulationTagType() {
    byte sak = strtoul(printableUID.sak.c_str(), NULL, 16);
    int dataLen = strDump.length() / 2;

    if (sak != 0x00) return false;

    if (strDump.substring(0,8) == strDump.substring(strDump.length()-8)) {
        strDump = strDump.substring(0,strDump.length()-8);
    }

    if (strDump.length() / 2 != 540) return false;  // Not an NTAG_215

    return true;
}


bool Amiibolink::searchDevice() {
    NimBLEDevice::init("");

    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new scanCallbacks());
    pScan->setActiveScan(true);

    BLEScanResults foundDevices = pScan->start(5);
    bool deviceFound = false;

    for (int i=0; i<foundDevices.getCount(); i++) {
        NimBLEAdvertisedDevice advertisedDevice = foundDevices.getDevice(i);

        if (advertisedDevice.getName() == "amiibolink") {
            deviceFound = true;
            bleDevice = advertisedDevice;
        }
    }

    pScan->clearResults();

    return deviceFound;
}


bool Amiibolink::connectToDevice() {
    NimBLEClient *pClient = NimBLEDevice::createClient();
    pClient->setConnectionParams(12,12,0,51);

    if (!pClient->connect(&bleDevice, false)) return false;

    Serial.print("Connected to: ");
    Serial.println(pClient->getPeerAddress().toString().c_str());

    delay(200);

    // pClient->discoverAttributes();

    NimBLERemoteService* pSvc = nullptr;
    NimBLERemoteCharacteristic* pChrWrite = nullptr;
    NimBLERemoteCharacteristic* pChrNotify = nullptr;

    pSvc = pClient->getService(serviceUUID);
    if (!pSvc) {
        Serial.println("Service does not exist");
        return false;
    }
    Serial.print("Service ");Serial.println(pSvc->toString().c_str());

    pChrWrite = pSvc->getCharacteristic(chrTxUUID);
    pChrNotify = pSvc->getCharacteristic(chrRxUUID);

    if (!pChrWrite || !pChrNotify) {
        Serial.println("Characteristics do not exist");
        return false;
    }

    Serial.print("Found Characteristic ");Serial.println(pChrWrite->toString().c_str());
    writeChr = pChrWrite;
    pChrNotify->subscribe(true, amiibolinkNotifyCB);

    return true;
}


bool Amiibolink::serviceDiscovery() {
    NimBLEClient *pClient = NimBLEDevice::createClient();

    if (!pClient->connect(&bleDevice)) return false;

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


bool Amiibolink::sendCommands() {
    return (
        cmdPreUploadDump()
        && cmdUploadDumpData()
        && cmdPostUploadDump()
    );
}


bool Amiibolink::cmdPreUploadDump() {
    Serial.println("Pre upload dump commands");

    uint8_t cmd0[2] = {0xA0, 0xB0};
    if (!submitCommand(cmd0, sizeof(cmd0))) return false;

    uint8_t cmd1[8] = {0xAC, 0xAC, 0x00, 0x04, 0x00, 0x00, 0x02, 0x1C};
    if (!submitCommand(cmd1, sizeof(cmd1))) return false;

    uint8_t cmd2[4] = {0xAB, 0xAB, 0x02, 0x1C};
    if (!submitCommand(cmd2, sizeof(cmd2))) return false;

    return true;
}


bool Amiibolink::cmdUploadDumpData() {
    Serial.println("Upload dump data");
    uint8_t cmd[26] = {0x0};

    cmd[0] = 0xDD;
    cmd[1] = 0xAA;
    cmd[2] = 0x00;
    cmd[3] = 0x14;

    int index = 0;
    int block = 0;
    for (size_t i = 0; i < strDump.length(); i += 2) {
        cmd[4 + index++] = strtoul(strDump.substring(i, i + 2).c_str(), NULL, 16);

        if (index == MAX_DUMP_SIZE || i+2 == strDump.length()) {
            index = 0;
            block = (i/2) / 20 + 1;

            cmd[24] = 0x00;
            cmd[25] = block;

            if (!submitCommand(cmd, sizeof(cmd))) return false;
        }
    }

    return true;
}


bool Amiibolink::cmdPostUploadDump() {
    Serial.println("Post upload dump commands");

    uint8_t cmd0[2] = {0xBC, 0xBC};
    if (!submitCommand(cmd0, sizeof(cmd0))) return false;

    uint8_t cmd1[2] = {0xCC, 0xDD};
    if (!submitCommand(cmd1, sizeof(cmd1))) return false;

    return true;
}


bool Amiibolink::submitCommand(uint8_t *data, size_t length) {
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
