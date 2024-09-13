#include "ConfigMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/i2c_finder.h"

void ConfigMenu::optionsMenu() {
    options = {
        {"Brightness",    [=]() { setBrightnessMenu(); }},
        {"Dim Time",      [=]() { setDimmerTimeMenu(); }},
        {"Orientation",   [=]() { gsetRotation(true); }},
        {"UI Color",      [=]() { setUIColor(); }},
        {"Sound On/Off",  [=]() { setSoundConfig(); }},
        {"Clock",         [=]() { setClock(); }},
        {"Sleep",         [=]() { setSleepMode(); }},
        {"Restart",       [=]() { ESP.restart(); }},
    };

    if (appConfig.getDevMode()) options.push_back({"Dev Mode", [=]() { devMenu(); }});
    options.push_back({"Main Menu", [=]() { backToMenu(); }});

    delay(200);
    loopOptions(options,false,true,"Config");
}

void ConfigMenu::devMenu(){
    options = {
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
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);
    int i=0;
    for(i=0;i<6;i++) {
        tft.drawArc(40+iconX,40+iconY,30,20,15+60*i,45+60*i,FGCOLOR,BGCOLOR,true);
    }
    tft.drawArc(40+iconX,40+iconY,22,8,0,360,FGCOLOR,BGCOLOR,false);
}