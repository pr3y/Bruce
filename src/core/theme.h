#ifndef __THEME_H
#define __THEME_H
#include <ArduinoJson.h>
#include "sd_functions.h"


#define DEFAULT_PRICOLOR 0xA80F

struct themeInfo {
    // Default storage (speed up loading)
    uint8_t fs      =0;
    // control booleans to not try to read unnecessary files, defined on validateTheme
    bool border     =true;
    bool wifi       =false;
    bool ble        =false;
    bool rf         =false;
    bool rfid       =false;
    bool fm         =false;
    bool ir         =false;
    bool files      =false;
    bool gps        =false;
    bool nrf        =false;
    bool interpreter=false;
    bool others     =false;
    bool clock      =false;
    bool connect    =false;
    bool config     =false;

    // Theme file paths, colors and border
    JsonObject paths;
};

class BruceTheme {
    public:
    themeInfo theme;
    String themePath = "";

    // Theme colors in RGB565 format
    int32_t priColor = DEFAULT_PRICOLOR;
    int32_t secColor = DEFAULT_PRICOLOR-0x2000;
    int32_t bgColor  = 0x0000;

    // UI Color
    void _setUiColor(uint16_t primary, uint16_t* secondary = nullptr, uint16_t* background = nullptr);
    void validateUiColor();

    bool openThemeFile(FS* fs, String filepath);
    String getThemeItemImg(String item) { return !theme.paths[item].isNull() ? themePath.substring(0,themePath.lastIndexOf('/'))+ "/" + theme.paths[item].as<String>() : "none"; };
    bool validateTheme(JsonObject& doc);

    // Need to call bruceConfig.saveFile();

    bool applyTheme(FS* fs, JsonObject& doc);
    void removeTheme(void);
};






#endif