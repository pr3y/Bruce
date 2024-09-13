#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

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
#define EEPROM_PWD            (20)


enum RFIDModules {
  M5_RFID2_MODULE  = 0,
  PN532_I2C_MODULE = 1,
  PN532_SPI_MODULE = 2,
};


enum RFModules {
  M5_RF_MODULE = 0,
  CC1101_SPI_MODULE = 1,
};


class AppConfig {
public:
    inline void setBruceFgColor(char16_t value) {
        Serial.printf("Setting BruceFgColor to 0x%x\n", value);

        EEPROM.begin(EEPROMSIZE);
        EEPROM.write(EEPROM_FGCOLOR0, int((value >> 8) & 0x00FF));
        EEPROM.write(EEPROM_FGCOLOR1, int(value & 0x00FF));
        EEPROM.commit();
        EEPROM.end();

        saveConfigs();
        Serial.println("Setting BruceFgColor success");
    };

    inline int getDimmer(void) { return dimmerSet; };
    inline void setDimmer(int value) {
        Serial.printf("Setting Dimmer to %d\n", value);
        dimmerSet = value;
        writeEEPROM(EEPROM_DIMMER, value);
        saveConfigs();
        Serial.println("Setting Dimmer success");
    };

    inline int getRotation(void) { return rotation; };
    inline void setRotation(int value) {
        Serial.printf("Setting Rotation to %d\n", value);
        rotation = value;
        writeEEPROM(EEPROM_ROT, value);
        saveConfigs();
        Serial.println("Setting Rotation success");
    };

    inline int getBright(void) { return bright; };
    inline void setBright(int value) {
        Serial.printf("Setting Bright to %d\n", value);
        bright = value;
        writeEEPROM(EEPROM_BRIGHT, value);
        saveConfigs();
        Serial.println("Setting Bright success");
    };

    inline int getTmz(void) { return tmz; };
    inline void setTmz(int value) {
        Serial.printf("Setting Tmz to %d\n", value);
        tmz = value;
        writeEEPROM(EEPROM_TMZ, value);
        saveConfigs();
        Serial.println("Setting Tmz success");
    };

    inline String getWuiUsr(void) { return wui_usr; };
    inline void setWuiUsr(String value) {
        Serial.println("Setting WuiUsr to "+value);
        wui_usr = value;
        saveConfigs();
        Serial.println("Setting WuiUsr success");
    };
    inline String getWuiPwd(void) { return wui_pwd; };
    inline void setWuiPwd(String value) {
        Serial.println("Setting WuiPwd to "+value);
        wui_pwd = value;
        saveConfigs();
        Serial.println("Setting WuiPwd success");
    };

    inline int getSoundEnabled(void) { return soundEnabled; };
    inline void setSoundEnabled(int value) {
        Serial.printf("Setting SoundEnabled to %d\n", value);
        soundEnabled = value;
        saveConfigs();
        Serial.println("Setting SoundEnabled success");
    };

    inline int getIrTx(void) { return IrTx; };
    inline void setIrTx(int value) {
        Serial.printf("Setting IrTx to %d\n", value);
        IrTx = value;
        writeEEPROM(EEPROM_IR_TX, value);
        saveConfigs();
        Serial.println("Setting IrTx success");
    };
    inline int getIrRx(void) { return IrRx; };
    inline void setIrRx(int value) {
        Serial.printf("Setting IrRx to %d\n", value);
        IrRx = value;
        writeEEPROM(EEPROM_IR_RX, value);
        saveConfigs();
        Serial.println("Setting IrRx success");
    };

    inline int getRfTx(void) { return RfTx; };
    inline void setRfTx(int value) {
        Serial.printf("Setting RfTx to %d\n", value);
        RfTx = value;
        writeEEPROM(EEPROM_RF_TX, value);
        saveConfigs();
        Serial.println("Setting RfTx success");
    };
    inline int getRfRx(void) { return RfRx; };
    inline void setRfRx(int value) {
        Serial.printf("Setting RfRx to %d\n", value);
        RfRx = value;
        writeEEPROM(EEPROM_RF_RX, value);
        saveConfigs();
        Serial.println("Setting RfRx success");
    };

    inline int getRfModule(void) { return RfModule; };
    inline void setRfModule(int value) {
        Serial.printf("Setting RfModule to %d\n", value);
        RfModule = value;
        writeEEPROM(EEPROM_RF_MODULE, value);
        saveConfigs();
        Serial.println("Setting RfModule success");
    };
    inline float getRfFreq(void) { return RfFreq; };
    inline void setRfFreq(float value) {
        Serial.printf("Setting RfFreq to %f\n", value);
        RfFreq = value;
        saveConfigs();
        Serial.println("Setting RfFreq success");
    };
    inline int getRfidModule(void) { return RfidModule; };
    inline void setRfidModule(int value) {
        Serial.printf("Setting RfidModule to %d\n", value);
        RfidModule = value;
        writeEEPROM(EEPROM_RFID_MODULE, value);
        saveConfigs();
        Serial.println("Setting RfidModule success");
    };

    inline int getDevMode(void) { return devMode; };
    inline void setDevMode(int value) {
        Serial.printf("Setting DevMode to %d\n", value);
        devMode = value;
        saveConfigs();
        Serial.println("Setting DevMode success");
    };

    inline String getWigleBasicToken(void) { return wigleBasicToken; };
    inline void setWigleBasicToken(String value) {
        Serial.println("Setting WigleBasicToken to "+value);
        wigleBasicToken = value;
        saveConfigs();
        Serial.println("Setting WigleBasicToken success");
    };

    void appendWifi(String ssid, String pwd);

    void getConfigs(void);
    void saveConfigs(void);

    void loadEEPROM(void);
    void writeEEPROM(int address, uint8_t val);

private:
    int dimmerSet;
    int rotation;
    int tmz = 3;
    int bright = 100;
    int devMode = 0;
    int soundEnabled = 1;

    int IrTx;
    int IrRx;
    int RfTx;
    int RfRx;
    int RfModule = M5_RF_MODULE;
    float RfFreq = 433.92;
    int RfidModule = M5_RFID2_MODULE;

    String wui_usr = "admin";
    String wui_pwd = "bruce";

    String wigleBasicToken = "";

    void _setConfigValues(JsonDocument setting);
    void _syncEEPROMValues(void);
};

#endif
