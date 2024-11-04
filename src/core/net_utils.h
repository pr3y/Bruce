#include <WiFi.h>
#include <ESPping.h>
#include <HTTPClient.h>

bool internetConnection();

String getPublicIP();

String getManufacturer(const String& mac);

String MAC(uint8_t* data);