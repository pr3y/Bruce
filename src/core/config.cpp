#include "config.h"
#include "sd_functions.h"
#include <ArduinoJson.h>


void BruceConfig::fromFile() {
    FS *fs;
    if(!getFsStorage(fs)) return;

    if(!fs->exists(filepath)) return saveFile();

    File file;
    file = fs->open(filepath, FILE_READ);
    if (!file) {
        log_e("Config file not found. Using default values");
        return;
    }

    // Deserialize the JSON document
    JsonDocument jsonDoc;
    if (deserializeJson(jsonDoc, file)) {
        log_e("Failed to read config file, using default configuration");
        return;
    }
    file.close();

    JsonObject setting = jsonDoc.as<JsonObject>();
    int count = 0;

    if(!setting["rot"].isNull())       { rotation  = setting["rot"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["dimmerSet"].isNull()) { dimmerSet = setting["dimmerSet"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["bright"].isNull())    { bright    = setting["bright"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["tmz"].isNull())       { tmz       = setting["tmz"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["wuiUsr"].isNull())    { wuiUsr    = setting["wuiUsr"].as<String>(); } else { count++; log_e("Fail"); }
    if(!setting["wuiPwd"].isNull())    { wuiPwd    = setting["wuiPwd"].as<String>(); } else { count++; log_e("Fail"); }

    if(!setting["priColor"].isNull())  { priColor  = setting["priColor"].as<uint16_t>(); } else { count++; log_e("Fail"); }
    if(!setting["secColor"].isNull())  { secColor  = setting["secColor"].as<uint16_t>(); } else { count++; log_e("Fail"); }
    if(!setting["bgColor"].isNull())   { bgColor   = setting["bgColor"].as<uint16_t>(); } else { count++; log_e("Fail"); }

    if(!setting["irTx"].isNull())        { irTx        = setting["irTx"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["irRx"].isNull())        { irRx        = setting["irRx"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfTx"].isNull())        { rfTx        = setting["rfTx"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfRx"].isNull())        { rfRx        = setting["rfRx"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfModule"].isNull())    { rfModule    = setting["rfModule"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfFreq"].isNull())      { rfFreq      = setting["rfFreq"].as<float>(); } else { count++; log_e("Fail"); }
    if(!setting["rfFxdFreq"].isNull())   { rfFxdFreq   = setting["rfFxdFreq"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfScanRange"].isNull()) { rfScanRange = setting["rfScanRange"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfidModule"].isNull())  { rfidModule  = setting["rfidModule"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["wigleBasicToken"].isNull()) { wigleBasicToken  = setting["wigleBasicToken"].as<String>(); } else { count++; log_e("Fail"); }
    if(!setting["devMode"].isNull())         { devMode  = setting["devMode"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["soundEnabled"].isNull())    { soundEnabled = setting["soundEnabled"].as<int>(); } else { count++; log_e("Fail"); }

    // if(!setting.containsKey("wifi"))  { count++; log_e("Fail"); }

    // if(setting.containsKey("wifi_ap")) {
    //     JsonObject wifiAp = setting["wifi_ap"].as<JsonObject>();
    //     if (wifiAp.containsKey("ssid")) { ap_ssid = wifiAp["ssid"].as<String>(); } else { count++; log_e("Fail"); }
    //     if (wifiAp.containsKey("pwd"))  { ap_pwd  = wifiAp["pwd"].as<String>(); } else { count++; log_e("Fail"); }
    // } else {
    //     count++; log_e("Fail");
    // }

    if(dimmerSet < 0) dimmerSet = 10;
    // log_i("Brightness: %d", bright);
    // setBrightness(bright);
    if(count>0) saveFile();
    // sync_eeprom_values();

    log_i("Using config from file");
}


void BruceConfig::saveFile() {
    FS *fs;
    if(!getFsStorage(fs)) return;

    JsonDocument jsonDoc;
    JsonObject setting = jsonDoc.to<JsonObject>();

    setting["rot"] = rotation;
    setting["dimmerSet"] = dimmerSet;
    setting["bright"] = bright;
    setting["tmz"] = tmz;
    setting["wuiUsr"] = wuiUsr;
    setting["wuiPwd"] = wuiPwd;
    setting["priColor"] = priColor;
    setting["secColor"] = secColor;
    setting["bgColor"] = bgColor;
    setting["irTx"] = irTx;
    setting["irRx"] = irRx;
    setting["rfTx"] = rfTx;
    setting["rfRx"] = rfRx;
    setting["rfModule"] = rfModule;
    setting["rfFreq"] = rfFreq;
    setting["rfFxdFreq"] = rfFxdFreq;
    setting["rfScanRange"] = rfScanRange;
    setting["rfidModule"] = rfidModule;
    setting["wigleBasicToken"] = wigleBasicToken;
    setting["devMode"] = devMode;
    setting["soundEnabled"] = soundEnabled;

    // if(!setting.containsKey("wifi")) {
    //     JsonArray WifiList = setting["wifi"].to<JsonArray>();
    //     if(WifiList.size()<1) {
    //     JsonObject WifiObj = WifiList.add<JsonObject>();
    //     WifiObj["ssid"] = "myNetSSID";
    //     WifiObj["pwd"] = "myNetPassword";
    //     }
    // }
    // if(!setting.containsKey("wifi_ap")) {
    //     JsonObject WifiAp = setting["wifi_ap"].to<JsonObject>();
    //     WifiAp["ssid"] = ap_ssid;
    //     WifiAp["pwd"] = ap_pwd;
    // }

    // Open file for writing
    File file = fs->open(filepath, FILE_WRITE);
    if (!file) {
        log_e("Failed to open config file");
        file.close();
        return;
    };

    // Serialize JSON to file
    serializeJsonPretty(jsonDoc, Serial);
    if (serializeJsonPretty(jsonDoc, file) < 5) log_e("Failed to write config file");
    else log_i("config file written successfully");

    // Close the file
    file.close();
}


void BruceConfig::setTheme(uint16_t primary, uint16_t secondary) {
    priColor = primary;
    secColor = secondary == NULL ? primary - 0x2000 : secondary;
    saveFile();
}


void BruceConfig::setWebUICreds(String usr, String pwd) {
    wuiUsr = usr;
    wuiPwd = pwd;
    saveFile();
}


void BruceConfig::setRotation(int value) {
    if (value!=1 && value!=3) value = 1;
    rotation = value;
    saveFile();
}


void BruceConfig::setSoundEnabled(int value) {
    if (value > 1) value = 1;
    soundEnabled = value;
    saveFile();
}


void BruceConfig::setBright(int value) {
    if (value > 100) value = 100;
    bright = value;
    saveFile();
}


void BruceConfig::setDimmer(int value) {
    if (value < 0 || value > 60) value = 0;
    dimmerSet = value;
    saveFile();
}


void BruceConfig::setIrTxPin(int value) {
    irTx = value;
    saveFile();
}


void BruceConfig::setIrRxPin(int value) {
    irRx = value;
    saveFile();
}


void BruceConfig::setRfTxPin(int value) {
    rfTx = value;
    saveFile();
}


void BruceConfig::setRfRxPin(int value) {
    rfRx = value;
    saveFile();
}


void BruceConfig::setRfModule(RFModules value) {
    rfModule = value;
    saveFile();
}


void BruceConfig::setRfFreq(float value, int fxdFreq) {
    rfFreq = value;
    if (fxdFreq != NULL) rfFxdFreq = fxdFreq;
    saveFile();
}


void BruceConfig::setRfScanRange(int value, int fxdFreq) {
    rfScanRange = value;
    rfFxdFreq = fxdFreq;
    saveFile();
}


void BruceConfig::setRfidModule(RFIDModules value) {
    rfidModule = value;
    saveFile();
}
