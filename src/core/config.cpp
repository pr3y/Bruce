#include "config.h"
#include "sd_functions.h"

JsonDocument BruceConfig::toJson() const {
    JsonDocument jsonDoc;
    JsonObject setting = jsonDoc.to<JsonObject>();

    setting["priColor"] = String(priColor, HEX);
    setting["secColor"] = String(secColor, HEX);
    setting["bgColor"] = String(bgColor, HEX);
    setting["themeFile"] = themePath;
    setting["themeOnSd"] = theme.fs;

    setting["rot"] = rotation;
    setting["dimmerSet"] = dimmerSet;
    setting["bright"] = bright;
    setting["tmz"] = tmz;
    setting["soundEnabled"] = soundEnabled;
    setting["wifiAtStartup"] = wifiAtStartup;
    setting["instantBoot"] = instantBoot;

    setting["ledBright"] = ledBright;
    setting["ledColor"] = String(ledColor, HEX);

    JsonObject _webUI = setting["webUI"].to<JsonObject>();
    _webUI["user"] = webUI.user;
    _webUI["pwd"] = webUI.pwd;

    JsonObject _wifiAp = setting["wifiAp"].to<JsonObject>();
    _wifiAp["ssid"] = wifiAp.ssid;
    _wifiAp["pwd"] = wifiAp.pwd;

    JsonArray _evilWifiNames = setting["evilWifiNames"].to<JsonArray>();
    for (auto key : evilWifiNames) _evilWifiNames.add(key);

    setting["bleName"] = bleName;

    JsonObject _wifi = setting["wifi"].to<JsonObject>();
    for (const auto &pair : wifi) { _wifi[pair.first] = pair.second; }

    setting["irTx"] = irTx;
    setting["irTxRepeats"] = irTxRepeats;
    setting["irRx"] = irRx;

    setting["rfTx"] = rfTx;
    setting["rfRx"] = rfRx;
    setting["rfModule"] = rfModule;
    setting["rfFreq"] = rfFreq;
    setting["rfFxdFreq"] = rfFxdFreq;
    setting["rfScanRange"] = rfScanRange;

    setting["rfidModule"] = rfidModule;

    setting["iButton"] = iButton;

    JsonArray _mifareKeys = setting["mifareKeys"].to<JsonArray>();
    for (auto key : mifareKeys) _mifareKeys.add(key);

    setting["gpsBaudrate"] = gpsBaudrate;

    setting["startupApp"] = startupApp;
    setting["wigleBasicToken"] = wigleBasicToken;
    setting["devMode"] = devMode;
    setting["colorInverted"] = colorInverted;

    JsonArray dm = setting["disabledMenus"].to<JsonArray>();
    for (int i = 0; i < disabledMenus.size(); i++) { dm.add(disabledMenus[i]); }

    JsonArray qrArray = setting["qrCodes"].to<JsonArray>();
    for (const auto &entry : qrCodes) {
        JsonObject qrEntry = qrArray.add<JsonObject>();
        qrEntry["menuName"] = entry.menuName;
        qrEntry["content"] = entry.content;
    }

    JsonObject _CC1101 = setting["CC1101_Pins"].to<JsonObject>();
    _CC1101["sck"] = CC1101_bus.sck;
    _CC1101["miso"] = CC1101_bus.miso;
    _CC1101["mosi"] = CC1101_bus.mosi;
    _CC1101["cs"] = CC1101_bus.cs;
    _CC1101["io0"] = CC1101_bus.io0;
    _CC1101["io2"] = CC1101_bus.io2;

    JsonObject _NRF = setting["NRF24_Pins"].to<JsonObject>();
    _NRF["sck"] = NRF24_bus.sck;
    _NRF["miso"] = NRF24_bus.miso;
    _NRF["mosi"] = NRF24_bus.mosi;
    _NRF["cs"] = NRF24_bus.cs;
    _NRF["io0"] = NRF24_bus.io0;

    JsonObject _SD = setting["SDCard_Pins"].to<JsonObject>();
    _SD["sck"] = SDCARD_bus.sck;
    _SD["miso"] = SDCARD_bus.miso;
    _SD["mosi"] = SDCARD_bus.mosi;
    _SD["cs"] = SDCARD_bus.cs;
    _SD["io0"] = SDCARD_bus.io0;

    return jsonDoc;
}

