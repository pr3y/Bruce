#include "BleMenu.h"
#include <globals.h>
#include "core/utils.h"
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

        addOption("Media Cmds", ble_MediaCommands);
#if !defined(LITE_VERSION)
        addOption("BLE Beacon", ble_test);
        addOption("BLE Scan", ble_scan);
        addOption("Bad BLE", ble_setup);
    #endif
    #if defined(HAS_KEYBOARD_HID)
        addOption("BLE Keyboard", ble_keyboard);
    #endif
    addOption("iOS Spam",     aj_adv,0);
    addOption("Windows Spam", aj_adv,1);
    addOption("Samsung Spam", aj_adv,2);
    addOption("Android Spam", aj_adv,3);
    addOption("Spam All",     aj_adv,4);
    addOptionToMainMenu();

    loopOptions(options,false,true,"Bluetooth");
}
void BleMenu::drawIconImg() {
    if(bruceConfig.theme.ble) {
        FS* fs = nullptr;
        if(bruceConfig.theme.fs == 1) fs=&LittleFS;
        else if (bruceConfig.theme.fs == 2) fs=&SD;
        drawImg(*fs, bruceConfig.getThemeItemImg(bruceConfig.theme.paths.ble), 0, imgCenterY, true);
    }
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
        iconCenterX + lineWidth/2,
        iconCenterY - iconH/4,
        iconCenterX - iconW/2,
        iconCenterY + lineWidth/2,
        iconCenterX - iconW/2,
        iconCenterY - iconH/2 - lineWidth/2,
        bruceConfig.priColor
    );
    tft.fillTriangle(
        iconCenterX + lineWidth/2,
        iconCenterY + iconH/4,
        iconCenterX - iconW/2,
        iconCenterY - lineWidth/2,
        iconCenterX - iconW/2,
        iconCenterY + iconH/2 + lineWidth/2,
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
