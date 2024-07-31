#include <Arduino.h>
#include "core/globals.h"
#include "core/display.h"
#include "core/main_menu.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include <SD.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <set>
#include <time.h>
#include "wardriving.h"

/* Made by @IncursioHack */

TinyGPSPlus gps;
HardwareSerial GPSserial(2); // Utiliza UART2 para o GPS
std::set<String> registeredMACs; // Conjunto para rastrear endereços MAC registrados
int wifiNetworkCount = 0; // Contador de redes Wi-Fi encontradas

void wardriving_logData() {
  if (wifiNetworkCount==0) drawMainBorder();
  tft.setCursor(10, 30);
  tft.println("WARDRIVING:"); 
  tft.setCursor(10, 38);
  tft.println("  Wi-Fi Networks Found: "  + String(wifiNetworkCount));
  // Escaneia redes Wi-Fi próximas
  int n = WiFi.scanNetworks();
  drawMainBorder();
  // Verifica se há redes disponíveis
  if (n == 0) {
    tft.setCursor(10, tft.getCursorY());      
    tft.println("No Wi-Fi networks found");
  } else {
    // Abre o arquivo para escrita

    // 
    File file;
    bool FirstLine = false;
    FS* Fs;
    if(sdcardMounted) Fs = &SD;
    else Fs = &LittleFS;
    if(!(*Fs).exists("/wardriving_bruce_log.csv")) FirstLine=true;
    file = (*Fs).open("/wardriving_bruce_log.csv", FirstLine ? FILE_WRITE : FILE_APPEND);
    if(FirstLine) file.println("WigleWifi-1.4,appRelease=v1.0.0,model=ESP32 M5Stack,release=v1.0.0,device=ESP32 M5Stack,display=SPI TFT,board=ESP32 M5Stack,brand=Bruce\nMAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,CurrentLongitude,AltitudeMeters,AccuracyMeters,Type");
    
    if (file) {
      // Limpa a tela antes de exibir novas informações
      int ListCursorStart = 49;
      tft.setCursor(10, ListCursorStart);
      // Grava os dados para cada rede Wi-Fi encontrada
      int line=0;
      for (int i = 0; i < n; i++) {
        String macAddress = WiFi.BSSIDstr(i);
        
        // Verifica se o endereço MAC já foi registrado
        if (registeredMACs.find(macAddress) == registeredMACs.end()) {
          registeredMACs.insert(macAddress); // Adiciona o MAC ao conjunto

          char buffer[512];
          snprintf(buffer, sizeof(buffer), "%s,%s,[%s],%02d/%02d/%02d %02d:%02d:%02d,%d,%d,%f,%f,%f,%f,WIFI\n",
                   macAddress.c_str(),
                   WiFi.SSID(i).c_str(),
                   wardriving_authModeToString(WiFi.encryptionType(i)).c_str(),
                   gps.date.month(), gps.date.day(), gps.date.year(),
                   gps.time.hour(), gps.time.minute(), gps.time.second(),
                   WiFi.channel(i),
                   WiFi.RSSI(i),
                   gps.location.lat(),
                   gps.location.lng(),
                   gps.altitude.meters(),
                   gps.hdop.hdop() * 1.0);
          file.print(buffer);

          // Incrementa o contador de redes Wi-Fi encontradas
          wifiNetworkCount++;

          // Exibe informações na tela
          tft.setTextSize(FP);
          tft.setTextColor(FGCOLOR, BGCOLOR); 
          if(i % 4 == 0 && i>0) { //Show 3 networks at a time, delays 500ms to see what has been found
            tft.setCursor(10, ListCursorStart); 
            tft.fillSmoothRoundRect(10,49,WIDTH-20,HEIGHT-59,5,BGCOLOR);
            line=0;
            delay(500); 
            } 
          tft.setCursor(10, ListCursorStart + line*8);
          tft.println("WIFI:" + WiFi.SSID(i).substring(0,15) + " [" + wardriving_authModeToString(WiFi.encryptionType(i)).c_str() + "]");
          tft.setCursor(10, ListCursorStart + (line+1)*8);
          tft.printf("GPS: %.4f , %.4f", gps.location.lat(), gps.location.lng());
          tft.setCursor(10, tft.getCursorY()+LH*FP+4);
          line +=2;
        }
      }

      file.close();
//      tft.setCursor(10, tft.getCursorY()+20); 
//      tft.println("Recorded GPS data and Wi-Fi networks");
    } else {
      tft.setCursor(10, tft.getCursorY()+20); 
      tft.println("Failed to open file for writing");
    }
  }
}

