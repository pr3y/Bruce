#include "BleMenu.h"
#include "../globals.h"
#include "core/display.h"
#include "modules/ble/ble_spam.h"
#include "modules/ble/ble_common.h"
#include "modules/ble/bad_ble.h"

void BleMenu::optionsMenu() {
    options.clear();

    if (BLEConnected) {
        options.push_back({"Disconnect", [=]() {
            BLEDevice::deinit();
            BLEConnected = false;
            if (Ask_for_restart == 1) Ask_for_restart = 2;
        }});
    }

    options.push_back({"Media Cmds", [=]() { ble_MediaCommands(); }});

    #if !defined(LITE_VERSION)
        options.push_back({"BLE Scan", [=]() { ble_scan(); }});
        options.push_back({"Bad BLE", [=]() { ble_setup(); }});
    #endif

    #if defined(HAS_KEYBOARD_HID)
        options.push_back({"BLE Keyboard", [=]() { ble_keyboard(); }});
    #endif

    const char* spamLabels[] = {"iOS Spam", "Windows Spam", "Samsung Spam", "Android Spam", "Spam All"};
    for (int i = 0; i < 5; i++) options.push_back({spamLabels[i], [=]() { aj_adv(i); }});

    options.push_back({"Main Menu", [=]() { backToMenu(); }});

    delay(200);
    loopOptions(options, false, true, "Bluetooth");
}

String BleMenu::getName() {
    return _name;
}


void BleMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);
    tft.drawWideLine(40+iconX,53+iconY,5+iconX,26+iconY,5,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawWideLine(40+iconX,26+iconY,5+iconX,53+iconY,5,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawWideLine(40+iconX,53+iconY,20+iconX,68+iconY,5,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawWideLine(40+iconX,26+iconY,20+iconX,12+iconY,5,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawWideLine(20+iconX,12+iconY,20+iconX,68+iconY,5,bruceConfig.priColor,bruceConfig.bgColor);
    tft.fillTriangle(40+iconX,26+iconY,20+iconX,40+iconY,20+iconX,12+iconY,bruceConfig.priColor);
    tft.fillTriangle(40+iconX,53+iconY,20+iconX,40+iconY,20+iconX,68+iconY,bruceConfig.priColor);
    tft.drawArc(40+iconX,40+iconY,10,12,210,330,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawArc(40+iconX,40+iconY,23,25,210,330,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawArc(40+iconX,40+iconY,36,38,210,330,bruceConfig.priColor,bruceConfig.bgColor);
}