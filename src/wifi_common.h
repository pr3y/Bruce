#include "display.h"
#include <WiFi.h>
#include <EEPROM.h>


bool wifiConnect(String ssid, int encryptation, bool isAP = false);

void wifiDisconnect();

void wifiConnectMenu( bool isAP = false);