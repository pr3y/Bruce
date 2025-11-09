#ifndef __PN532BLE_H__
#define __PN532BLE_H__
#ifndef LITE_VERSION
#include "core/scrollableTextArea.h"
#include "pn532_ble.h"
#include <set>
#include <vector>

class Pn532ble {
public:
    Pn532ble();
    ~Pn532ble();

    void setup();
    void loop();
    bool connect();
    String getDeviceName() { return String(pn532_ble.getName().c_str()); }

    enum AppMode {
        STANDBY_MODE,
        GET_FW_MODE,
        HF_MF_LOAD_DUMP_MODE,
        HF_MFU_LOAD_DUMP_MODE,
        HF_ISO15693_LOAD_DUMP_MODE,
        HF_14A_SCAN_MODE,
        HF_14B_SCAN_MODE,
        HF_15_SCAN_MODE,
        HF_MF_READ_MODE,
        HF_MFU_READ_MODE,
        HF_ISO15693_READ_MODE,
        HF_MF_WRITE_MODE,
        HF_MFU_WRITE_MODE,
        HF_ISO15693_WRITE_MODE,
        LF_EM4100_SCAN_MODE,
        HF_TG_INIT_AS_TARGET_MODE,
    };

private:
    PN532_BLE pn532_ble = PN532_BLE(true);

    std::vector<uint8_t> buffer;
    void onNotify(uint8_t *data, size_t length);
    void displayBanner();
    void showDeviceInfo();
    void hf14aScan();
    void hf15Scan();
    void lfScan();
    void hf14aMfReadDumpMode();
    void hf14aMfuReadDumpMode();
    void hf14aMfuWriteDumpMode();
    void hf14aMfWriteDumpMode();
    void hf14aMfWriteDump(ScrollableTextArea &area);
    void hf15ReadDumpMode();
    void hf15WriteDumpMode();
    void selectMode();
    void scanTagMenu();
    void readTagMenu();
    void writeDumpMenu();
    void saveDumpMenu();
    void loadDumpMenu();
    void loadNdefEmulateMenu();
    String emulationNdefData = "https://pn532killer.com";
    AppMode currentMode;
    void setMode(AppMode mode);
    uint8_t getMifareClassicSectorCount(uint8_t sak);
    String saveHfDumpBinFile(std::vector<uint8_t> data, String uid, String prefix);
    void loadMifareClassicDumpFile();
    void loadMifareUltralightDumpFile();
    void loadIso15693DumpFile();
    void ntagEmulationMode();
    std::vector<uint8_t> mfd;
    std::vector<uint8_t> mfud;
    std::vector<uint8_t> iso15dump;
    std::string gen4pwd = "00000000";
    std::vector<uint8_t> default4bS20Config = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x09, 0x78,
                                               0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0x10, 0x11, 0x12,
                                               0x13, 0x14, 0x15, 0x16, 0x04, 0x00, 0x09, 0x00};
    std::vector<uint8_t> default7bS20Config = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x09, 0x78,
                                               0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0x10, 0x11, 0x12,
                                               0x13, 0x14, 0x15, 0x16, 0x44, 0x00, 0x09, 0x00};
    std::vector<uint8_t> default4bS50Config = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x09, 0x78,
                                               0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0x10, 0x11, 0x12,
                                               0x13, 0x14, 0x15, 0x16, 0x04, 0x00, 0x08, 0x00};
    std::vector<uint8_t> default7bS50Config = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x09, 0x78,
                                               0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0x10, 0x11, 0x12,
                                               0x13, 0x14, 0x15, 0x16, 0x44, 0x00, 0x08, 0x00};
    std::vector<uint8_t> default4bS70Config = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x09, 0x78,
                                               0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0x10, 0x11, 0x12,
                                               0x13, 0x14, 0x15, 0x16, 0x02, 0x00, 0x18, 0x00};
    std::vector<uint8_t> default7bS70Config = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x09, 0x78,
                                               0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0x10, 0x11, 0x12,
                                               0x13, 0x14, 0x15, 0x16, 0x42, 0x00, 0x18, 0x00};
};

#endif
#endif
