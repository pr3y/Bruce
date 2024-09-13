#include "RFMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "modules/rf/rf.h"

void RFMenu::optionsMenu() {
    options = {
        {"Scan/copy",     [=]() { RCSwitch_Read(); }},
        {"Custom SubGhz", [=]() { otherRFcodes(); }},
        {"Spectrum",      [=]() { rf_spectrum(); }}, //@IncursioHack
        {"Jammer Itmt",   [=]() { rf_jammerIntermittent(); }}, //@IncursioHack
        {"Jammer Full",   [=]() { rf_jammerFull(); }}, //@IncursioHack
        {"Config",        [=]() { configMenu(); }},
        {"Main Menu",     [=]() { backToMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"Radio Frequency");
}

void RFMenu::configMenu() {
    options = {
        {"RF TX Pin",     [=]() { gsetRfTxPin(true); }},
        {"RF RX Pin",     [=]() { gsetRfRxPin(true); }},
        {"RF Module",     [=]() { setRFModuleMenu(); }},
        {"RF Frequency",  [=]() { setRFFreqMenu(); }},
        {"Back",          [=]() { optionsMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"RF Config");
}

String RFMenu::getName() {
    return _name;
}

void RFMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);
    tft.fillCircle(40+iconX,30+iconY,7,FGCOLOR);
    tft.fillTriangle(40+iconX,40+iconY,25+iconX,70+iconY,55+iconX,70+iconY,FGCOLOR);
    tft.drawArc(40+iconX,30+iconY,18,15,40,140,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,30+iconY,28,25,40,140,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,30+iconY,38,35,40,140,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,30+iconY,18,15,220,320,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,30+iconY,28,25,220,320,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,30+iconY,38,35,220,320,FGCOLOR,BGCOLOR);
}