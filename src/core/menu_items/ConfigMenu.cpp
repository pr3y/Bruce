#include "ConfigMenu.h"
#include "core/utils.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/i2c_finder.h"
#include "core/wifi_common.h"
#ifdef HAS_RGB_LED
#include "core/led_control.h"
#endif

void ConfigMenu::optionsMenu() {
    options = {
        {"Brightness",    setBrightnessMenu},
        {"Dim Time",      setDimmerTimeMenu},
        {"Orientation",   lambdaHelper(gsetRotation, true) },
        {"UI Color",      setUIColor},
        {"UI Theme",      setTheme},
    #ifdef HAS_RGB_LED
        {"LED Color",     [=]() { beginLed(); setLedColorConfig(); }},
        {"LED Brightness",[=]() { beginLed(); setLedBrightnessConfig(); }},
    #endif
        {"Sound On/Off",  setSoundConfig },
        {"Startup WiFi",  setWifiStartupConfig },
        {"Startup App",   setStartupApp },
        {"Network Creds", setNetworkCredsMenu },
        {"Clock",         setClock },
        {"Sleep",         setSleepMode },
        {"Factory Reset", [=]() { bruceConfig.factoryReset(); }},
        {"Restart",       [=]() { ESP.restart(); }},
    };

    options.push_back({"Turn-off",   powerOff });
    options.push_back({"Deep Sleep", goToDeepSleep });

    if (bruceConfig.devMode) options.push_back({"Dev Mode", [=]() { devMenu(); }});

    options.push_back({"About", showDeviceInfo});
    addOptionToMainMenu();

    loopOptions(options,true,"Config");
}

void ConfigMenu::devMenu(){
    options = {
        {"I2C Finder",    find_i2c_addresses},
        {"CC1101 Pins",   [=]() { setSPIPinsMenu(bruceConfig.CC1101_bus); }},
        {"NRF24  Pins",   [=]() { setSPIPinsMenu(bruceConfig.NRF24_bus); }},
        {"SDCard Pins",   [=]() { setSPIPinsMenu(bruceConfig.SDCARD_bus); }},
        {"Back",          [=]() { optionsMenu(); }},
    };

    loopOptions(options,true,"Dev Mode");
}
void ConfigMenu::drawIconImg() {
    if(bruceConfig.theme.config) {
        FS* fs = nullptr;
        if(bruceConfig.theme.fs == 1) fs=&LittleFS;
        else if (bruceConfig.theme.fs == 2) fs=&SD;
        drawImg(*fs, bruceConfig.getThemeItemImg(bruceConfig.theme.paths.config), 0, imgCenterY, true);
    }
}
void ConfigMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 9;

    int i=0;
    for(i=0; i<6; i++) {
        tft.drawArc(
            iconCenterX,
            iconCenterY,
            3.5*radius, 2*radius,
            15+60*i, 45+60*i,
            bruceConfig.priColor,
            bruceConfig.bgColor,
            true
        );
    }

    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5*radius, radius,
        0, 360,
        bruceConfig.priColor,
        bruceConfig.bgColor,
        false
    );
}
