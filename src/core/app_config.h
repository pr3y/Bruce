#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <Arduino.h>
#include <ArduinoJson.h>
// #include "core/sd_functions.h"

#define EEPROM_ROT            (0)
#define EEPROM_DIMMER         (1)
#define EEPROM_BRIGHT         (2)
#define EEPROM_IR_TX          (6)
#define EEPROM_IR_RX          (7)
#define EEPROM_RF_TX          (8)
#define EEPROM_RF_RX          (9)
#define EEPROM_TMZ            (10)
#define EEPROM_FGCOLOR0       (11)
#define EEPROM_FGCOLOR1       (12)
#define EEPROM_RF_MODULE      (13)
#define EEPROM_RFID_MODULE    (14)



class AppConfig {
public:
    inline int getSoundEnabled(void) { return soundEnabled; };
    inline void setSoundEnabled(int value) { soundEnabled = value; };

    inline int isDevMode(void) { return devMode; };

    inline String getWigleBasicToken(void) { return wigleBasicToken; };

    void getConfigs(void);
    void saveConfigs(void);

    void loadEEPROM(void);

private:
    // int dimmerSet;
    // int rotation;
    // int tmz = 3;
    // int bright = 100;
    int devMode = 0;
    int soundEnabled = 1;

    // int IrTx;
    // int IrRx;
    // int RfTx;
    // int RfRx;
    // int RfModule = M5_RF_MODULE;
    // float RfFreq = 433.92;
    // int RfidModule = M5_RFID2_MODULE;

    // String wui_usr = "admin";
    // String wui_pwd = "bruce";

    String wigleBasicToken = "";

    void _setConfigValues(JsonDocument setting);
    void _syncEEPROMValues(void);
};

#endif
