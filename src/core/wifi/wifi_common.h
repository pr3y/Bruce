#include "core/display.h"
#include <NTPClient.h>
#include <Timezone.h>
#include <WiFi.h>

#ifndef __WIFI_COMMON_H__
#define __WIFI_COMMON_H__
// TODO wrap in a class

// public
/**
 * @brief disconnects and turns off wifi module
 */
void wifiDisconnect();

/**
 * @brief Opens a menu to connect to a wifi
 * @param mode connection mode(AP, STA, AP_STA)
 * @note This is the primary entry point for establishing connections
 * @note returns false if wifi is already connected
 */
bool wifiConnectMenu(wifi_mode_t = WIFI_MODE_STA);

/**
 * @brief returns MAC adress
 */
String checkMAC();

/**
 * @brief tries to connect to min(found_networks, maxSearch) networks
 * using stored passwords
 * @TODO fix: rn it skips open networks due to password == "" check
 */
void wifiConnectTask(void *pvParameters);

// private
/**
 * @brief Connects to wifiNetwork
 */
bool _wifiConnect(const String &ssid, int encryption);
bool _connectToWifiNetwork(const String &ssid, const String &pwd);

/**
 * @brief sets up wifi in AP mode
 * @note wifi.mode should be set before calling the method
 */
bool _setupAP();

#endif
