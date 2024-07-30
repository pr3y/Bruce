//Wardriving WIFI - @IncursioHack - https://github.com/IncursioHack
#ifndef WAR_DRIVING_H
#define WAR_DRIVING_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include <set>

extern TinyGPSPlus gps;
extern HardwareSerial GPSserial;
extern std::set<String> registeredMACs;

void wardriving_setup();
void wardriving_logData();
String wardriving_authModeToString(wifi_auth_mode_t authMode);

#endif // WAR_DRIVING_H