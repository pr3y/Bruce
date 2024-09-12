#include "app_config.h"
#include "core/globals.h"
#include "core/sd_functions.h"
#include "core/settings.h"



String _getDefaultConfigJson() {
    int rot;
#if ROTATION > 1
    rot = 3;
#else
    rot = 1;
#endif

    return
        "[{"
        "\"rot\":"+String(rot)+","
        "\"dimmerSet\":10,"
        "\"bright\":100,"
        "\"wui_usr\":\"admin\","
        "\"wui_pwd\":\"bruce\","
        "\"Bruce_FGCOLOR\":43023,"
        "\"IrTx\":"+String(LED)+","
        "\"IrRx\":"+String(GROVE_SCL)+","
        "\"RfTx\":"+String(GROVE_SDA)+","
        "\"RfRx\":"+String(GROVE_SCL)+","
        "\"tmz\":3,"
        "\"RfModule\":0,"
        "\"RfFreq\":433.92,"
        "\"RfidModule\":0,"
        "\"wifi\":[{"
            "\"ssid\":\"myNetSSID\","
            "\"pwd\":\"myNetPassword\""
        "}],"
        "\"wigleBasicToken\":\"\","
        "\"devMode\":0,"
        "\"soundEnabled\":1"
        "}]";
}


void _createConfigFile (FS *fs) {
    File file;
    file = fs->open(CONFIG_FILE, FILE_WRITE);

    if(file) file.print(_getDefaultConfigJson());
    file.close();
    delay(50);
}


