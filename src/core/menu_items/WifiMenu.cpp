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
#include "modules/ble/ble_jammer.h"

void WifiMenu::optionsMenu() {
    if(!wifiConnected) {
        options = {
        {"Connect Wifi", [=]()  { wifiConnectMenu(); }},    //wifi_common.h
        {"WiFi AP",      [=]()  { wifiConnectMenu(true); }},//wifi_common.h
        };
    } else {
        options = {
        {"Disconnect",   [=]()  { wifiDisconnect(); }},    //wifi_common.h
        };
    }
    options.push_back({"Wifi Atks", [=]()     { wifi_atk_menu(); }});
    options.push_back({"Wardriving", [=]()    { Wardriving(); }});
#ifndef LITE_VERSION
    options.push_back({"TelNET", [=]()        { telnet_setup(); }});
    options.push_back({"SSH", [=]()           { ssh_setup(); }});
    options.push_back({"DPWO", [=]()          { dpwo_setup(); }});
#endif
    options.push_back({"Raw Sniffer", [=]()   { sniffer_setup(); }});
    options.push_back({"Evil Portal", [=]()   { startEvilPortal(); }});
    options.push_back({"Scan Hosts", [=]()    { local_scan_setup(); }});
#ifndef LITE_VERSION
    options.push_back({"Wireguard", [=]()     { wg_setup(); }});
#endif
#if defined(USE_NRF24_VIA_SPI)
    options.push_back({"NRF24 Jammer", [=]() { ble_jammer(); }});
#endif
    options.push_back({"Main Menu", [=]()     { backToMenu(); }});

    delay(200);
    loopOptions(options,false,true,"WiFi");
}

String WifiMenu::getName() {
    return name;
}

void WifiMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);
    tft.fillCircle(40+iconX,60+iconY,6,FGCOLOR);
    tft.drawArc(40+iconX,60+iconY,26,20,130,230,FGCOLOR, BGCOLOR);
    tft.drawArc(40+iconX,60+iconY,46,40,130,230,FGCOLOR, BGCOLOR);
}