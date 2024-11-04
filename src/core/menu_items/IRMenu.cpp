#include "IRMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/ir_read.h"

void IRMenu::optionsMenu() {
    options = {
        {"TV-B-Gone", [=]() { StartTvBGone(); }},
        {"Custom IR", [=]() { otherIRcodes(); }},
        {"IR Read",   [=]() { IrRead(); }},
        {"Config",    [=]() { configMenu(); }},
        {"Main Menu", [=]() { backToMenu(); }}
    };

    delay(200);
    String txt = "Infrared";
    txt+=" Tx: " + String(bruceConfig.irTx) + " Rx: " + String(bruceConfig.irRx);
    loopOptions(options,false,true,txt);
}

void IRMenu::configMenu() {
    options = {
        {"Ir TX Pin", [=]() { gsetIrTxPin(true); }},
        {"Ir RX Pin", [=]() { gsetIrRxPin(true); }},
        {"Back",      [=]() { optionsMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"IR Config");
}

String IRMenu::getName() {
    return _name;
}

void IRMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);
    tft.fillRoundRect(11+iconX,10+iconY,10,60,2,bruceConfig.priColor);
    tft.fillRoundRect(21+iconX,20+iconY,10,40,2,bruceConfig.priColor);
    tft.drawCircle(31+iconX,40+iconY,7,bruceConfig.priColor);
    tft.drawArc(31+iconX,40+iconY,18,15,220,320,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawArc(31+iconX,40+iconY,28,25,220,320,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawArc(31+iconX,40+iconY,38,35,220,320,bruceConfig.priColor,bruceConfig.bgColor);
}