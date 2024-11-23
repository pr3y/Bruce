#include "WifiMenu.h"
#include "core/display.h"
#include "core/wifi_common.h"
#include "core/wg.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/dpwo.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wardriving.h"
#include "modules/wifi/wifi_atks.h"
#include "modules/wifi/ap_info.h"

#ifndef LITE_VERSION
#include "modules/pwnagotchi/pwnagotchi.h"
#endif

void WifiMenu::gpsConfigMenu() {
    options = {
        {"GPS Generic (9600)", [=]() {
            bruceConfig.setGPSModule(GPS_GENERIC);
            displaySuccess("Set to Generic GPS", true);
        }},
        {"GPS 1.1 M5Stack (115200)", [=]() {
            bruceConfig.setGPSModule(GPS_M5STACK_V1_1);
            displaySuccess("Set to M5Stack GPS 1.1", true);
        }},
        {"Back", [=]() { return; }}
    };
    
    while(1) {
        drawMainBorderWithTitle("GPS Module Selection");
        padprintln("");
        padprintln("Current: " + String(bruceConfig.gpsModule == GPS_M5STACK_V1_1 ? 
            "M5Stack GPS 1.1 (115200)" : 
            "Generic GPS (9600)"));
        padprintln("");
        
        if(loopOptions(options)) break;
    }
}

void WifiMenu::optionsMenu() {
    if(!wifiConnected) {
        options = {
            {"Connect Wifi", [=]()  { wifiConnectMenu(WIFI_STA); }},  
            {"WiFi AP",      [=]()  { wifiConnectMenu(WIFI_AP); displayInfo("pwd: " + bruceConfig.wifiAp.pwd, true); }},
        };
    } else {
        options = {{"Disconnect",   [=]()  { wifiDisconnect(); }} };
        if(WiFi.getMode() == WIFI_MODE_STA) options.push_back({"AP info",   [=]()  { displayAPInfo(); }});
    }
    
    options.push_back({"Wifi Atks", [=]()     { wifi_atk_menu(); }});
    options.push_back({"Evil Portal", [=]()   { EvilPortal(); }});
    options.push_back({"Wardriving", [=]()    { Wardriving(); }});
    options.push_back({"GPS Config", [=]()    { gpsConfigMenu(); }});  // Add GPS configuration menu

#ifndef LITE_VERSION
    options.push_back({"TelNET", [=]()        { telnet_setup(); }});
    options.push_back({"SSH", [=]()           { ssh_setup(); }});
    options.push_back({"DPWO", [=]()          { dpwo_setup(); }});
    options.push_back({"Raw Sniffer", [=]()   { sniffer_setup(); }});
    options.push_back({"Scan Hosts", [=]()    { local_scan_setup(); }});
    options.push_back({"Wireguard", [=]()     { wg_setup(); }});
    options.push_back({"Brucegotchi",  [=]()   { brucegotchi_start(); }});
#endif
    options.push_back({"Main Menu", [=]()     { backToMenu(); }});

    delay(200);
    loopOptions(options,false,true,"WiFi");
}

String WifiMenu::getName() {
    return _name;
}

void WifiMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);
    tft.fillCircle(40+iconX,60+iconY,6,bruceConfig.priColor);
    tft.drawArc(40+iconX,60+iconY,26,20,130,230,bruceConfig.priColor, bruceConfig.bgColor);
    tft.drawArc(40+iconX,60+iconY,46,40,130,230,bruceConfig.priColor, bruceConfig.bgColor);
}