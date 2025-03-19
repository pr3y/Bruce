#include "theme.h"
#include "display.h"

void BruceTheme::removeTheme(void) {
    themeInfo t;
    theme = t;
}
bool BruceTheme::openThemeFile(FS *fs, String filepath) {

    if (fs == nullptr) return true;
    if (!fs->exists(filepath)) return false;
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
        displayError("5", true);
        log_e("THEME: Failed reading theme file, using default theme");
        removeTheme();
        return false;
    }
    themePath = filepath;
    String baseThemePath = themePath.substring(0, themePath.lastIndexOf('/')) + "/";
    JsonObject _th = jsonDoc.as<JsonObject>();
    if (!_th["wifi"].isNull()) {
        if (fs->exists(baseThemePath + _th["wifi"].as<String>())) {
            theme.wifi = true;
            theme.paths.wifi = _th["wifi"].as<String>();
        } else log_w("THEME: file not found: wifi");
    }
    if (!_th["ble"].isNull()) {
        if (fs->exists(baseThemePath + _th["ble"].as<String>())) {
            theme.ble = true;
            theme.paths.ble = _th["ble"].as<String>();
        } else log_w("THEME: file not found: ble");
    }
    if (!_th["rf"].isNull()) {
        if (fs->exists(baseThemePath + _th["rf"].as<String>())) {
            theme.rf = true;
            theme.paths.rf = _th["rf"].as<String>();
        } else log_w("THEME: file not found: rf");
    }
    if (!_th["rfid"].isNull()) {
        if (fs->exists(baseThemePath + _th["rfid"].as<String>())) {
            theme.rfid = true;
            theme.paths.rfid = _th["rfid"].as<String>();
        } else log_w("THEME: file not found: rfid");
    }
    if (!_th["fm"].isNull()) {
        if (fs->exists(baseThemePath + _th["fm"].as<String>())) {
            theme.fm = true;
            theme.paths.fm = _th["fm"].as<String>();
        } else log_w("THEME: file not found: fm");
    }
    if (!_th["ir"].isNull()) {
        if (fs->exists(baseThemePath + _th["ir"].as<String>())) {
            theme.ir = true;
            theme.paths.ir = _th["ir"].as<String>();
        } else log_w("THEME: file not found: ir");
    }
    if (!_th["files"].isNull()) {
        if (fs->exists(baseThemePath + _th["files"].as<String>())) {
            theme.files = true;
            theme.paths.files = _th["files"].as<String>();
        } else log_w("THEME: file not found: files");
    }
    if (!_th["gps"].isNull()) {
        if (fs->exists(baseThemePath + _th["gps"].as<String>())) {
            theme.gps = true;
            theme.paths.gps = _th["gps"].as<String>();
        } else log_w("THEME: file not found: gps");
    }
    if (!_th["nrf"].isNull()) {
        if (fs->exists(baseThemePath + _th["nrf"].as<String>())) {
            theme.nrf = true;
            theme.paths.nrf = _th["nrf"].as<String>();
        } else log_w("THEME: file not found: nrf");
    }
    if (!_th["interpreter"].isNull()) {
        if (fs->exists(baseThemePath + _th["interpreter"].as<String>())) {
            theme.interpreter = true;
            theme.paths.interpreter = _th["interpreter"].as<String>();
        } else log_w("THEME: file not found: interpreter");
    }
    if (!_th["clock"].isNull()) {
        if (fs->exists(baseThemePath + _th["clock"].as<String>())) {
            theme.clock = true;
            theme.paths.clock = _th["clock"].as<String>();
        } else log_w("THEME: file not found: clock");
    }
    if (!_th["others"].isNull()) {
        if (fs->exists(baseThemePath + _th["others"].as<String>())) {
            theme.others = true;
            theme.paths.others = _th["others"].as<String>();
        } else log_w("THEME: file not found: others");
    }
    if (!_th["connect"].isNull()) {
        if (fs->exists(baseThemePath + _th["connect"].as<String>())) {
            theme.connect = true;
            theme.paths.connect = _th["connect"].as<String>();
        } else log_w("THEME: file not found: connect");
    }
    if (!_th["config"].isNull()) {
        if (fs->exists(baseThemePath + _th["config"].as<String>())) {
            theme.config = true;
            theme.paths.config = _th["config"].as<String>();
        } else log_w("THEME: file not found: config");
    }
    uint16_t _priColor = bruceConfig.priColor;
    uint16_t _secColor = bruceConfig.secColor;
    uint16_t _bgColor = bruceConfig.bgColor;

    if (!_th["priColor"].isNull()) {
        _priColor = strtoul(_th["priColor"], nullptr, 16);
    }
    if (!_th["secColor"].isNull()) {
        _secColor = strtoul(_th["secColor"], nullptr, 16);
    }
    if (!_th["bgColor"].isNull()) {
        _bgColor = strtoul(_th["bgColor"], nullptr, 16);
    }
    if (!_th["border"].isNull()) {
        theme.border = _th["border"].as<int>();
    }
    if (!_th["label"].isNull()) {
        theme.label = _th["label"].as<int>();
    }

    file.close();
    _setUiColor(_priColor, &_secColor, &_bgColor);

    if (fs == &LittleFS) theme.fs = 1;
    else if (fs == &SD) theme.fs = 2;
    else fs = 0;

    return true;
}

bool BruceTheme::validateImgFile(FS *fs, String filepath) {
    // Think of a way to check if the images are at maximum height of tftHeight
    // this size is the maximun value to be shown on screen without overlapping the status bar.
    return true;
}

void BruceTheme::_setUiColor(uint16_t primary, uint16_t *secondary, uint16_t *background) {
    priColor = primary;
    secColor = secondary == nullptr ? primary - 0x2000 : *secondary;
    bgColor = background == nullptr ? 0x0 : *background;
    validateUiColor();
}

// uint16_t can't be lower than 0 or greater than 0xFFFF, thats its limit
void BruceTheme::validateUiColor() {
    if (priColor < 0 || priColor > 0xFFFF) priColor = DEFAULT_PRICOLOR;
    if (secColor < 0 || secColor > 0xFFFF) secColor = priColor - 0x2000;
    if (bgColor < 0 || bgColor > 0xFFFF) bgColor = 0;
}
