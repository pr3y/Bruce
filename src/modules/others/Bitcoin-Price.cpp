#include "BitcoinPrice.h"

// Informations WiFi
const char* ssid = "your_wifi_name";
const char* password = "your-wifi_password";

// Adresse API pour récupérer le prix Bitcoin en USD
const char* bitcoinPriceApi = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

// Initialisation de l'écran TFT
TFT_eSPI tft = TFT_eSPI();

void setupDisplay() {
  Serial.begin(115200);

  // Initialise l'écran TFT
  tft.init();
  
  // Régle l'orientation de l'écran pour afficher le texte à l'envers
  tft.setRotation(2); // Rotation pour afficher le texte tête en bas
  
  // Connexion au réseau WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion au WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connecté!");

  // Affiche le titre sur l'écran TFT
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLUE);
  tft.setTextSize(1);  // Taille de la police
  tft.setCursor(10, 10);
  tft.println("Bitcoin Price Tracker");
}

void fetchBitcoinPrice() {
  // Vérifie la connexion WiFi
  checkWiFiConnection();
  if(!wifiConnected) wifiConnectMenu(false);
  else(!wifiDisconneted) wifiConnectMenu(true);

  // Fait une requête HTTP pour récupérer le prix Bitcoin en USD
  HTTPClient http;
  http.begin(bitcoinPriceApi);

  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Code de réponse HTTP: ");
    Serial.println(httpResponseCode);
    Serial.println(response);

    // Parse la réponse JSON pour obtenir le prix
    int index = response.indexOf("usd\":");
    if (index != -1) {
      String priceStr = response.substring(index + 5, response.indexOf('}', index));
      float bitcoinPrice = priceStr.toFloat();

      // Efface l'affichage précédent
      tft.fillScreen(TFT_WHITE);

      // Affiche le prix sur l'écran TFT
      tft.setTextColor(TFT_BLUE);
      tft.setTextSize(2);
      tft.setCursor(40, tft.height() / 2 - 20);
      tft.print("BTC/USD: $");
      tft.println(bitcoinPrice, 2); // Affiche le prix avec 2 décimales
    }
  } else {
    Serial.print("Erreur HTTP: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connexion WiFi perdue. Reconnexion...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Reconnecté!");
  }
}
