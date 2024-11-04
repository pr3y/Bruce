#include "FMMenu.h"
#include "core/display.h"
#include "modules/fm/fm.h"

void FMMenu::optionsMenu() {
    options = {
//    #if !defined(LITE_VERSION) and defined(FM_SI4713)
      #if defined(FM_SI4713)
        {"Brdcast std",   [=]() { fm_live_run(false); }},
        {"Brdcast rsvd",  [=]() { fm_live_run(true); }},
        {"Brdcast stop",  [=]() { fm_stop(); }},
        {"FM Spectrum",   [=]() { fm_spectrum(); }},
        {"Hijack TA",     [=]() { fm_ta_run(); }},
    #endif
        {"Main Menu",     [=]() { backToMenu(); }}
    };

    delay(200);
    loopOptions(options,false,true,"FM");
}

String FMMenu::getName() {
    return _name;
}

void FMMenu::draw() {
    // Blank
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);

    // Case
    tft.drawRoundRect(-12+iconX,16+iconY,110,55,8,bruceConfig.priColor);
    tft.drawRoundRect(-12+iconX-1,16-1+iconY,112,57,8,bruceConfig.priColor);
    tft.drawRoundRect(-12+iconX-2,16-2+iconY,114,59,8,bruceConfig.priColor);

    // Potentiometer
    tft.fillCircle(75+iconX,40+iconY,12,bruceConfig.priColor);

    // Screen
    tft.drawRect(7+iconX,27+iconY,40,20,bruceConfig.priColor);

    // Antenna
    tft.drawLine(iconX  ,16+iconY,iconX+28,iconY+3,bruceConfig.priColor);
    tft.drawLine(iconX+1,16+iconY,iconX+29,iconY+3,bruceConfig.priColor);
    tft.fillCircle(iconX+28,iconY+3,2,bruceConfig.priColor);

    // Buttons
    tft.fillCircle(12+iconX,58+iconY,5,bruceConfig.priColor);
    tft.fillCircle(42+iconX,58+iconY,5,bruceConfig.priColor);
}