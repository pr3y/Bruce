#include "BleMenu.h"
#include "../globals.h"
#include "core/display.h"
#include "modules/ble/ble_spam.h"
#include "modules/ble/ble_common.h"
#include "modules/ble/ble_jammer.h"
#include "modules/ble/bad_ble.h"

void BleMenu::optionsMenu() {
    options.clear();
    if(BLEConnected) options.push_back({"Disconnect",     [=]() { 
        BLEDevice::deinit(); 
        BLEConnected=false; 
        if(Ask_for_restart==1) Ask_for_restart=2; // Sets the variable to ask for restart;
    }});
    
    options.push_back({"Media Cmds",     [=]() { ble_MediaCommands(); }});
#if !defined(LITE_VERSION)
    // options.push_back({"BLE Beacon",   [=]() { ble_test(); }});
    options.push_back({"BLE Scan",     [=]() { ble_scan(); }});
    options.push_back({"Bad BLE",      [=]() { ble_setup(); }});
#endif
#if defined(CARDPUTER)
    options.push_back({"BLE Keyboard", [=]() { ble_keyboard(); }});
#endif
#if defined(USE_NRF24_VIA_SPI)
    options.push_back({"NRF24 Jammer", [=]() { ble_jammer(); }});
#endif
    options.push_back({"iOS Spam",     [=]() { aj_adv(0); }});
    options.push_back({"Windows Spam", [=]() { aj_adv(1); }});
    options.push_back({"Samsung Spam", [=]() { aj_adv(2); }});
    options.push_back({"Android Spam", [=]() { aj_adv(3); }});
    options.push_back({"Spam All",     [=]() { aj_adv(4); }});
    options.push_back({"Main Menu",    [=]() { backToMenu(); }});
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