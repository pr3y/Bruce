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

    enum RFIDFreq {
        RFID_LF = 0x01,
        RFID_HF = 0x02,
    };

    enum HwMode {
        HW_MODE_EMULATOR = 0x00,
        HW_MODE_READER = 0x01,
    };

    enum TagType {
        UNDEFINED = 0,

        // LF
        EM410X = 100,
        TAG_TYPES_LF_END = 999,

        // HF
        // MIFARE Classic series  1000
        MIFARE_Mini = 1000,
        MIFARE_1024 = 1001,
        MIFARE_2048 = 1002,
        MIFARE_4096 = 1003,
        // MFUL / NTAG series     1100
        NTAG_213 = 1100,
        NTAG_215 = 1101,
        NTAG_216 = 1102,
        MF0ICU1 = 1103,
        MF0ICU2 = 1104,
        MF0UL11 = 1105,
        MF0UL21 = 1106,
        NTAG_210 = 1107,
        NTAG_212 = 1108,
    };

    enum RspStatus {
        HF_TAG_OK = 0x00,     // IC card operation is successful
        HF_TAG_NO = 0x01,     // IC card not found
        HF_ERR_STAT = 0x02,   // Abnormal IC card communication
        HF_ERR_CRC = 0x03,    // IC card communication verification abnormal
        HF_COLLISION = 0x04,  // IC card conflict
        HF_ERR_BCC = 0x05,    // IC card BCC error
        MF_ERR_AUTH = 0x06,   // MF card verification failed
        HF_ERR_PARITY = 0x07, // IC card parity error
        HF_ERR_ATS = 0x08,    // ATS should be present but card NAKed, or ATS too large

        // Some operations with low frequency cards succeeded!
        LF_TAG_OK = 0x40,
        // Unable to search for a valid EM410X label
        EM410X_TAG_NO_FOUND = 0x41,

        // The parameters passed by the BLE instruction are wrong,
        // or the parameters passed by calling some functions are wrong
        PAR_ERR = 0x60,
        // The mode of the current device is wrong, and the corresponding
        // API cannot be called
        DEVICE_MODE_ERROR = 0x66,
        INVALID_CMD = 0x67,
        SUCCESS = 0x68,
        NOT_IMPLEMENTED = 0x69,
        FLASH_WRITE_FAIL = 0x70,
        FLASH_READ_FAIL = 0x71,
        INVALID_SLOT_TYPE = 0x72,
    };


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
