/**
 * @file amiibolink.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Emulate amiibo using an amiibolink
 * @version 0.1
 * @date 2024-10-11
 */


#ifndef __AMIIBOLINK_H__
#define __AMIIBOLINK_H__

#include <NimBLEDevice.h>


class Amiibolink {
public:
    typedef struct {
        String uid;
        String bcc;
        String sak;
        String atqa;
        String picc_type;
    } PrintableUID;

    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    Amiibolink();
    ~Amiibolink();

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    void setup();

private:
    NimBLERemoteCharacteristic* writeChr;
    NimBLEAdvertisedDevice bleDevice;
    PrintableUID printableUID;
    String strDump = "";

    NimBLEUUID serviceUUID = NimBLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    NimBLEUUID chrTxUUID = NimBLEUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
    NimBLEUUID chrRxUUID = NimBLEUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

    void displayBanner();
    void displayDumpInfo();

    bool openDumpFile();
    bool checkEmulationTagType();

    bool searchDevice();
    bool connectToDevice();
    bool serviceDiscovery();

    bool sendCommands();
    bool submitCommand(uint8_t *data, size_t length);

    bool cmdPreUploadDump();
    bool cmdUploadDumpData();
    bool cmdPostUploadDump();
};

#endif
