#include <ESPping.h>
#include <HTTPClient.h>
#include <WiFi.h>

bool internetConnection();

String getManufacturer(const String &mac);

String MAC(uint8_t *data);
