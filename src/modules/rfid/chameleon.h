/**
 * @file chameleon.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Emulate tags using a Chameleon device
 * @version 0.1
 * @date 2024-10-09
 */


#ifndef __CHAMELEON_H__
#define __CHAMELEON_H__

#include <NimBLEDevice.h>


class Chameleon {
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
    Chameleon();
    ~Chameleon();

    /////////////////////////////////////////////////////////////////////////////////////
    // Life Cycle
    /////////////////////////////////////////////////////////////////////////////////////
    void setup();

private:
    NimBLERemoteCharacteristic* writeChr;
    NimBLEAdvertisedDevice chameleonDevice;
    PrintableUID printableUID;
    String strDump = "";
    int emulationSlot;
    int tagType = 0;

    void displayBanner();
    void displayDumpInfo();

    bool openDumpFile();
    bool getEmulationTagType();
    void selectEmulationSlot();

    bool searchChameleonDevice();
    bool connectToChamelon();
    bool chamelonServiceDiscovery();

    bool sendCommands();
    bool submitCommand(uint8_t *data, size_t length);

    bool cmdEnableSlotHF();
    bool cmdChangeActiveSlot();
    bool cmdChangeSlotType();
    bool cmdUploadDumpData();
    bool cmdSetEmulationConfig();
    bool cmdSetEmulationMode();
    bool cmdChangeHFSlotNickName();

};

#endif
