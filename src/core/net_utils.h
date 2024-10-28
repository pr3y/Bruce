#include <WiFi.h>
#include <ESPping.h>
#include <HTTPClient.h>

bool internetConnection();

String getManufacturer(const String& mac);

String MAC(uint8_t* data);