#include "theme.h"
#include "display.h"

void BruceTheme::removeTheme(void) {
    theme.fs         =0;
    theme.wifi       =false;
    theme.ble        =false;
    theme.rf         =false;
    theme.rfid       =false;
    theme.fm         =false;
    theme.ir         =false;
    theme.files      =false;
    theme.gps        =false;
    theme.nrf        =false;
    theme.interpreter=false;
    theme.others     =false;
    theme.clock      =false;
    theme.connect    =false;
    theme.config     =false;
    theme.paths.clear();
}
bool BruceTheme::openThemeFile(FS* fs, String filepath) {

    if(fs == nullptr) return true;
    if(!fs->exists(filepath)) return false;
    File file;
    file = fs->open(filepath, FILE_READ);
    if (!file) {
        log_e("THEME: Theme file not found. Using default theme");
        removeTheme();
        return false;
    }

    // Deserialize the JSON document
    JsonDocument jsonDoc;
    if (deserializeJson(jsonDoc, file)) {
        displayError("5",true);
        log_e("THEME: Failed reading theme file, using default theme");
        removeTheme();
        return false;
    }
    themePath = filepath;
    theme.paths.clear();
    theme.paths = jsonDoc.as<JsonObject>();
    if(!validateTheme(theme.paths)) {
        removeTheme();
        return false;
    }
    file.close();
    return applyTheme(fs,theme.paths);

}
bool BruceTheme::validateTheme(JsonObject& _th) {
    if(_th["wifi"].isNull())        { log_e("THEME: fail to retrieve Wifi");       return false; }
    if(_th["ble"].isNull())         { log_e("THEME: fail to retrieve ble");        return false; }
    if(_th["rf"].isNull())          { log_e("THEME: fail to retrieve rf");         return false; }
    if(_th["rfid"].isNull())        { log_e("THEME: fail to retrieve rfid");       return false; }
    if(_th["fm"].isNull())          { log_e("THEME: fail to retrieve fm");         return false; }
    if(_th["ir"].isNull())          { log_e("THEME: fail to retrieve ir");         return false; }
    if(_th["files"].isNull())       { log_e("THEME: fail to retrieve files");      return false; }
    if(_th["gps"].isNull())         { log_e("THEME: fail to retrieve gps");        return false; }
    if(_th["nrf"].isNull())         { log_e("THEME: fail to retrieve nrf");        return false; }
    if(_th["interpreter"].isNull()) { log_e("THEME: fail to retrieve interpreter");return false; }
    if(_th["clock"].isNull())       { log_e("THEME: fail to retrieve clock");      return false; }
    if(_th["others"].isNull())      { log_e("THEME: fail to retrieve others");     return false; }
    if(_th["connect"].isNull())     { log_e("THEME: fail to retrieve connect");    return false; }
    if(_th["config"].isNull())      { log_e("THEME: fail to retrieve config");     return false; }
    if(_th["priColor"].isNull())    { log_e("THEME: fail to retrieve priColor");   return false; }
    if(_th["secColor"].isNull())    { log_e("THEME: fail to retrieve secColor");   return false; }
    if(_th["bgColor"].isNull())     { log_e("THEME: fail to retrieve bgColor");    return false; }
    if(_th["border"].isNull())      { log_e("THEME: fail to retrieve border");     return false; }
    return true;
}
bool BruceTheme::applyTheme(FS* fs, JsonObject& _th) {
    String baseThemePath = themePath.substring(0,themePath.lastIndexOf('/'))+ "/";
    if(fs->exists(baseThemePath + _th["wifi"].as<String>()))     theme.wifi = true;          else log_w("THEME: file not found: wifi");
    if(fs->exists(baseThemePath + _th["ble"].as<String>()))      theme.ble = true;           else log_w("THEME: file not found: ble");
    if(fs->exists(baseThemePath + _th["rf"].as<String>()))       theme.rf = true;            else log_w("THEME: file not found: rf");
    if(fs->exists(baseThemePath + _th["rfid"].as<String>()))     theme.rfid = true;          else log_w("THEME: file not found: rfid");
    if(fs->exists(baseThemePath + _th["fm"].as<String>()))       theme.fm = true;            else log_w("THEME: file not found: fm");
    if(fs->exists(baseThemePath + _th["ir"].as<String>()))       theme.ir = true;            else log_w("THEME: file not found: ir");
    if(fs->exists(baseThemePath + _th["files"].as<String>()))    theme.files = true;         else log_w("THEME: file not found: files");
    if(fs->exists(baseThemePath + _th["gps"].as<String>()))      theme.gps = true;           else log_w("THEME: file not found: gps");
    if(fs->exists(baseThemePath + _th["nrf"].as<String>()))      theme.nrf = true;           else log_w("THEME: file not found: nrf");
    if(fs->exists(baseThemePath + _th["interpreter"].as<String>())) theme.interpreter = true;else log_w("THEME: file not found: interpreter");
    if(fs->exists(baseThemePath + _th["clock"].as<String>()))    theme.clock = true;         else log_w("THEME: file not found: clock");
    if(fs->exists(baseThemePath + _th["others"].as<String>()))   theme.others = true;        else log_w("THEME: file not found: others");
    if(fs->exists(baseThemePath + _th["connect"].as<String>()))  theme.connect = true;       else log_w("THEME: file not found: connect");
    if(fs->exists(baseThemePath + _th["config"].as<String>()))   theme.config = true;        else log_w("THEME: file not found: config");

    uint16_t _priColor = strtoul(_th["priColor"], nullptr, 16);
    uint16_t _secColor = strtoul(_th["secColor"], nullptr, 16);
    uint16_t _bgColor  = strtoul(_th["bgColor"], nullptr, 16);
    _setUiColor(_priColor, &_secColor, &_bgColor);
    
    theme.border         = _th["border"].as<int>();
    if(fs==&LittleFS) theme.fs = 1;
    else if(fs==&SD)  theme.fs = 2;
    else fs=0;
    return true;
}

void BruceTheme::_setUiColor(uint16_t primary, uint16_t* secondary, uint16_t* background) {
    priColor = primary;
    secColor = secondary == nullptr ? primary - 0x2000 : *secondary;
    bgColor = background == nullptr ? 0x0 : *background;
    validateUiColor();
}

// uint16_t can't be lower than 0 or greater than 0xFFFF, thats its limit
void BruceTheme::validateUiColor() {
    if (priColor < 0 || priColor > 0xFFFF) priColor = DEFAULT_PRICOLOR;
    if (secColor < 0 || secColor > 0xFFFF) secColor = priColor - 0x2000;
    if (bgColor  < 0 || bgColor  > 0xFFFF) bgColor  = 0;
}
