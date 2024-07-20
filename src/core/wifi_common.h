#include "display.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <Timezone.h>

bool wifiConnect(String ssid, int encryptation, bool isAP = false);

void wifiDisconnect();

bool wifiConnectMenu( bool isAP = false);