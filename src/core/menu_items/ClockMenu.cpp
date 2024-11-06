#include "ClockMenu.h"
#include "core/display.h"
#include "core/settings.h"

void ClockMenu::optionsMenu() {
    runClockLoop();
}

String ClockMenu::getName() {
    return _name;
}

void ClockMenu::draw() {
    // Blank
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);

    // Case
    tft.drawCircle(40+iconX,40+iconY,30,bruceConfig.priColor);
    tft.drawCircle(40+iconX,40+iconY,31,bruceConfig.priColor);
    tft.drawCircle(40+iconX,40+iconY,32,bruceConfig.priColor);

    // Pivot center
    tft.fillCircle(40+iconX,40+iconY,3,bruceConfig.priColor);

    // Hours & minutes
    tft.drawLine(40+iconX,40+iconY,40+iconX-10,40+iconY-10,bruceConfig.priColor);
    tft.drawLine(40+iconX,40+iconY,40+iconX+16,40+iconY-16,bruceConfig.priColor);
}