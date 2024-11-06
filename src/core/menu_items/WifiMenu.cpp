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

void WifiMenu::optionsMenu() {
    if(!wifiConnected) {
        options = {
        {"Connect Wifi", [=]()  { wifiConnectMenu(); }},    //wifi_common.h
        {"WiFi AP",      [=]()  { wifiConnectMenu(true); displayInfo("pwd: " + bruceConfig.wifiAp.pwd, true); }},//wifi_common.h
        };
    } else {
        options = {
        {"Disconnect",   [=]()  { wifiDisconnect(); }},    //wifi_common.h
        {"AP info",   [=]()  { displayAPInfo(); }},
        };
    }
    options.push_back({"Wifi Atks", [=]()     { wifi_atk_menu(); }});
    options.push_back({"Wardriving", [=]()    { Wardriving(); }});
#ifndef LITE_VERSION
    options.push_back({"TelNET", [=]()        { telnet_setup(); }});
    options.push_back({"SSH", [=]()           { ssh_setup(); }});
    options.push_back({"DPWO", [=]()          { dpwo_setup(); }});
    options.push_back({"Raw Sniffer", [=]()   { sniffer_setup(); }});
#endif
    options.push_back({"Evil Portal", [=]()   { startEvilPortal(); }});
#ifndef LITE_VERSION
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
