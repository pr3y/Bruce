#ifndef __PN532BLE_H__
#define __PN532BLE_H__

#include <set>
#include "pn532_ble.h"
#include <vector>

class Pn532ble
{
public:
    Pn532ble();
    ~Pn532ble();

    void setup();
    void loop();
    bool connect();
    String getDeviceName() { return String(pn532_ble.getName().c_str()); }

    enum AppMode
    {
        GET_FW_MODE,
        HF_SCAN_MODE
    };
    typedef struct 
    {
        std::vector<uint8_t> atqa;
        uint8_t sak;
        uint8_t uidSize;
        std::vector<uint8_t> uid;
        String uid_hex;
        String sak_hex;
        String atqa_hex;
    } Iso14aTagInfo;

private:
    PN532_BLE pn532_ble = PN532_BLE(false);
    PN532_BLE::HfTag hfTagData;

    std::vector<uint8_t> buffer;
    void onNotify(uint8_t *data, size_t length);
    void displayBanner();
    void showDeviceInfo();
    void hf14aScan();
    void selectMode();
    AppMode currentMode;
    void setMode(AppMode mode);

    Iso14aTagInfo parseHf14aScan(uint8_t *data, uint8_t dataSize);
};

#endif