void BruceConfig::fromFile() {
    FS *fs;
    if (!getFsStorage(fs)) return;

    if (!fs->exists(filepath)) return saveFile();

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

    if (!setting["priColor"].isNull()) {
        priColor = strtoul(setting["priColor"], nullptr, 16);
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["secColor"].isNull()) {
        secColor = strtoul(setting["secColor"], nullptr, 16);
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["bgColor"].isNull()) {
        bgColor = strtoul(setting["bgColor"], nullptr, 16);
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["themeFile"].isNull()) {
        themePath = setting["themeFile"].as<String>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["themeOnSd"].isNull()) {
        theme.fs = setting["themeOnSd"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["rot"].isNull()) {
        rotation = setting["rot"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["dimmerSet"].isNull()) {
        dimmerSet = setting["dimmerSet"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["bright"].isNull()) {
        bright = setting["bright"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["tmz"].isNull()) {
        tmz = setting["tmz"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["soundEnabled"].isNull()) {
        soundEnabled = setting["soundEnabled"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["wifiAtStartup"].isNull()) {
        wifiAtStartup = setting["wifiAtStartup"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["instantBoot"].isNull()) {
        instantBoot = setting["instantBoot"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["ledBright"].isNull()) {
        ledBright = setting["ledBright"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["ledColor"].isNull()) {
        ledColor = strtoul(setting["ledColor"], nullptr, 16);
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["webUI"].isNull()) {
        JsonObject webUIObj = setting["webUI"].as<JsonObject>();
        webUI.user = webUIObj["user"].as<String>();
        webUI.pwd = webUIObj["pwd"].as<String>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["wifiAp"].isNull()) {
        JsonObject wifiApObj = setting["wifiAp"].as<JsonObject>();
        wifiAp.ssid = wifiApObj["ssid"].as<String>();
        wifiAp.pwd = wifiApObj["pwd"].as<String>();
    } else {
        count++;
        log_e("Fail");
    }

    // SPI Pins list
    if (!setting["CC1101_Pins"].isNull()) {
        JsonObject Pins = setting["CC1101_Pins"].as<JsonObject>();
        CC1101_bus.sck = (gpio_num_t)Pins["sck"].as<int>();
        CC1101_bus.miso = (gpio_num_t)Pins["miso"].as<int>();
        CC1101_bus.mosi = (gpio_num_t)Pins["mosi"].as<int>();
        CC1101_bus.cs = (gpio_num_t)Pins["cs"].as<int>();
        CC1101_bus.io0 = (gpio_num_t)Pins["io0"].as<int>();
        CC1101_bus.io2 = (gpio_num_t)Pins["io2"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["NRF24_Pins"].isNull()) {
        JsonObject Pins = setting["NRF24_Pins"].as<JsonObject>();
        NRF24_bus.sck = (gpio_num_t)Pins["sck"].as<int>();
        NRF24_bus.miso = (gpio_num_t)Pins["miso"].as<int>();
        NRF24_bus.mosi = (gpio_num_t)Pins["mosi"].as<int>();
        NRF24_bus.cs = (gpio_num_t)Pins["cs"].as<int>();
        NRF24_bus.io0 = (gpio_num_t)Pins["io0"].as<int>();
        NRF24_bus.io2 = (gpio_num_t)Pins["io2"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["SDCard_Pins"].isNull()) {
        JsonObject Pins = setting["SDCard_Pins"].as<JsonObject>();
        SDCARD_bus.sck = (gpio_num_t)Pins["sck"].as<int>();
        SDCARD_bus.miso = (gpio_num_t)Pins["miso"].as<int>();
        SDCARD_bus.mosi = (gpio_num_t)Pins["mosi"].as<int>();
        SDCARD_bus.cs = (gpio_num_t)Pins["cs"].as<int>();
        SDCARD_bus.io0 = (gpio_num_t)Pins["io0"].as<int>();
        SDCARD_bus.io2 = (gpio_num_t)Pins["io2"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    // Wifi List
    if (!setting["wifi"].isNull()) {
        wifi.clear();
        for (JsonPair kv : setting["wifi"].as<JsonObject>()) wifi[kv.key().c_str()] = kv.value().as<String>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["evilWifiNames"].isNull()) {
        evilWifiNames.clear();
        JsonArray _evilWifiNames = setting["evilWifiNames"].as<JsonArray>();
        for (JsonVariant key : _evilWifiNames) evilWifiNames.insert(key.as<String>());
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["bleName"].isNull()) {
        bleName = setting["bleName"].as<String>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["irTx"].isNull()) {
        irTx = setting["irTx"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["irTxRepeats"].isNull()) {
        irTxRepeats = setting["irTxRepeats"].as<uint8_t>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["irRx"].isNull()) {
        irRx = setting["irRx"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["rfTx"].isNull()) {
        rfTx = setting["rfTx"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["rfRx"].isNull()) {
        rfRx = setting["rfRx"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["rfModule"].isNull()) {
        rfModule = setting["rfModule"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["rfFreq"].isNull()) {
        rfFreq = setting["rfFreq"].as<float>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["rfFxdFreq"].isNull()) {
        rfFxdFreq = setting["rfFxdFreq"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["rfScanRange"].isNull()) {
        rfScanRange = setting["rfScanRange"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["rfidModule"].isNull()) {
        rfidModule = setting["rfidModule"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["iButton"].isNull()) {
        int val = setting["iButton"].as<int>();
        if (val < GPIO_NUM_MAX) iButton = val;
        else log_w("iButton pin not set");
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["mifareKeys"].isNull()) {
        mifareKeys.clear();
        JsonArray _mifareKeys = setting["mifareKeys"].as<JsonArray>();
        for (JsonVariant key : _mifareKeys) mifareKeys.insert(key.as<String>());
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["gpsBaudrate"].isNull()) {
        gpsBaudrate = setting["gpsBaudrate"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["startupApp"].isNull()) {
        startupApp = setting["startupApp"].as<String>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["wigleBasicToken"].isNull()) {
        wigleBasicToken = setting["wigleBasicToken"].as<String>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["devMode"].isNull()) {
        devMode = setting["devMode"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["colorInverted"].isNull()) {
        colorInverted = setting["colorInverted"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }

    if (!setting["disabledMenus"].isNull()) {
        disabledMenus.clear();
        JsonArray dm = setting["disabledMenus"].as<JsonArray>();
        for (JsonVariant e : dm) { disabledMenus.push_back(e.as<String>()); }
    } else {
        count++;
        log_e("Fail");
    }

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
    if (count > 0) saveFile();

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

    if (setupSdCard()) copyToFs(LittleFS, SD, filepath, false);
}

void BruceConfig::factoryReset() {
    FS *fs = &LittleFS;
    fs->rename(String(filepath), "/bak." + String(filepath).substring(1));
    if (setupSdCard()) SD.rename(String(filepath), "/bak." + String(filepath).substring(1));
    ESP.restart();
}

void BruceConfig::validateConfig() {
    validateUiColor();
    validateRotationValue();
    validateDimmerValue();
    validateBrightValue();
    validateTmzValue();
    validateSoundEnabledValue();
    validateWifiAtStartupValue();
    validateLedBrightValue();
    validateLedColorValue();
    validateRfScanRangeValue();
    validateRfModuleValue();
    validateRfidModuleValue();
    validateMifareKeysItems();
    validateGpsBaudrateValue();
    validateDevModeValue();
    validateColorInverted();
}

void BruceConfig::setUiColor(uint16_t primary, uint16_t *secondary, uint16_t *background) {
    BruceTheme::_setUiColor(primary, secondary, background);
    saveFile();
}

void BruceConfig::setRotation(int value) {
    rotation = value;
    validateRotationValue();
    saveFile();
}

void BruceConfig::validateRotationValue() {
    if (rotation < 0 || rotation > 3) rotation = 1;
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

void BruceConfig::setBright(uint8_t value) {
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

void BruceConfig::setLedBright(int value) {
    ledBright = value;
    validateLedBrightValue();
    saveFile();
}

void BruceConfig::validateLedBrightValue() { ledBright = max(0, min(100, ledBright)); }

void BruceConfig::setLedColor(uint32_t value) {
    ledColor = value;
    validateLedColorValue();
    saveFile();
}

void BruceConfig::validateLedColorValue() {
    ledColor = max<uint32_t>(0, min<uint32_t>(0xFFFFFFFF, ledColor));
}

void BruceConfig::setWebUICreds(const String &usr, const String &pwd) {
    webUI.user = usr;
    webUI.pwd = pwd;
    saveFile();
}

void BruceConfig::setWifiApCreds(const String &ssid, const String &pwd) {
    wifiAp.ssid = ssid;
    wifiAp.pwd = pwd;
    saveFile();
}

void BruceConfig::addWifiCredential(const String &ssid, const String &pwd) {
    wifi[ssid] = pwd;
    saveFile();
}

String BruceConfig::getWifiPassword(const String &ssid) const {
    auto it = wifi.find(ssid);
    if (it != wifi.end()) return it->second;
    return "";
}

void BruceConfig::addEvilWifiName(String value) {
    evilWifiNames.insert(value);
    saveFile();
}

void BruceConfig::removeEvilWifiName(String value) {
    evilWifiNames.erase(value);
    saveFile();
}

void BruceConfig::setBleName(String value) {
    bleName = value;
    saveFile();
}

void BruceConfig::setIrTxPin(int value) {
    irTx = value;
    saveFile();
}

void BruceConfig::setIrTxRepeats(uint8_t value) {
    irTxRepeats = value;
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
    if (rfModule != M5_RF_MODULE && rfModule != CC1101_SPI_MODULE) { rfModule = M5_RF_MODULE; }
}

void BruceConfig::setRfFreq(float value, int fxdFreq) {
    rfFreq = value;
    if (fxdFreq > 1) rfFxdFreq = fxdFreq;
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
    if (rfidModule != M5_RFID2_MODULE && rfidModule != PN532_I2C_MODULE && rfidModule != PN532_SPI_MODULE) {
        rfidModule = M5_RFID2_MODULE;
    }
}

void BruceConfig::setiButtonPin(int value) {
    if (value < GPIO_NUM_MAX) {
        iButton = value;
        saveFile();
    } else log_e("iButton: Gpio pin not set, incompatible with this device\n");
}

void BruceConfig::addMifareKey(String value) {
    if (value.length() != 12) return;
    mifareKeys.insert(value);
    validateMifareKeysItems();
    saveFile();
}

void BruceConfig::validateMifareKeysItems() {
    for (auto key = mifareKeys.begin(); key != mifareKeys.end();) {
        if (key->length() != 12) key = mifareKeys.erase(key);
        else ++key;
    }
}

void BruceConfig::setGpsBaudrate(int value) {
    gpsBaudrate = value;
    validateGpsBaudrateValue();
    saveFile();
}

void BruceConfig::validateGpsBaudrateValue() {
    if (gpsBaudrate != 9600 && gpsBaudrate != 19200 && gpsBaudrate != 57600 && gpsBaudrate != 38400 &&
        gpsBaudrate != 115200)
        gpsBaudrate = 9600;
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

void BruceConfig::setColorInverted(int value) {
    colorInverted = value;
    validateColorInverted();
    saveFile();
}

void BruceConfig::validateColorInverted() {
    if (colorInverted > 1) colorInverted = 1;
}

void BruceConfig::addDisabledMenu(String value) {
    // TODO: check if duplicate
    disabledMenus.push_back(value);
    saveFile();
}

void BruceConfig::addQrCodeEntry(const String &menuName, const String &content) {
    qrCodes.push_back({menuName, content});
    saveFile();
}

void BruceConfig::removeQrCodeEntry(const String &menuName) {
    qrCodes.erase(
        std::remove_if(
            qrCodes.begin(),
            qrCodes.end(),
            [&](const QrCodeEntry &entry) { return entry.menuName == menuName; }
        ),
        qrCodes.end()
    );
    saveFile();
}

void BruceConfig::setSpiPins(SPIPins value) {
    validateSpiPins(value);
    saveFile();
}
void BruceConfig::validateSpiPins(SPIPins value) {
    if (value.sck < 0 || value.sck > GPIO_PIN_COUNT) value.sck = GPIO_NUM_NC;
    if (value.miso < 0 || value.miso > GPIO_PIN_COUNT) value.miso = GPIO_NUM_NC;
    if (value.mosi < 0 || value.mosi > GPIO_PIN_COUNT) value.mosi = GPIO_NUM_NC;
    if (value.cs < 0 || value.cs > GPIO_PIN_COUNT) value.cs = GPIO_NUM_NC;
    if (value.io0 < 0 || value.io0 > GPIO_PIN_COUNT) value.io0 = GPIO_NUM_NC;
    if (value.io2 < 0 || value.io2 > GPIO_PIN_COUNT) value.io2 = GPIO_NUM_NC;
}
