#ifndef __BRUCE_CONFIG_H__
#define __BRUCE_CONFIG_H__

// #include "globals.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <vector>

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
    struct WiFiCredential {
        String ssid;
        String pwd;
    };
    struct Credential {
        String user;
        String pwd;
    };

    const char *filepath = "/bruce.conf";

    // Theme colors in RGB565 format
    uint16_t priColor = DEFAULT_PRICOLOR;
    uint16_t secColor = DEFAULT_PRICOLOR-0x2000;
    uint16_t bgColor  = 0x0000;

    int rotation = ROTATION > 1 ? 3 : 1;
    int dimmerSet = 10;
    int bright = 100;
    int tmz = 0;
    int soundEnabled = 1;
    int wifiAtStartup = 0;

    Credential webUI = {"admin", "bruce"};
    WiFiCredential wifiAp = {"BruceNet", "brucenet"};
    std::map<String, String> wifi = {};

    int irTx = LED;
    int irRx = GROVE_SCL;

    int rfTx = GROVE_SDA;
    int rfRx = GROVE_SCL;
    int rfModule = M5_RF_MODULE;
    float rfFreq = 433.92;
    int rfFxdFreq = 1;
    int rfScanRange = 3;

    int rfidModule = M5_RFID2_MODULE;

    int gpsBaudrate = 9600;

    String startupApp = "";
    String wigleBasicToken = "";
    int devMode = 0;

    std::vector<String> disabledMenus = {};

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
    void validateConfig();
    JsonDocument toJson() const;

    void setTheme(uint16_t primary, uint16_t secondary = NULL, uint16_t background = NULL);
    void validateTheme();

    void setRotation(int value);
    void validateRotationValue();
    void setDimmer(int value);
    void validateDimmerValue();
    void setBright(int value);
    void validateBrightValue();
    void setTmz(int value);
    void validateTmzValue();
    void setSoundEnabled(int value);
    void validateSoundEnabledValue();
    void setWifiAtStartup(int value);
    void validateWifiAtStartupValue();

    void setWebUICreds(const String& usr, const String& pwd);
    void setWifiApCreds(const String& ssid, const String& pwd);
    void addWifiCredential(const String& ssid, const String& pwd);
    String getWifiPassword(const String& ssid) const;

    void setIrTxPin(int value);
    void setIrRxPin(int value);

    void setRfTxPin(int value);
    void setRfRxPin(int value);
    void setRfModule(RFModules value);
    void validateRfModuleValue();
    void setRfFreq(float value, int fxdFreq = NULL);
    void setRfFxdFreq(float value);
    void setRfScanRange(int value, int fxdFreq = 0);
    void validateRfScanRangeValue();

    void setRfidModule(RFIDModules value);
    void validateRfidModuleValue();

    void setGpsBaudrate(int value);
    void validateGpsBaudrateValue();

    void setStartupApp(String value);
    void setWigleBasicToken(String value);
    void setDevMode(int value);
    void validateDevModeValue();
    void addDisabledMenu(String value);
    // TODO: removeDisabledMenu(String value);
};

#endif
