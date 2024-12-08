#include "WifiMenu.h"
#include "core/display.h"
#include "core/wifi_common.h"
#include "core/wg.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/dpwo.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wifi_atks.h"
#include "modules/wifi/ap_info.h"

#ifndef LITE_VERSION
#include "modules/pwnagotchi/pwnagotchi.h"
#endif

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

void WifiMenu::drawIcon(float scale) {
    clearIconArea();

    int deltaY = scale * 20;
    int radius = scale * 6;

    tft.fillCircle(iconCenterX, iconCenterY + deltaY, radius, bruceConfig.priColor);
    tft.drawArc(
        iconCenterX, iconCenterY + deltaY,
        deltaY + radius, deltaY,
        130, 230,
        bruceConfig.priColor, bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX, iconCenterY + deltaY,
        2*deltaY + radius, 2*deltaY,
        130, 230,
        bruceConfig.priColor, bruceConfig.bgColor
    );
}
