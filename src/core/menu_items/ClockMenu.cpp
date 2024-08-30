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
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);

    // Case
    tft.drawCircle(40+iconX,40+iconY,30,FGCOLOR);
    tft.drawCircle(40+iconX,40+iconY,31,FGCOLOR);
    tft.drawCircle(40+iconX,40+iconY,32,FGCOLOR);

    // Pivot center
    tft.fillCircle(40+iconX,40+iconY,3,FGCOLOR);

    // Hours & minutes
    tft.drawLine(40+iconX,40+iconY,40+iconX-10,40+iconY-10,FGCOLOR);
    tft.drawLine(40+iconX,40+iconY,40+iconX+16,40+iconY-16,FGCOLOR);
}