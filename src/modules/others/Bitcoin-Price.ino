#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>

// Remplacez par vos informations WiFi
const char* ssid = "SFR-0658";
const char* password = "DCFPZJF1DZKN";

// Adresse API pour récupérer le prix Bitcoin en USD (CoinGecko utilisé ici)
const char* bitcoinPriceApi = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

// Initialisation de l'écran TFT
TFT_eSPI tft = TFT_eSPI();

void setup() {
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

void loop() {
  // Vérifie la connexion WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connexion WiFi perdue. Reconnexion...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Reconnecté!");
  }

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
      tft.fillScreen(TFT_WHITE);  // Efface l'écran

      // Affiche le prix sur l'écran TFT
      tft.setTextColor(TFT_BLUE);
      
      // Taille de texte réduite pour afficher tout le contenu
      tft.setTextSize(2);  // Taille de texte

      // Positionne le texte légèrement décalé à droite
      int x = (40); // Position horizontale décalée à droite
      int y = (tft.height() / 2) - 20; // Ajustement vertical

      tft.setCursor(x, y);
      tft.print("BTC/USD: $");
      tft.println(bitcoinPrice, 2); // Affiche le prix avec 2 décimales
    }
  } else {
    Serial.print("Erreur HTTP: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  // Attends 5 minutes avant de faire une nouvelle requête
  delay(300000); // 300000 ms = 5 minutes
}
