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
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);

    // Case
    tft.drawRoundRect(-12+iconX,16+iconY,110,55,8,FGCOLOR);
    tft.drawRoundRect(-12+iconX-1,16-1+iconY,112,57,8,FGCOLOR);
    tft.drawRoundRect(-12+iconX-2,16-2+iconY,114,59,8,FGCOLOR);

    // Potentiometer
    tft.fillCircle(75+iconX,40+iconY,12,FGCOLOR);

    // Screen
    tft.drawRect(7+iconX,27+iconY,40,20,FGCOLOR);

    // Antenna
    tft.drawLine(iconX  ,16+iconY,iconX+28,iconY+3,FGCOLOR);
    tft.drawLine(iconX+1,16+iconY,iconX+29,iconY+3,FGCOLOR);
    tft.fillCircle(iconX+28,iconY+3,2,FGCOLOR);

    // Buttons
    tft.fillCircle(12+iconX,58+iconY,5,FGCOLOR);
    tft.fillCircle(42+iconX,58+iconY,5,FGCOLOR);
}