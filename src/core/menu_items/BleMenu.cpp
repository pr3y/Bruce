#include "BleMenu.h"
#include "core/display.h"
#include "modules/ble/ble_spam.h"
#include "modules/ble/ble_common.h"
#include "modules/ble/ble_jammer.h"

void BleMenu::optionsMenu() {
    options = {
    #if !defined(LITE_VERSION)
        {"BLE Beacon",   [=]() { ble_test(); }},
        {"BLE Scan",     [=]() { ble_scan(); }},
    #endif
    #if defined(USE_NRF24_VIA_SPI)
        {"NRF24 Jammer", [=]() { ble_jammer(); }},
    #endif
        {"iOS Spam",     [=]() { aj_adv(0); }},
        {"Windows Spam", [=]() { aj_adv(1); }},
        {"Samsung Spam", [=]() { aj_adv(2); }},
        {"Android Spam", [=]() { aj_adv(3); }},
        {"Spam All",     [=]() { aj_adv(4); }},
        {"Main Menu",    [=]() { backToMenu(); }},
    };
    delay(200);
    loopOptions(options,false,true,"Bluetooth");
}

String BleMenu::getName() {
    return _name;
}

void BleMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);
    tft.drawWideLine(40+iconX,53+iconY,5+iconX,26+iconY,5,FGCOLOR,BGCOLOR);
    tft.drawWideLine(40+iconX,26+iconY,5+iconX,53+iconY,5,FGCOLOR,BGCOLOR);
    tft.drawWideLine(40+iconX,53+iconY,20+iconX,68+iconY,5,FGCOLOR,BGCOLOR);
    tft.drawWideLine(40+iconX,26+iconY,20+iconX,12+iconY,5,FGCOLOR,BGCOLOR);
    tft.drawWideLine(20+iconX,12+iconY,20+iconX,68+iconY,5,FGCOLOR,BGCOLOR);
    tft.fillTriangle(40+iconX,26+iconY,20+iconX,40+iconY,20+iconX,12+iconY,FGCOLOR);
    tft.fillTriangle(40+iconX,53+iconY,20+iconX,40+iconY,20+iconX,68+iconY,FGCOLOR);
    tft.drawArc(40+iconX,40+iconY,10,12,210,330,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,40+iconY,23,25,210,330,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,40+iconY,36,38,210,330,FGCOLOR,BGCOLOR);
}