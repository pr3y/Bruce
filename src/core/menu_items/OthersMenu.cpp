#include "OthersMenu.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "modules/others/openhaystack.h"
#include "modules/others/tururururu.h"
#include "modules/others/webInterface.h"
#include "modules/others/qrcode_menu.h"
#include "modules/others/mic.h"
#include "modules/bjs_interpreter/interpreter.h"

#ifdef USB_as_HID
#include "modules/others/bad_usb.h"
#endif
#ifdef HAS_RGB_LED
#include "modules/others/led_control.h"
#endif

void OthersMenu::optionsMenu() {
    options = {
        {"SD Card",      [=]() { loopSD(SD); }},
        {"LittleFS",     [=]() { loopSD(LittleFS); }},
        {"WebUI",        [=]() { loopOptionsWebUi(); }},
        {"QRCodes",      [=]() { qrcode_menu(); }},
        {"Megalodon",    [=]() { shark_setup(); }},
    #ifdef MIC_SPM1423
        {"Mic Spectrum", [=]() { mic_test(); }},
    #endif
    #ifdef USB_as_HID
        {"BadUSB",       [=]()  { usb_setup(); }},
        #if defined(CARDPUTER)
        {"USB Keyboard", [=]()  { usb_keyboard(); }},
        #endif
    #endif
    #ifdef HAS_RGB_LED
        {"LED Control",  [=]()  { ledrgb_setup(); }}, //IncursioHack
        {"LED FLash",    [=]()  { ledrgb_flash(); }}, // IncursioHack
    #endif
    #ifndef LITE_VERSION
        {"Openhaystack", [=]()  { openhaystack_setup(); }},
    #endif
    #if !defined(CORE) && !defined(CORE2)
        {"Interpreter", [=]()   { run_bjs_script(); }},
    #endif
        {"Main Menu",    [=]()  { backToMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"Others");
}

String OthersMenu::getName() {
    return name;
}

void OthersMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);
    tft.fillCircle(40+iconX,40+iconY,7,FGCOLOR);
    tft.drawArc(40+iconX,40+iconY,18,15,0,340,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,40+iconY,25,22,20,360,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,40+iconY,32,29,0,200,FGCOLOR,BGCOLOR);
    tft.drawArc(40+iconX,40+iconY,32,29,240,360,FGCOLOR,BGCOLOR);
}