#include "config.h"
#include "sd_functions.h"

/**
 * @brief Converts the configuration to a JSON document.
 *
 * @return JsonDocument The JSON document.
 */
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
    setting["soundVolume"] = soundVolume;
    setting["wifiAtStartup"] = wifiAtStartup;
    setting["instantBoot"] = instantBoot;

    setting["ledBright"] = ledBright;
    setting["ledColor"] = String(ledColor, HEX);
    setting["ledBlinkEnabled"] = ledBlinkEnabled;
    setting["ledEffect"] = ledEffect;
    setting["ledEffectSpeed"] = ledEffectSpeed;
    setting["ledEffectDirection"] = ledEffectDirection;

    JsonObject _webUI = setting["webUI"].to<JsonObject>();
    _webUI["user"] = webUI.user;
    _webUI["pwd"] = webUI.pwd;

    JsonObject _wifiAp = setting["wifiAp"].to<JsonObject>();
    _wifiAp["ssid"] = wifiAp.ssid;
    _wifiAp["pwd"] = wifiAp.pwd;
    setting["wifiMAC"] = wifiMAC; //@IncursioHack

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

    return jsonDoc;
}

/**
 * @brief Loads the configuration from a file.
 *
 * @param checkFS Whether to check the file system.
 */
void BruceConfig::fromFile(bool checkFS) {
    FS *fs;
    if (checkFS) {
        if (!getFsStorage(fs)) {
            log_i("Fail getting filesystem for config");
            return;
        }
    } else {
        if (checkLittleFsSize()) fs = &LittleFS;
        else return;
    }

    if (!fs->exists(filepath)) {
        log_i("Config file not found. Creating default config");
        return saveFile();
    }

    File file;
    file = fs->open(filepath, FILE_READ);
    if (!file) {
        log_i("Config file not found. Using default values");
        return;
    }

    // Deserialize the JSON document
    JsonDocument jsonDoc;
    if (deserializeJson(jsonDoc, file)) {
        Serial.println("Failed to read config file, using default configuration");
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
    if (!setting["soundVolume"].isNull()) {
        soundVolume = setting["soundVolume"].as<int>();
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
    if (!setting["ledBlinkEnabled"].isNull()) {
        ledBlinkEnabled = setting["ledBlinkEnabled"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["ledEffect"].isNull()) {
        ledEffect = setting["ledEffect"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["ledEffectSpeed"].isNull()) {
        ledEffectSpeed = setting["ledEffectSpeed"].as<int>();
    } else {
        count++;
        log_e("Fail");
    }
    if (!setting["ledEffectDirection"].isNull()) {
        ledEffectDirection = setting["ledEffectDirection"].as<int>();
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

    //@IncursioHack
    if (!setting["wifiMAC"].isNull()) {
    wifiMAC = setting["wifiMAC"].as<String>();
    } else {
        wifiMAC = "";
        count++;
        log_e("wifiMAC not found, using default");
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

/**
 * @brief Saves the configuration to a file.
 */
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

/**
 * @brief Resets the configuration to the factory settings.
 */
void BruceConfig::factoryReset() {
    FS *fs = &LittleFS;
    fs->rename(String(filepath), "/bak." + String(filepath).substring(1));
    if (setupSdCard()) SD.rename(String(filepath), "/bak." + String(filepath).substring(1));
    ESP.restart();
}

/**
 * @brief Validates the configuration.
 */
void BruceConfig::validateConfig() {
    validateRotationValue();
    validateDimmerValue();
    validateBrightValue();
    validateTmzValue();
    validateSoundEnabledValue();
    validateSoundVolumeValue();
    validateWifiAtStartupValue();
    validateLedBrightValue();
    validateLedColorValue();
    validateLedBlinkEnabledValue();
    validateLedEffectValue();
    validateLedEffectSpeedValue();
    validateLedEffectDirectionValue();
    validateRfScanRangeValue();
    validateRfModuleValue();
    validateRfidModuleValue();
    validateMifareKeysItems();
    validateGpsBaudrateValue();
    validateDevModeValue();
    validateColorInverted();
}

/**
 * @brief Sets the UI color.
 *
 * @param primary The primary color.
 * @param secondary The secondary color.
 * @param background The background color.
 */
void BruceConfig::setUiColor(uint16_t primary, uint16_t *secondary, uint16_t *background) {
    BruceTheme::_setUiColor(primary, secondary, background);
    saveFile();
}

/**
 * @brief Sets the rotation.
 *
 * @param value The rotation value.
 */
void BruceConfig::setRotation(int value) {
    rotation = value;
    validateRotationValue();
    saveFile();
}

/**
 * @brief Validates the rotation value.
 */
void BruceConfig::validateRotationValue() {
    if (rotation < 0 || rotation > 3) rotation = 1;
}

/**
 * @brief Sets the dimmer.
 *
 * @param value The dimmer value.
 */
void BruceConfig::setDimmer(int value) {
    dimmerSet = value;
    validateDimmerValue();
    saveFile();
}

/**
 * @brief Validates the dimmer value.
 */
void BruceConfig::validateDimmerValue() {
    if (dimmerSet < 0) dimmerSet = 10;
    if (dimmerSet > 60) dimmerSet = 0;
}

/**
 * @brief Sets the brightness.
 *
 * @param value The brightness value.
 */
void BruceConfig::setBright(uint8_t value) {
    bright = value;
    validateBrightValue();
    saveFile();
}

/**
 * @brief Validates the brightness value.
 */
void BruceConfig::validateBrightValue() {
    if (bright > 100) bright = 100;
}

/**
 * @brief Sets the timezone.
 *
 * @param value The timezone value.
 */
void BruceConfig::setTmz(int value) {
    tmz = value;
    validateTmzValue();
    saveFile();
}

/**
 * @brief Validates the timezone value.
 */
void BruceConfig::validateTmzValue() {
    if (tmz < -12 || tmz > 12) tmz = 0;
}

/**
 * @brief Sets whether the sound is enabled.
 *
 * @param value Whether the sound is enabled.
 */
void BruceConfig::setSoundEnabled(int value) {
    soundEnabled = value;
    validateSoundEnabledValue();
    saveFile();
}

/**
 * @brief Sets the sound volume.
 *
 * @param value The sound volume.
 */
void BruceConfig::setSoundVolume(int value) {
    soundVolume = value;
    validateSoundVolumeValue();
    saveFile();
}

/**
 * @brief Validates the sound enabled value.
 */
void BruceConfig::validateSoundEnabledValue() {
    if (soundEnabled > 1) soundEnabled = 1;
}

/**
 * @brief Validates the sound volume value.
 */
void BruceConfig::validateSoundVolumeValue() {
    if (soundVolume > 100) soundVolume = 100;
}

/**
 * @brief Sets whether WiFi is enabled at startup.
 *
 * @param value Whether WiFi is enabled at startup.
 */
void BruceConfig::setWifiAtStartup(int value) {
    wifiAtStartup = value;
    validateWifiAtStartupValue();
    saveFile();
}

/**
 * @brief Validates the WiFi at startup value.
 */
void BruceConfig::validateWifiAtStartupValue() {
    if (wifiAtStartup > 1) wifiAtStartup = 1;
}

/**
 * @brief Sets the LED brightness.
 *
 * @param value The LED brightness.
 */
void BruceConfig::setLedBright(int value) {
    ledBright = value;
    validateLedBrightValue();
    saveFile();
}

/**
 * @brief Validates the LED brightness value.
 */
void BruceConfig::validateLedBrightValue() { ledBright = max(0, min(100, ledBright)); }

/**
 * @brief Sets the LED color.
 *
 * @param value The LED color.
 */
void BruceConfig::setLedColor(uint32_t value) {
    ledColor = value;
    validateLedColorValue();
    saveFile();
}

/**
 * @brief Validates the LED color value.
 */
void BruceConfig::validateLedColorValue() {
    ledColor = max<uint32_t>(0, min<uint32_t>(0xFFFFFFFF, ledColor));
}

/**
 * @brief Sets whether the LED blink is enabled.
 *
 * @param value Whether the LED blink is enabled.
 */
void BruceConfig::setLedBlinkEnabled(int value) {
    ledBlinkEnabled = value;
    validateLedBlinkEnabledValue();
    saveFile();
}

/**
 * @brief Validates the LED blink enabled value.
 */
void BruceConfig::validateLedBlinkEnabledValue() {
    if (ledBlinkEnabled > 1) ledBlinkEnabled = 1;
}

/**
 * @brief Sets the LED effect.
 *
 * @param value The LED effect.
 */
void BruceConfig::setLedEffect(int value) {
    ledEffect = value;
    validateLedEffectValue();
    saveFile();
}

/**
 * @brief Validates the LED effect value.
 */
void BruceConfig::validateLedEffectValue() {
    if (ledEffect < 0 || ledEffect > 5) ledEffect = 0;
}

/**
 * @brief Sets the LED effect speed.
 *
 * @param value The LED effect speed.
 */
void BruceConfig::setLedEffectSpeed(int value) {
    ledEffectSpeed = value;
    validateLedEffectSpeedValue();
    saveFile();
}

/**
 * @brief Validates the LED effect speed value.
 */
void BruceConfig::validateLedEffectSpeedValue() {
#ifdef HAS_ENCODER_LED
    if (ledEffectSpeed > 11) ledEffectSpeed = 11;
#else
    if (ledEffectSpeed > 10) ledEffectSpeed = 10;
#endif
    if (ledEffectSpeed < 0) ledEffectSpeed = 1;
}

/**
 * @brief Sets the LED effect direction.
 *
 * @param value The LED effect direction.
 */
void BruceConfig::setLedEffectDirection(int value) {
    ledEffectDirection = value;
    validateLedEffectDirectionValue();
    saveFile();
}

/**
 * @brief Validates the LED effect direction value.
 */
void BruceConfig::validateLedEffectDirectionValue() {
    if (ledEffectDirection > 1 || ledEffectDirection == 0) ledEffectDirection = 1;
    if (ledEffectDirection < -1) ledEffectDirection = -1;
}

/**
 * @brief Sets the web UI credentials.
 *
 * @param usr The username.
 * @param pwd The password.
 */
void BruceConfig::setWebUICreds(const String &usr, const String &pwd) {
    webUI.user = usr;
    webUI.pwd = pwd;
    saveFile();
}

/**
 * @brief Sets the WiFi AP credentials.
 *
 * @param ssid The SSID.
 * @param pwd The password.
 */
void BruceConfig::setWifiApCreds(const String &ssid, const String &pwd) {
    wifiAp.ssid = ssid;
    wifiAp.pwd = pwd;
    saveFile();
}

/**
 * @brief Adds a WiFi credential.
 *
 * @param ssid The SSID.
 * @param pwd The password.
 */
void BruceConfig::addWifiCredential(const String &ssid, const String &pwd) {
    wifi[ssid] = pwd;
    saveFile();
}

/**
 * @brief Gets the WiFi password for a given SSID.
 *
 * @param ssid The SSID.
 * @return String The password.
 */
String BruceConfig::getWifiPassword(const String &ssid) const {
    auto it = wifi.find(ssid);
    if (it != wifi.end()) return it->second;
    return "";
}

/**
 * @brief Adds an evil WiFi name.
 *
 * @param value The evil WiFi name.
 */
void BruceConfig::addEvilWifiName(String value) {
    evilWifiNames.insert(value);
    saveFile();
}

/**
 * @brief Removes an evil WiFi name.
 *
 * @param value The evil WiFi name.
 */
void BruceConfig::removeEvilWifiName(String value) {
    evilWifiNames.erase(value);
    saveFile();
}

/**
 * @brief Sets the BLE name.
 *
 * @param name The BLE name.
 */
void BruceConfig::setBleName(String value) {
    bleName = value;
    saveFile();
}

/**
 * @brief Sets the IR TX pin.
 *
 * @param value The IR TX pin.
 */
void BruceConfig::setIrTxPin(int value) {
    irTx = value;
    saveFile();
}

/**
 * @brief Sets the IR TX repeats.
 *
 * @param value The IR TX repeats.
 */
void BruceConfig::setIrTxRepeats(uint8_t value) {
    irTxRepeats = value;
    saveFile();
}

/**
 * @brief Sets the IR RX pin.
 *
 * @param value The IR RX pin.
 */
void BruceConfig::setIrRxPin(int value) {
    irRx = value;
    saveFile();
}

/**
 * @brief Sets the RF TX pin.
 *
 * @param value The RF TX pin.
 */
void BruceConfig::setRfTxPin(int value) {
    rfTx = value;
    saveFile();
}

/**
 * @brief Sets the RF RX pin.
 *
 * @param value The RF RX pin.
 */
void BruceConfig::setRfRxPin(int value) {
    rfRx = value;
    saveFile();
}

/**
 * @brief Sets the RF module.
 *
 * @param value The RF module.
 */
void BruceConfig::setRfModule(RFModules value) {
    rfModule = value;
    validateRfModuleValue();
    saveFile();
}

/**
 * @brief Validates the RF module value.
 */
void BruceConfig::validateRfModuleValue() {
    if (rfModule != M5_RF_MODULE && rfModule != CC1101_SPI_MODULE) { rfModule = M5_RF_MODULE; }
}

/**
 * @brief Sets the RF frequency.
 *
 * @param value The RF frequency.
 * @param fxdFreq The fixed frequency.
 */
void BruceConfig::setRfFreq(float value, int fxdFreq) {
    rfFreq = value;
    if (fxdFreq > 1) rfFxdFreq = fxdFreq;
    saveFile();
}

/**
 * @brief Sets the RF fixed frequency.
 *
 * @param value The RF fixed frequency.
 */
void BruceConfig::setRfFxdFreq(float value) {
    rfFxdFreq = value;
    saveFile();
}

/**
 * @brief Sets the RF scan range.
 *
 * @param value The RF scan range.
 * @param fxdFreq The fixed frequency.
 */
void BruceConfig::setRfScanRange(int value, int fxdFreq) {
    rfScanRange = value;
    rfFxdFreq = fxdFreq;
    validateRfScanRangeValue();
    saveFile();
}

/**
 * @brief Validates the RF scan range value.
 */
void BruceConfig::validateRfScanRangeValue() {
    if (rfScanRange < 0 || rfScanRange > 3) rfScanRange = 3;
}

/**
 * @brief Sets the RFID module.
 *
 * @param value The RFID module.
 */
void BruceConfig::setRfidModule(RFIDModules value) {
    rfidModule = value;
    validateRfidModuleValue();
    saveFile();
}

/**
 * @brief Validates the RFID module value.
 */
void BruceConfig::validateRfidModuleValue() {
    if (rfidModule != M5_RFID2_MODULE && rfidModule != PN532_I2C_MODULE && rfidModule != PN532_SPI_MODULE &&
        rfidModule != RC522_SPI_MODULE && rfidModule != PN532_I2C_SPI_MODULE) {
        rfidModule = M5_RFID2_MODULE;
    }
}

/**
 * @brief Sets the iButton pin.
 *
 * @param value The iButton pin.
 */
void BruceConfig::setiButtonPin(int value) {
    if (value < GPIO_NUM_MAX) {
        iButton = value;
        saveFile();
    } else log_e("iButton: Gpio pin not set, incompatible with this device\n");
}

/**
 * @brief Adds a Mifare key.
 *
 * @param value The Mifare key.
 */
void BruceConfig::addMifareKey(String value) {
    if (value.length() != 12) return;
    mifareKeys.insert(value);
    validateMifareKeysItems();
    saveFile();
}

/**
 * @brief Validates the Mifare keys.
 */
void BruceConfig::validateMifareKeysItems() {
    for (auto key = mifareKeys.begin(); key != mifareKeys.end();) {
        if (key->length() != 12) key = mifareKeys.erase(key);
        else ++key;
    }
}

/**
 * @brief Sets the GPS baudrate.
 *
 * @param value The GPS baudrate.
 */
void BruceConfig::setGpsBaudrate(int value) {
    gpsBaudrate = value;
    validateGpsBaudrateValue();
    saveFile();
}

/**
 * @brief Validates the GPS baudrate value.
 */
void BruceConfig::validateGpsBaudrateValue() {
    if (gpsBaudrate != 9600 && gpsBaudrate != 19200 && gpsBaudrate != 57600 && gpsBaudrate != 38400 &&
        gpsBaudrate != 115200)
        gpsBaudrate = 9600;
}

/**
 * @brief Sets the startup app.
 *
 * @param value The startup app.
 */
void BruceConfig::setStartupApp(String value) {
    startupApp = value;
    saveFile();
}

/**
 * @brief Sets the Wigle basic token.
 *
 * @param value The Wigle basic token.
 */
void BruceConfig::setWigleBasicToken(String value) {
    wigleBasicToken = value;
    saveFile();
}

/**
 * @brief Sets whether dev mode is enabled.
 *
 * @param value Whether dev mode is enabled.
 */
void BruceConfig::setDevMode(int value) {
    devMode = value;
    validateDevModeValue();
    saveFile();
}

/**
 * @brief Validates the dev mode value.
 */
void BruceConfig::validateDevModeValue() {
    if (devMode > 1) devMode = 1;
}

/**
 * @brief Sets whether the color is inverted.
 *
 * @param value Whether the color is inverted.
 */
void BruceConfig::setColorInverted(int value) {
    colorInverted = value;
    validateColorInverted();
    saveFile();
}

/**
 * @brief Validates the color inverted value.
 */
void BruceConfig::validateColorInverted() {
    if (colorInverted > 1) colorInverted = 1;
}

/**
 * @brief Adds a disabled menu.
 *
 * @param value The disabled menu.
 */
void BruceConfig::addDisabledMenu(String value) {
    // TODO: check if duplicate
    disabledMenus.push_back(value);
    saveFile();
}

/**
 * @brief Adds a QR code entry.
 *
 * @param menuName The menu name.
 * @param content The content.
 */
void BruceConfig::addQrCodeEntry(const String &menuName, const String &content) {
    qrCodes.push_back({menuName, content});
    saveFile();
}

/**
 * @brief Removes a QR code entry.
 *
 * @param menuName The menu name.
 */
void BruceConfig::removeQrCodeEntry(const String &menuName) {
    size_t writeIndex = 0;

    for (size_t readIndex = 0; readIndex < qrCodes.size(); ++readIndex) {
        const QrCodeEntry &entry = qrCodes[readIndex];

        if (entry.menuName != menuName) {
            if (writeIndex != readIndex) { qrCodes[writeIndex] = std::move(qrCodes[readIndex]); }
            ++writeIndex;
        }
    }

    if (writeIndex < qrCodes.size()) { qrCodes.erase(qrCodes.begin() + writeIndex, qrCodes.end()); }

    saveFile();
}
