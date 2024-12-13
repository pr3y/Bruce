#include "config.h"
#include "sd_functions.h"


JsonDocument BruceConfig::toJson() const {
    JsonDocument jsonDoc;
    JsonObject setting = jsonDoc.to<JsonObject>();

    setting["priColor"] = String(priColor, HEX);
    setting["secColor"] = String(secColor, HEX);
    setting["bgColor"] = String(bgColor, HEX);

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

    setting["gpsBaudrate"] = gpsBaudrate;

    setting["startupApp"] = startupApp;
    setting["wigleBasicToken"] = wigleBasicToken;
    setting["devMode"] = devMode;

    JsonArray dm = setting.createNestedArray("disabledMenus");
    for(int i=0; i < disabledMenus.size(); i++){
        dm.add(disabledMenus[i]);
    }

    JsonArray qrArray = setting.createNestedArray("qrCodes");
    for (const auto& entry : qrCodes) {
        JsonObject qrEntry = qrArray.createNestedObject();
        qrEntry["menuName"] = entry.menuName;
        qrEntry["content"] = entry.content;
    }

    return jsonDoc;
}


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

    if(!setting["priColor"].isNull())  { priColor  = strtoul(setting["priColor"], nullptr, 16); } else { count++; log_e("Fail"); }
    if(!setting["secColor"].isNull())  { secColor  = strtoul(setting["secColor"], nullptr, 16); } else { count++; log_e("Fail"); }
    if(!setting["bgColor"].isNull())   { bgColor   = strtoul(setting["bgColor"], nullptr, 16); } else { count++; log_e("Fail"); }

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

    if(!setting["gpsBaudrate"].isNull()) { gpsBaudrate  = setting["gpsBaudrate"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["startupApp"].isNull())      { startupApp  = setting["startupApp"].as<String>(); } else { count++; log_e("Fail"); }
    if(!setting["wigleBasicToken"].isNull()) { wigleBasicToken  = setting["wigleBasicToken"].as<String>(); } else { count++; log_e("Fail"); }
    if(!setting["devMode"].isNull())         { devMode  = setting["devMode"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["disabledMenus"].isNull()) {
        disabledMenus.clear();
        JsonArray dm = setting["disabledMenus"].as<JsonArray>();
        for (JsonVariant e : dm) {
            disabledMenus.push_back(e.as<String>());
        }
    } else { count++; log_e("Fail"); }

    if (!setting["qrCodes"].isNull()) {
        qrCodes.clear();
        JsonArray qrArray = setting["qrCodes"].as<JsonArray>();
        for (JsonObject qrEntry : qrArray) {
            String menuName = qrEntry["menuName"].as<String>();
            String content = qrEntry["content"].as<String>();
            qrCodes.push_back({menuName, content});
            }
    } else {
        count++;
        log_e("Fail to load qrCodes");
    }

    validateConfig();
    if (count>0) saveFile();

    log_i("Using config from file");
}


void BruceConfig::saveFile() {
    FS *fs = &LittleFS;
    JsonDocument jsonDoc = toJson();

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

    file.close();

    if (setupSdCard()) copyToFs(LittleFS, SD, filepath,false);
}


void BruceConfig::validateConfig() {
    validateTheme();
    validateRotationValue();
    validateDimmerValue();
    validateBrightValue();
    validateTmzValue();
    validateSoundEnabledValue();
    validateWifiAtStartupValue();
    validateRfScanRangeValue();
    validateRfModuleValue();
    validateRfidModuleValue();
    validateGpsBaudrateValue();
    validateDevModeValue();
}


void BruceConfig::setTheme(uint16_t primary, uint16_t secondary, uint16_t background) {
    priColor = primary;
    secColor = secondary == NULL ? primary - 0x2000 : secondary;
    bgColor = background == NULL ? 0x0 : background;
    validateTheme();
    saveFile();
}


void BruceConfig::validateTheme() {
    if (priColor < 0 || priColor > 0xFFFF) priColor = DEFAULT_PRICOLOR;
    if (secColor < 0 || secColor > 0xFFFF) secColor = priColor - 0x2000;
    if (bgColor  < 0 || bgColor  > 0xFFFF) bgColor  = 0;
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
    validateTmzValue();
    saveFile();
}


void BruceConfig::validateTmzValue() {
    if (tmz < -12 || tmz > 12) tmz = 0;
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


void BruceConfig::setWifiApCreds(const String& ssid, const String& pwd) {
    wifiAp.ssid = ssid;
    wifiAp.pwd = pwd;
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
    validateRfModuleValue();
    saveFile();
}


void BruceConfig::validateRfModuleValue() {
    if (rfModule != M5_RF_MODULE && rfModule != CC1101_SPI_MODULE) {
        rfModule = M5_RF_MODULE;
    }
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
    validateRfidModuleValue();
    saveFile();
}


void BruceConfig::validateRfidModuleValue() {
    if (
        rfidModule != M5_RFID2_MODULE
        && rfidModule != PN532_I2C_MODULE
        && rfidModule != PN532_SPI_MODULE
    ) {
        rfidModule = M5_RFID2_MODULE;
    }
}


void BruceConfig::setGpsBaudrate(int value) {
    gpsBaudrate = value;
    validateGpsBaudrateValue();
    saveFile();
}


void BruceConfig::validateGpsBaudrateValue() {
    if (gpsBaudrate != 9600 && gpsBaudrate != 115200) gpsBaudrate = 9600;
}


void BruceConfig::setStartupApp(String value) {
    startupApp = value;
    saveFile();
}


void BruceConfig::setWigleBasicToken(String value) {
    wigleBasicToken = value;
    saveFile();
}


void BruceConfig::setDevMode(int value) {
    devMode = value;
    validateDevModeValue();
    saveFile();
}


void BruceConfig::validateDevModeValue() {
    if (devMode > 1) devMode = 1;
}


void BruceConfig::addDisabledMenu(String value) {
    // TODO: check if duplicate
    disabledMenus.push_back(value);
    saveFile();
}

void BruceConfig::addQrCodeEntry(const String& menuName, const String& content) {
    qrCodes.push_back({menuName, content});
    saveFile();
}

void BruceConfig::removeQrCodeEntry(const String& menuName) {
    qrCodes.erase(std::remove_if(qrCodes.begin(), qrCodes.end(), 
        [&](const QrCodeEntry& entry) { return entry.menuName == menuName; }), qrCodes.end());
    saveFile();
}
