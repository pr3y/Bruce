#ifndef __BRUCE_CONFIG_H__
#define __BRUCE_CONFIG_H__

#include "core/globals.h"
// #include <Arduino.h>


class BruceConfig {
public:
    int rotation = ROTATION > 1 ? 3 : 1;
    int dimmerSet = 10;
    int bright = 100;
    int tmz = 3;
    String wuiUsr = "admin";
    String wuiPwd = "bruce";

    // Theme colors in RGB565 format
    uint16_t priColor = 0xA80F;
    uint16_t secColor = 0xFA99;  // 0x0566;
    uint16_t bgColor = 0x0;

    int irTx = LED;
    int irRx = GROVE_SCL;
    int rfTx = GROVE_SDA;
    int rfRx = GROVE_SCL;
    int rfModule = M5_RF_MODULE;
    float rfFreq = 433.92;
    int rfFxdFreq = 1;
    int rfScanRange = 3;
    int rfidModule = M5_RFID2_MODULE;

    String wigleBasicToken = "";
    int devMode = 0;
    int soundEnabled = 1;
    // wifi = [{"ssid":"myNetSSID","pwd":"myNetPassword"}];
    // wifi_ap = {"ssid":"BruceNet","pwd":"brucenet"};


    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    BruceConfig() {};
    // ~BruceConfig();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    void saveFile();
    void fromFile();

private:
    const char *filepath = "/bruceNew.conf";
};

#endif
