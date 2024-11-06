#include "config.h"
#include "sd_functions.h"


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

    if(!setting["priColor"].isNull())  { priColor  = setting["priColor"].as<uint16_t>(); } else { count++; log_e("Fail"); }
    if(!setting["secColor"].isNull())  { secColor  = setting["secColor"].as<uint16_t>(); } else { count++; log_e("Fail"); }
    if(!setting["bgColor"].isNull())   { bgColor   = setting["bgColor"].as<uint16_t>(); } else { count++; log_e("Fail"); }

    if(!setting["rot"].isNull())       { rotation  = setting["rot"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["dimmerSet"].isNull()) { dimmerSet = setting["dimmerSet"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["bright"].isNull())    { bright    = setting["bright"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["tmz"].isNull())       { tmz       = setting["tmz"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["soundEnabled"].isNull())    { soundEnabled  = setting["soundEnabled"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["wifiAtStartup"].isNull())   { wifiAtStartup = setting["wifiAtStartup"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["webUI"].isNull()) {
        JsonObject webUIObj = setting["webUI"].as<JsonObject>();
        webUI.user = webUIObj["user"].as<String>();
        webUI.pwd  = webUIObj["pwd"].as<String>();
    } else { count++; log_e("Fail"); }

    if(!setting["wifiAp"].isNull()) {
        JsonObject wifiApObj = setting["wifiAp"].as<JsonObject>();
        wifiAp.ssid = wifiApObj["ssid"].as<String>();
        wifiAp.pwd  = wifiApObj["pwd"].as<String>();
    } else { count++; log_e("Fail"); }

    if(!setting["wifi"].isNull()) {
        wifi.clear();
        for (JsonPair kv : setting["wifi"].as<JsonObject>())
            wifi[kv.key().c_str()] = kv.value().as<String>();
    } else { count++; log_e("Fail"); }

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

    validateConfig();
    if(count>0) saveFile();

    log_i("Using config from file");
}


void BruceConfig::saveFile() {
    FS *fs;
    if(!getFsStorage(fs)) return;

    JsonDocument jsonDoc;
    JsonObject setting = jsonDoc.to<JsonObject>();

    setting["priColor"] = priColor;
    setting["secColor"] = secColor;
    setting["bgColor"] = bgColor;

    setting["rot"] = rotation;
    setting["dimmerSet"] = dimmerSet;
    setting["bright"] = bright;
    setting["tmz"] = tmz;
    setting["soundEnabled"] = soundEnabled;
    setting["wifiAtStartup"] = wifiAtStartup;

    JsonObject _webUI = setting.createNestedObject("webUI");
    _webUI["user"] = webUI.user;
    _webUI["pwd"] = webUI.pwd;

    JsonObject _wifiAp = setting.createNestedObject("wifiAp");
    _wifiAp["ssid"] = wifiAp.ssid;
    _wifiAp["pwd"] = wifiAp.pwd;

    JsonObject _wifi = setting.createNestedObject("wifi");
    for (const auto& pair : wifi) {
        _wifi[pair.first] = pair.second;
    }

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


void BruceConfig::validateConfig() {
    validateRotationValue();
    validateDimmerValue();
    validateBrightValue();
    validateSoundEnabledValue();
    validateWifiAtStartupValue();
    validateRfScanRangeValue();
}


void BruceConfig::setTheme(uint16_t primary, uint16_t secondary, uint16_t background) {
    priColor = primary;
    secColor = secondary == NULL ? primary - 0x2000 : secondary;
    bgColor = background == NULL ? 0x0 : background;
    saveFile();
}


void BruceConfig::setRotation(int value) {
    rotation = value;
    validateRotationValue();
    saveFile();
}


void BruceConfig::validateRotationValue() {
    if (rotation!=1 && rotation!=3) rotation = 1;
}


void BruceConfig::setDimmer(int value) {
    dimmerSet = value;
    validateDimmerValue();
    saveFile();
}


void BruceConfig::validateDimmerValue() {
    if (dimmerSet < 0) dimmerSet = 10;
    if (dimmerSet > 60) dimmerSet = 0;
}


void BruceConfig::setBright(int value) {
    bright = value;
    validateBrightValue();
    saveFile();
}


void BruceConfig::validateBrightValue() {
    if (bright > 100) bright = 100;
}


void BruceConfig::setTmz(int value) {
    tmz = value;
    saveFile();
}


void BruceConfig::setSoundEnabled(int value) {
    soundEnabled = value;
    validateSoundEnabledValue();
    saveFile();
}


void BruceConfig::validateSoundEnabledValue() {
    if (soundEnabled > 1) soundEnabled = 1;
}

void BruceConfig::setWifiAtStartup(int value) {
    wifiAtStartup = value;
    validateWifiAtStartupValue();
    saveFile();
}


void BruceConfig::validateWifiAtStartupValue() {
    if (wifiAtStartup > 1) wifiAtStartup = 1;
}


void BruceConfig::setWebUICreds(const String& usr, const String& pwd) {
    webUI.user = usr;
    webUI.pwd = pwd;
    saveFile();
}


void BruceConfig::addWifiCredential(const String& ssid, const String& pwd) {
    wifi[ssid] = pwd;
    saveFile();
}


String BruceConfig::getWifiPassword(const String& ssid) const {
    auto it = wifi.find(ssid);
    if (it != wifi.end()) return it->second;
    return "";
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


void BruceConfig::setRfFxdFreq(float value) {
    rfFxdFreq = value;
    saveFile();
}


void BruceConfig::setRfScanRange(int value, int fxdFreq) {
    rfScanRange = value;
    rfFxdFreq = fxdFreq;
    validateRfScanRangeValue();
    saveFile();
}


void BruceConfig::validateRfScanRangeValue() {
    if (rfScanRange < 0 || rfScanRange > 3) rfScanRange = 3;
}


void BruceConfig::setRfidModule(RFIDModules value) {
    rfidModule = value;
    saveFile();
}
