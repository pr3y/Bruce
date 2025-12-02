#include "ConfigMenu.h"
#include "core/display.h"
#include "core/i2c_finder.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"
#include <i18n.h>
#ifdef HAS_RGB_LED
#include "core/led_control.h"
#endif

void ConfigMenu::optionsMenu() {
    options = {
        {LANG_BRIGHTNESS, setBrightnessMenu},
        {LANG_DIM_TIME, setDimmerTimeMenu},
        {LANG_ORIENTATION, lambdaHelper(gsetRotation, true)},
        {LANG_UI_COLOR, setUIColor},
        {LANG_UI_THEME, setTheme},
#ifdef HAS_RGB_LED
        {LANG_LED_COLOR,
         [=]() {
             beginLed();
             setLedColorConfig();
         }},
        {LANG_LED_BRIGHTNESS,
         [=]() {
             beginLed();
             setLedBrightnessConfig();
         }},
        {LANG_LED_BLINK_ON_OFF, setLedBlinkConfig},
#endif
        {LANG_SOUND_ON_OFF, setSoundConfig},
        {LANG_LANGUAGE, setLanguageMenu},
        {LANG_STARTUP_WIFI, setWifiStartupConfig},
        {LANG_STARTUP_APP, setStartupApp},
        {LANG_NETWORK_CREDS, setNetworkCredsMenu},
        {LANG_CLOCK, setClock},
        {LANG_SLEEP, setSleepMode},
        {LANG_FACTORY_RESET, [=]() { bruceConfig.factoryReset(); }},
        {LANG_RESTART, [=]() { ESP.restart(); }},
    };

    options.push_back({LANG_TURN_OFF, powerOff});
    options.push_back({LANG_DEEP_SLEEP, goToDeepSleep});

    if (bruceConfig.devMode) options.push_back({LANG_DEV_MODE, [=]() { devMenu(); }});

    options.push_back({LANG_ABOUT, showDeviceInfo});
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Config");
}

void ConfigMenu::devMenu() {
    options = {
        {LANG_I2C_FINDER,  find_i2c_addresses                               },
        {LANG_CC1101_PINS, [=]() { setSPIPinsMenu(bruceConfig.CC1101_bus); }},
        {LANG_NRF24_PINS, [=]() { setSPIPinsMenu(bruceConfig.NRF24_bus); } },
        {LANG_SDCARD_PINS, [=]() { setSPIPinsMenu(bruceConfig.SDCARD_bus); }},
        {LANG_BACK,        [=]() { optionsMenu(); }                         },
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "Dev Mode");
}
void ConfigMenu::drawIconImg() {
    drawImg(
        *bruceConfig.themeFS(),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.config),
        0,
        imgCenterY,
        true
    );
}
void ConfigMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 9;

    int i = 0;
    for (i = 0; i < 6; i++) {
        tft.drawArc(
            iconCenterX,
            iconCenterY,
            3.5 * radius,
            2 * radius,
            15 + 60 * i,
            45 + 60 * i,
            bruceConfig.priColor,
            bruceConfig.bgColor,
            true
        );
    }

    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5 * radius,
        radius,
        0,
        360,
        bruceConfig.priColor,
        bruceConfig.bgColor,
        false
    );
}