void AppConfig::_setConfigValues(JsonDocument setting) {
    int count=0;

    if(setting.containsKey("bright"))    { bright    = setting["bright"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("dimmerSet")) { dimmerSet = setting["dimmerSet"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("rot"))       { rotation  = setting["rot"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("Bruce_FGCOLOR"))   { FGCOLOR   = setting["Bruce_FGCOLOR"].as<uint16_t>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("wui_usr"))   { wui_usr   = setting["wui_usr"].as<String>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("wui_pwd"))   { wui_pwd   = setting["wui_pwd"].as<String>(); } else { count++; log_i("Fail"); }

    if(setting.containsKey("IrTx"))      { IrTx       = setting["IrTx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("IrRx"))      { IrRx       = setting["IrRx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("RfTx"))      { RfTx       = setting["RfTx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("RfRx"))      { RfRx       = setting["RfRx"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("tmz"))       { tmz        = setting["tmz"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("RfModule"))  { RfModule   = setting["RfModule"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("RfFreq"))    { RfFreq     = setting["RfFreq"].as<float>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("RfidModule")){ RfidModule = setting["RfidModule"].as<int>(); } else { count++; log_i("Fail"); }

    if(!setting.containsKey("wifi"))  { count++; log_i("Fail"); }

    if(setting.containsKey("wigleBasicToken"))  { wigleBasicToken  = setting["wigleBasicToken"].as<String>(); } else { count++; log_i("Fail"); }

    if(setting.containsKey("devMode"))  { devMode  = setting["devMode"].as<int>(); } else { count++; log_i("Fail"); }
    if(setting.containsKey("soundEnabled"))  { soundEnabled = setting["soundEnabled"].as<int>(); } else { count++; log_i("Fail"); }

    log_i("Brightness: %d", bright);
    setBrightness(bright);
    if(dimmerSet < 0) dimmerSet = 10;

    if(count > 0) saveConfigs();
}


void AppConfig::_syncEEPROMValues(void) {
    int count = 0;

    EEPROM.begin(EEPROMSIZE); // open eeprom

    if(EEPROM.read(EEPROM_ROT) != rotation) { EEPROM.write(EEPROM_ROT, rotation); count++; }
    if(EEPROM.read(EEPROM_DIMMER) != dimmerSet) { EEPROM.write(EEPROM_DIMMER, dimmerSet); count++; }
    if(EEPROM.read(EEPROM_BRIGHT) != bright) { EEPROM.write(EEPROM_BRIGHT, bright);  count++; }

    if(EEPROM.read(EEPROM_IR_TX) != IrTx) { EEPROM.write(EEPROM_IR_TX, IrTx); count++; }
    if(EEPROM.read(EEPROM_IR_RX) != IrRx) { EEPROM.write(EEPROM_IR_RX, IrRx); count++; }
    if(EEPROM.read(EEPROM_RF_TX) != RfTx) { EEPROM.write(EEPROM_RF_TX, RfTx); count++; }
    if(EEPROM.read(EEPROM_RF_RX) != RfRx) { EEPROM.write(EEPROM_RF_RX, RfRx); count++; }
    if(EEPROM.read(EEPROM_TMZ) != tmz) { EEPROM.write(EEPROM_TMZ, tmz); count++; }
    if(EEPROM.read(EEPROM_FGCOLOR0) !=(int((FGCOLOR >> 8) & 0x00FF))) {EEPROM.write(EEPROM_FGCOLOR0, int((FGCOLOR >> 8) & 0x00FF));  count++; }
    if(EEPROM.read(EEPROM_FGCOLOR1) != int(FGCOLOR & 0x00FF)) { EEPROM.write(EEPROM_FGCOLOR1, int(FGCOLOR & 0x00FF)); count++; }
    if(EEPROM.read(EEPROM_RF_MODULE) != RfModule) { EEPROM.write(EEPROM_RF_MODULE, RfModule); count++; }
    if(EEPROM.read(EEPROM_RFID_MODULE) != RfidModule) { EEPROM.write(EEPROM_RFID_MODULE, RfidModule); count++; }
    // TODO: add RfFreq

    //If something changed, saves the changes on EEPROM.
    if(count > 0) {
        if(!EEPROM.commit()) log_i("fail to write EEPROM");      // Store data to EEPROM
        else log_i("Wrote new conf to EEPROM");
    }

    EEPROM.end();
}


/*********************************************************************
**  Function: getConfigs
**  get configs from JSON bruce.conf file
**********************************************************************/
void AppConfig::getConfigs() {
    FS *fs;
    if(!getFsStorage(fs)) return;

    if(!fs->exists(CONFIG_FILE)) _createConfigFile(fs);
    else log_i("getConfigs: config.conf exists");

    bool EEPROMSave = false;

    File file = fs->open(CONFIG_FILE);
    if(file) {
        // Deserialize the JSON document
        DeserializationError error;
        JsonObject setting;
        error = deserializeJson(settings, file);
        file.close();

        if (error) {
            log_i("Failed to read file, using default configuration");
            Serial.println("End of Config");
            return;
        }

        log_i("getConfigs: deserialized correctly");

        setting = settings[0];
        _setConfigValues(setting);
        _syncEEPROMValues();

        log_i("Using config.conf setup file");
    } else {
        log_i("Using settings stored on EEPROM");
    }

    Serial.println("End of Config");
}


/*********************************************************************
**  Function: saveConfigs
**  save configs into JSON bruce.conf file
**********************************************************************/
void AppConfig::saveConfigs() {
    // Delete existing file, otherwise the configuration is appended to the file
    FS *fs;
    if(!getFsStorage(fs)) return;

    JsonObject setting = settings[0];
    setting["bright"] = bright;
    setting["dimmerSet"] = dimmerSet;
    setting["rot"] = rotation;
    setting["Bruce_FGCOLOR"] = FGCOLOR;
    setting["wui_usr"] = wui_usr;
    setting["wui_pwd"] = wui_pwd;
    setting["IrTx"] = IrTx;
    setting["IrRx"] = IrRx;
    setting["RfTx"] = RfTx;
    setting["RfRx"] = RfRx;
    setting["RfModule"] = RfModule;
    setting["RfFreq"] = RfFreq;
    setting["RfidModule"] = RfidModule;
    setting["tmz"] = tmz;
    if(!setting.containsKey("wifi")) {
        JsonArray WifiList = setting["wifi"].to<JsonArray>();
        if(WifiList.size() <= 0) {
            JsonObject WifiObj = WifiList.add<JsonObject>();
            WifiObj["ssid"] = "myNetSSID";
            WifiObj["pwd"] = "myNetPassword";
        }
    }
    setting["wigleBasicToken"] = wigleBasicToken;
    setting["devMode"] = devMode;
    setting["soundEnabled"] = soundEnabled;

    // Open file for writing
    File file = fs->open(CONFIG_FILE, FILE_WRITE);
    if (!file) {
        log_i("Failed to create file");
        file.close();
        return;
    }
    log_i("config.conf created");

    // Serialize JSON to file
    serializeJsonPretty(settings,Serial);
    if (serializeJsonPretty(settings, file) < 5) {
        log_i("Failed to write to file");
    } else log_i("config.conf written successfully");

    // Close the file
    file.close();
}


/*********************************************************************
**  Function: loadEEPROM
**  Load EEPROM data
*********************************************************************/
void AppConfig::loadEEPROM() {
    EEPROM.begin(EEPROMSIZE); // open eeprom

    rotation = EEPROM.read(EEPROM_ROT);
    dimmerSet = EEPROM.read(EEPROM_DIMMER);
    bright = EEPROM.read(EEPROM_BRIGHT);
    IrTx = EEPROM.read(EEPROM_IR_TX);
    IrRx = EEPROM.read(EEPROM_IR_RX);
    RfTx = EEPROM.read(EEPROM_RF_TX);
    RfRx = EEPROM.read(EEPROM_RF_RX);
    tmz = EEPROM.read(EEPROM_TMZ);
    FGCOLOR = EEPROM.read(EEPROM_FGCOLOR0) << 8 | EEPROM.read(EEPROM_FGCOLOR1);
    RfModule = EEPROM.read(EEPROM_RF_MODULE);
    RfidModule = EEPROM.read(EEPROM_RFID_MODULE);

    log_i("\
    \n*-*EEPROM Settings*-* \
    \n- rotation  =%03d, \
    \n- dimmerSet =%03d, \
    \n- Brightness=%03d, \
    \n- IR Tx Pin =%03d, \
    \n- IR Rx Pin =%03d, \
    \n- RF Tx Pin =%03d, \
    \n- RF Rx Pin =%03d, \
    \n- Time Zone =%03d, \
    \n- FGColor   =0x%04X \
    \n- RfModule  =%03d, \
    \n- RfidModule=%03d, \
    \n*-*-*-*-*-*-*-*-*-*-*",
    rotation, dimmerSet, bright,IrTx, IrRx, RfTx, RfRx, tmz, FGCOLOR, RfModule, RfidModule
    );

    if (
        rotation > 3
        || dimmerSet > 60
        || bright > 100
        || IrTx > 100
        || IrRx > 100
        || RfRx > 100
        || RfTx > 100
        || tmz > 24
    ) {
        rotation = ROTATION;
        dimmerSet = 10;
        bright = 100;
        IrTx = LED;
        IrRx = GROVE_SCL;
        RfTx = GROVE_SDA;
        RfRx = GROVE_SCL;
        FGCOLOR = 0xA80F;
        tmz = 0;
        RfModule = M5_RF_MODULE;
        RfidModule = M5_RFID2_MODULE;

        EEPROM.write(EEPROM_ROT, rotation);
        EEPROM.write(EEPROM_DIMMER, dimmerSet);
        EEPROM.write(EEPROM_BRIGHT, bright);
        EEPROM.write(EEPROM_IR_TX, IrTx);
        EEPROM.write(EEPROM_IR_RX, IrRx);
        EEPROM.write(EEPROM_RF_TX, RfTx);
        EEPROM.write(EEPROM_RF_RX, RfRx);
        EEPROM.write(EEPROM_TMZ, tmz);
        EEPROM.write(EEPROM_FGCOLOR0, int((FGCOLOR >> 8) & 0x00FF));
        EEPROM.write(EEPROM_FGCOLOR1, int(FGCOLOR & 0x00FF));
        EEPROM.write(EEPROM_RF_MODULE, RfModule);
        EEPROM.write(EEPROM_RFID_MODULE, RfidModule);
        EEPROM.writeString(20,"");

        EEPROM.commit();      // Store data to EEPROM
        EEPROM.end();
        log_w("One of the eeprom values is invalid");
    }

    setBrightness(bright, false);
    EEPROM.end();
}
