#include "display.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <Timezone.h>

bool wifiConnect(String ssid, int encryption, bool isAP = false);
bool wifiApConnect();

bool _connectToWifiNetwork(String ssid, String pwd);
void _updateClockTimezone();

void wifiDisconnect();

bool wifiConnectMenu( bool isAP = false);

void checkMAC();

void wifiConnectTask(int maxSearch = 5);
