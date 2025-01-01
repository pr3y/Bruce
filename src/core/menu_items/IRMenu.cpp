#include "IRMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/custom_ir.h"
#include "modules/ir/ir_read.h"
#include "core/utils.h"

void IRMenu::optionsMenu() {
    options = {
        {"TV-B-Gone", [=]() { StartTvBGone(); }},
        {"Custom IR", [=]() { otherIRcodes(); }},
        {"IR Read",   [=]() { IrRead(); }},
        {"Config",    [=]() { configMenu(); }},
        {"Main Menu", [=]() { backToMenu(); }}
    };

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

    loopOptions(options,false,true,"IR Config");
}

void IRMenu::drawIcon(float scale) {
    clearIconArea();

    int iconSize = scale * 60;
    int radius = scale * 7;
    int deltaRadius = scale * 10;

    if (iconSize % 2 != 0) iconSize++;

    tft.fillRect(
        iconCenterX - iconSize/2,
        iconCenterY - iconSize/2,
        iconSize/6,
        iconSize,
        bruceConfig.priColor
    );
    tft.fillRect(
        iconCenterX - iconSize/3,
        iconCenterY - iconSize/3,
        iconSize/6,
        2*iconSize/3,
        bruceConfig.priColor
    );

    tft.drawCircle(
        iconCenterX - iconSize/6,
        iconCenterY,
        radius,
        bruceConfig.priColor
    );

    tft.drawArc(
        iconCenterX - iconSize/6,
        iconCenterY,
        2.5*radius, 2*radius,
        220, 320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize/6,
        iconCenterY,
        2.5*radius + deltaRadius, 2*radius + deltaRadius,
        220, 320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX - iconSize/6,
        iconCenterY,
        2.5*radius + 2*deltaRadius, 2*radius + 2*deltaRadius,
        220, 320,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
}