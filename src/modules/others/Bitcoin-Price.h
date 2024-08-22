#ifndef BITCOIN_PRICE_H
#define BITCOIN_PRICE_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>

// Informations WiFi
extern const char* ssid;
extern const char* password;

// Adresse API pour récupérer le prix Bitcoin en USD
extern const char* bitcoinPriceApi;

// Initialisation de l'écran TFT
extern TFT_eSPI tft;

// Fonctions
void setupDisplay();
void fetchBitcoinPrice();
void checkWiFiConnection();

#endif // BITCOIN_PRICE_H
