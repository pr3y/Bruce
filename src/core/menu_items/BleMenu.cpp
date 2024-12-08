#include "BleMenu.h"
#include "../globals.h"
#include "core/display.h"
#include "modules/ble/ble_spam.h"
#include "modules/ble/ble_common.h"
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
    #if defined(HAS_KEYBOARD_HID)
        options.push_back({"BLE Keyboard", [=]() { ble_keyboard(); }});
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

void BleMenu::drawIcon(float scale) {
    clearIconArea();

    int lineWidth = scale * 5;
    int iconW = scale * 36;
    int iconH = scale * 60;
    int radius = scale * 5;
    int deltaRadius = scale * 10;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 4 != 0) iconH += 4 - (iconH % 4);

    tft.drawWideLine(
        iconCenterX,
        iconCenterY + iconH/4,
        iconCenterX - iconW,
        iconCenterY - iconH/4,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawWideLine(
        iconCenterX,
        iconCenterY - iconH/4,
        iconCenterX - iconW,
        iconCenterY + iconH/4,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawWideLine(
        iconCenterX,
        iconCenterY + iconH/4,
        iconCenterX - iconW/2,
        iconCenterY + iconH/2,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawWideLine(
        iconCenterX,
        iconCenterY - iconH/4,
        iconCenterX - iconW/2,
        iconCenterY - iconH/2,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );

    tft.drawWideLine(
        iconCenterX - iconW/2,
        iconCenterY - iconH/2,
        iconCenterX - iconW/2,
        iconCenterY + iconH/2,
        lineWidth,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );

    tft.fillTriangle(
        iconCenterX,
        iconCenterY - iconH/4,
        iconCenterX - iconW/2,
        iconCenterY,
        iconCenterX - iconW/2,
        iconCenterY - iconH/2,
        bruceConfig.priColor
    );
    tft.fillTriangle(
        iconCenterX,
        iconCenterY + iconH/4,
        iconCenterX - iconW/2,
        iconCenterY,
        iconCenterX - iconW/2,
        iconCenterY + iconH/2,
        bruceConfig.priColor
    );

    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5*radius, 2*radius,
        210, 330,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5*radius + deltaRadius, 2*radius + deltaRadius,
        210, 330,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5*radius + 2*deltaRadius, 2*radius + 2*deltaRadius,
        210, 330,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );

}