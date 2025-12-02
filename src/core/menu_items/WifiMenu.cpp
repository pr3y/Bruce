#include "WifiMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/webInterface.h"
#include "core/wifi/wg.h"
#include "core/wifi/wifi_common.h"
#include "modules/wifi/ap_info.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/dpwo.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wifi_atks.h"
#include <i18n.h>

#ifndef LITE_VERSION
#include "modules/pwnagotchi/pwnagotchi.h"
#endif

// #include "modules/reverseShell/reverseShell.h"
//  Developed by Fourier (github.com/9dl)
//  Use BruceC2 to interact with the reverse shell server
//  BruceC2: https://github.com/9dl/Bruce-C2
//  To use BruceC2:
//  1. Start Reverse Shell Mode in Bruce
//  2. Start BruceC2 and wait.
//  3. Visit 192.168.4.1 in your browser to access the web interface for shell executing.

// 32bit: https://github.com/9dl/Bruce-C2/releases/download/v1.0/BruceC2_windows_386.exe
// 64bit: https://github.com/9dl/Bruce-C2/releases/download/v1.0/BruceC2_windows_amd64.exe
#include "modules/wifi/tcp_utils.h"

void WifiMenu::optionsMenu() {
    if (!wifiConnected) {
        options = {
            {LANG_CONNECT_WIFI, lambdaHelper(wifiConnectMenu, WIFI_STA)},
            {LANG_WIFI_AP,
             [=]() {
                 wifiConnectMenu(WIFI_AP);
                 displayInfo("pwd: " + bruceConfig.wifiAp.pwd, true);
             }},
        };
    } else {
        options = {
            {LANG_DISCONNECT, wifiDisconnect}
        };
        if (WiFi.getMode() == WIFI_MODE_STA) options.push_back({LANG_AP_INFO, displayAPInfo});
    }
    options.push_back({LANG_WIFI_ATKS, wifi_atk_menu});
    options.push_back({LANG_EVIL_PORTAL, [=]() {
                           if (isWebUIActive || server) {
                               stopWebUi();
                               wifiDisconnect();
                           }
                           EvilPortal();
                       }});
    // options.push_back({"ReverseShell", [=]()       { ReverseShell(); }});
    options.push_back({LANG_LISTEN_TCP, listenTcpPort});
    options.push_back({LANG_CLIENT_TCP, clientTCP});
#ifndef LITE_VERSION
    options.push_back({LANG_TELNET, telnet_setup});
    options.push_back({LANG_SSH, lambdaHelper(ssh_setup, String(""))});
    options.push_back({LANG_DPWO, dpwo_setup});
    options.push_back({LANG_RAW_SNIFFER, sniffer_setup});
    options.push_back({LANG_SCAN_HOSTS, local_scan_setup});
    options.push_back({LANG_WIREGUARD, wg_setup});
    options.push_back({LANG_BRUCEGOTCHI, brucegotchi_start});
#endif
    options.push_back({LANG_CONFIG, [=]() { configMenu(); }});
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "WiFi");
}

void WifiMenu::configMenu() {
    options = {
        {LANG_ADD_EVIL_WIFI,    addEvilWifiMenu         },
        {LANG_REMOVE_EVIL_WIFI, removeEvilWifiMenu      },
        {LANG_BACK,             [=]() { optionsMenu(); }},
    };

    loopOptions(options, MENU_TYPE_SUBMENU, "WiFi Config");
}
void WifiMenu::drawIconImg() {
    drawImg(
        *bruceConfig.themeFS(), bruceConfig.getThemeItemImg(bruceConfig.theme.paths.wifi), 0, imgCenterY, true
    );
}
void WifiMenu::drawIcon(float scale) {
    clearIconArea();
    int deltaY = scale * 20;
    int radius = scale * 6;

    tft.fillCircle(iconCenterX, iconCenterY + deltaY, radius, bruceConfig.priColor);
    tft.drawArc(
        iconCenterX,
        iconCenterY + deltaY,
        deltaY + radius,
        deltaY,
        130,
        230,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
    tft.drawArc(
        iconCenterX,
        iconCenterY + deltaY,
        2 * deltaY + radius,
        2 * deltaY,
        130,
        230,
        bruceConfig.priColor,
        bruceConfig.bgColor
    );
}
