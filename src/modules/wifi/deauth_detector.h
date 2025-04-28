#ifndef DEAUTH_DETECTOR_H
#define DEAUTH_DETECTOR_H

#include <vector>
#include <string>
#include <esp_wifi.h>
#include <WiFi.h>

void deauthSniffer(void *buf, wifi_promiscuous_pkt_type_t type);
void startDeauthDetector();
void stopDeauthDetector();

#endif // DEAUTH_DETECTOR_H
