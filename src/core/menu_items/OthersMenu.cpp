#include "OthersMenu.h"
#include "core/display.h"
#include "core/utils.h"
#include "modules/others/openhaystack.h"
#include "modules/others/tururururu.h"
#include "modules/others/qrcode_menu.h"
#include "modules/others/mic.h"
#include "modules/bjs_interpreter/interpreter.h"
#include "modules/others/timer.h"
#include "modules/others/clicker.h"
#include "modules/others/bad_usb.h"
#include "modules/others/ibutton.h"

void OthersMenu::optionsMenu() {
    options = {
        {"QRCodes",      [=]() { qrcode_menu(); }},
        {"Megalodon",    [=]() { shark_setup(); }},
    #ifdef MIC_SPM1423
        {"Mic Spectrum", [=]() { mic_test(); }},
    #endif
        {"BadUSB",       [=]() { usb_setup(); }},
    #ifdef HAS_KEYBOARD_HID
        {"USB Keyboard", [=]() { usb_keyboard(); }},
    #endif
    #ifdef USB_as_HID
        {"Clicker",       [=]() { clicker_setup(); }},
    #endif
    #ifndef LITE_VERSION
        {"Openhaystack", [=]() { openhaystack_setup(); }},
    #endif
    #if !defined(ARDUINO_M5STACK_ARDUINO_M5STACK_CORE) && !defined(ARDUINO_M5STACK_ARDUINO_M5STACK_CORE2)
        {"Interpreter", [=]()  { run_bjs_script(); }},
    #endif
        {"iButton",        [=]() { setup_ibutton(); }},
        {"Timer",        [=]() { Timer(); }},
        {"Main Menu",    [=]() { backToMenu(); }},
    };


    loopOptions(options,false,true,"Others");
}

void OthersMenu::drawIcon(float scale) {
    clearIconArea();
    if(bruceConfig.theme.others) {
        FS* fs = nullptr;
        if(bruceConfig.theme.fs == 1) fs=&LittleFS;
        else if (bruceConfig.theme.fs == 2) fs=&SD;
        showJpeg(*fs, bruceConfig.getThemeItemImg("others"), iconCenterX, iconCenterY, true);
        return;
    }
    int radius = scale * 7;

    tft.fillCircle(iconCenterX, iconCenterY, radius, bruceConfig.priColor);

    tft.drawArc(
        iconCenterX, iconCenterY,
        2.5*radius, 2*radius,
        0, 340,
        bruceConfig.priColor, bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX, iconCenterY,
        3.5*radius, 3*radius,
        20, 360,
        bruceConfig.priColor, bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX, iconCenterY,
        4.5*radius, 4*radius,
        0, 200,
        bruceConfig.priColor, bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX, iconCenterY,
        4.5*radius, 4*radius,
        240, 360,
        bruceConfig.priColor, bruceConfig.bgColor
    );
}
