#include "ConfigMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/i2c_finder.h"
#include "core/wifi_common.h"

void ConfigMenu::optionsMenu() {
    options = {
        #if defined(CYD) // Brightness control -> Not working yet, don't know why! @Pirata, Delete if from here after you solve this thing
        {"Brightness",    [=]() { displayWarning("Bright CTRL not working",true);}},
        #else
        {"Brightness",    [=]() { setBrightnessMenu(); }},
        #endif
        {"Dim Time",      [=]() { setDimmerTimeMenu(); }},
        {"Orientation",   [=]() { gsetRotation(true); }},
        {"UI Color",      [=]() { setUIColor(); }},
        {"Sound On/Off",  [=]() { setSoundConfig(); }},
        {"Startup WiFi",  [=]() { setWifiStartupConfig(); }},
        {"Clock",         [=]() { setClock(); }},
        {"Sleep",         [=]() { setSleepMode(); }},
        {"Restart",       [=]() { ESP.restart(); }},
    };

  #if defined(T_EMBED_1101)
    options.push_back({"Turn-off",  [=]() { digitalWrite(PIN_POWER_ON,LOW); esp_sleep_enable_ext0_wakeup(GPIO_NUM_6,LOW); esp_deep_sleep_start(); }});
  #endif
    if (bruceConfig.devMode) options.push_back({"Dev Mode", [=]() { devMenu(); }});

    options.push_back({"Main Menu", [=]() { backToMenu(); }});

    delay(200);
    loopOptions(options,false,true,"Config");
}

void ConfigMenu::devMenu(){
    options = {
        {"MAC Address",   [=]() { checkMAC(); }},
        {"I2C Finder",    [=]() { find_i2c_addresses(); }},
        {"Back",          [=]() { optionsMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"Dev Mode");
}

String ConfigMenu::getName() {
    return _name;
}

void ConfigMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);
    int i=0;
    for(i=0;i<6;i++) {
        tft.drawArc(40+iconX,40+iconY,30,20,15+60*i,45+60*i,bruceConfig.priColor,bruceConfig.bgColor,true);
    }
    tft.drawArc(40+iconX,40+iconY,22,8,0,360,bruceConfig.priColor,bruceConfig.bgColor,false);
}