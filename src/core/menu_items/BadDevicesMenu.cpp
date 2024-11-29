#include "BadDevicesMenu.h"
#include "../globals.h"
#include "core/display.h"

#include "modules/ble/ble_spam.h"
#include "modules/ble/ble_common.h"

#include "modules/ble/bad_ble.h"
#include "modules/others/bad_usb.h"

// Love from 9dl <3
void BadDevicesMenu::optionsMenu() {
    options.clear();
    if(BLEConnected) options.push_back({"Disconnect",     [=]() {
        BLEDevice::deinit();
        BLEConnected=false;
        if(Ask_for_restart==1) Ask_for_restart=2;
    }});

    options.push_back({"Media Cmds",     [=]() { ble_MediaCommands(); }});

    #if !defined(LITE_VERSION)
        options.push_back({"Bad BLE",      [&]() { ble_setup(); }});
    #endif

    options.push_back({"Bad USB",       [&]()  { usb_setup(); }});
    options.push_back({"Main Menu",    [=]() { backToMenu(); }});
    
    delay(200);
    loopOptions(options, false, true, "Bad Devices");
}

String BadDevicesMenu::getName() {
    return _name;
}

void BadDevicesMenu::draw() {
    tft.fillRect(iconX, iconY, 80, 80, bruceConfig.bgColor);
    tft.fillCircle(40 + iconX, 40 + iconY, 30, bruceConfig.priColor);
    tft.fillCircle(30 + iconX, 30 + iconY, 5, bruceConfig.bgColor);
    tft.fillCircle(50 + iconX, 30 + iconY, 5, bruceConfig.bgColor);
    tft.fillCircle(40 + iconX, 45 + iconY, 5, bruceConfig.bgColor);
}