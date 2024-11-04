#ifndef __BRUCE_CONFIG_H__
#define __BRUCE_CONFIG_H__

// #include "globals.h"
#include <Arduino.h>

#define DEFAULT_PRICOLOR 0xA80F

enum RFIDModules {
    M5_RFID2_MODULE  = 0,
    PN532_I2C_MODULE = 1,
    PN532_SPI_MODULE = 2,
};

enum RFModules {
    M5_RF_MODULE = 0,
    CC1101_SPI_MODULE = 1,
};


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
    uint16_t secColor = 0x880F;
    // uint16_t secColor = 0xFA99;  // 0x0566;
    uint16_t bgColor = 0x0;  // Black

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

    /////////////////////////////////////////////////////////////////////////////////////
    // Setters
    /////////////////////////////////////////////////////////////////////////////////////
    void setTheme(uint16_t primary, uint16_t secondary = NULL);
    void setWebUICreds(String usr, String pwd);
    void setRotation(int value);
    void setBright(int value);
    void setDimmer(int value);
    void setSoundEnabled(int value);

    void setIrTxPin(int value);
    void setIrRxPin(int value);

    void setRfTxPin(int value);
    void setRfRxPin(int value);
    void setRfModule(RFModules value);
    void setRfFreq(float value, int fxdFreq = NULL);
    void setRfScanRange(int value, int fxdFreq = 0);

    void setRfidModule(RFIDModules value);

private:
    const char *filepath = "/bruceNew.conf";
};

#endif
