#include "theme.h"
#include "display.h"

struct ThemeEntry {
    const char *key;
    bool *flag;
    String &path;
};

void BruceTheme::removeTheme(void) {
    themeInfo t;
    theme = t;
}
FS* BruceTheme::themeFS(void){
    if (theme.fs == 1) return &LittleFS;
    else if (theme.fs == 2) return &SD;
    return &LittleFS; // always get back to safety
}
bool BruceTheme::openThemeFile(FS *fs, String filepath) {

    if (fs == nullptr) return true;
    if (!fs->exists(filepath)) return false;
    File file;
    file = fs->open(filepath, FILE_READ);
    if (!file) {
        log_e("THEME: %s. Using default theme", "Theme file not found");
        removeTheme();
        return false;
    }

    // Deserialize the JSON document
    JsonDocument jsonDoc;
    if (deserializeJson(jsonDoc, file)) {
        displayError("5", true);
        log_e("THEME: %s. Using default theme", "Failed reading theme file");
        removeTheme();
        return false;
    }
    themePath = filepath;
    String baseThemePath = themePath.substring(0, themePath.lastIndexOf('/')) + "/";

    ThemeEntry entries[] = {
        {"wifi", &theme.wifi, theme.paths.wifi},
        {"ble", &theme.ble, theme.paths.ble},
        {"rf", &theme.rf, theme.paths.rf},
        {"rfid", &theme.rfid, theme.paths.rfid},
        {"fm", &theme.fm, theme.paths.fm},
        {"ir", &theme.ir, theme.paths.ir},
        {"files", &theme.files, theme.paths.files},
        {"gps", &theme.gps, theme.paths.gps},
        {"nrf", &theme.nrf, theme.paths.nrf},
        {"interpreter", &theme.interpreter, theme.paths.interpreter},
        {"clock", &theme.clock, theme.paths.clock},
        {"others", &theme.others, theme.paths.others},
        {"connect", &theme.connect, theme.paths.connect},
        {"config", &theme.config, theme.paths.config}
    };

    JsonObject _th = jsonDoc.as<JsonObject>();
    for (auto &entry : entries) {
        if (!_th[entry.key].isNull()) {
            String path = baseThemePath + _th[entry.key].as<String>();
            if (fs->exists(path)) {
                *entry.flag = true;
                entry.path = _th[entry.key].as<String>();
            } else {
                log_w("THEME: file not found: %s", entry.key);
            }
        }
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
    else theme.fs = 0;

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