String wardriving_authModeToString(wifi_auth_mode_t authMode) {
  switch (authMode) {
    case WIFI_AUTH_OPEN: return "OPEN";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2_ENTERPRISE";
    case WIFI_AUTH_WPA3_PSK: return "WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2_WPA3_PSK";
    case WIFI_AUTH_WAPI_PSK: return "WAPI_PSK";
    default: return "UNKNOWN";
  }
}

void wardriving_setup() {
  // Inicialização do M5Stack
  Serial.begin(115200);
  drawMainBorder(); // no loop para dar refresh na borda sem piscar a tela
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); 
  tft.setTextSize(FP);
  tft.setTextColor(FGCOLOR, BGCOLOR); 
  tft.setCursor(10, 30); 
  tft.setCursor(10, tft.getCursorY()); 
  tft.println("  Initializing...");

  // Inicializa o serial do GPS
  GPSserial.begin(9600, SERIAL_8N1, GROVE_SCL, GROVE_SDA); // RX, TX

  // Verifica o cartão SD
  if (!SD.begin()) {
    tft.setCursor(10, tft.getCursorY());  
    tft.println("  Failed to initialize SD card");
    tft.setCursor(10, tft.getCursorY());  
    tft.println("  Using LittleFS");
   
    while (1);
  } else {
    tft.setCursor(10, tft.getCursorY()); 
    tft.println("  SD card successfully mounted");

  }

  // Waits until GPS is connected
  tft.setCursor(10, tft.getCursorY()); 
  tft.println("  Waiting for GPS data");
  int count=0;
  while(GPSserial.available() <= 0) {
    if(checkEscPress()) goto EndGPS;
    displayRedStripe("Waiting GPS: " + String(count)+ "s",TFT_WHITE, FGCOLOR);
    delay(1000);
  }

  gpsConnected=true;
  drawMainBorder(); // no loop para dar refresh na borda sem piscar a tela
  // Loop contínuo
  count=0;
  while (true) {
    tft.setCursor(10, 30); 
    tft.setTextSize(FP);
    tft.setTextColor(FGCOLOR, BGCOLOR); 
    // Depuração para verificar disponibilidade de dados no GPS
    if (GPSserial.available() > 0) {
      tft.setCursor(10, tft.getCursorY()); 
      tft.println("  GPS data available");
      while (GPSserial.available() > 0) {
        gps.encode(GPSserial.read());
      }

      // Depuração para verificar se a localização do GPS foi atualizada
      if (gps.location.isUpdated()) {
        tft.setCursor(10, tft.getCursorY()); 
        tft.println("  GPS location updated");
        wifiConnected=true;
        wardriving_logData();
      } else {
        tft.setCursor(10, tft.getCursorY()+3); 
        tft.println("  GPS location not updated");
        tft.setCursor(10, tft.getCursorY()+3);         
        tft.printf("  Time: %02d:%02d:%02d\n", gps.time.hour(), gps.time.minute(), gps.time.second());
        tft.setCursor(10, tft.getCursorY()); 
        tft.printf("  Date: %02d/%02d/%02d\n", gps.date.month(), gps.date.day(), gps.date.year());
        tft.setCursor(10, tft.getCursorY()); 
        tft.printf("  Satellites: %d\n", gps.satellites.value());
        tft.setCursor(10, tft.getCursorY()); 
        tft.printf("  HDOP: %f\n", gps.hdop.hdop());
      }
    } else {
      if(count>5) { 
        displayError("GPS not Found!");
        delay(2000);
        break;
      }
      tft.setCursor(10, tft.getCursorY()); 
      tft.println("  No GPS data available");
      count++;
    }

    // Keep running to wait next GPS Iteration
    int tmp = millis();
    // Checks para sair do while
    while(millis()-tmp < 10000 && GPSserial.available() <= 0) {
      if (checkEscPress()) {
          displayRedStripe("Stopped");
          delay(2000);
          returnToMenu = true;
          goto EndGPS;
          break;
      }
    }
  }
EndGPS:
  wifiDisconnect();
  GPSserial.end();
  returnToMenu=true;
  gpsConnected=false;
}
