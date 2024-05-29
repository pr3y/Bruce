/*
  ===========================================
       Copyright (c) 2017 Stefan Kremser
              github.com/spacehuhn
  ===========================================
*/


/* include all necessary libraries */ 
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
//#include "esp_wifi_internal.h"
#include "lwip/err.h"
#include "esp_system.h"
#include "esp_event.h"
//#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include <Arduino.h>
#include <TimeLib.h>
#include "FS.h"
#include "PCAP.h"
#include "display.h"
#include "globals.h"
#include "sd_functions.h"



//===== SETTINGS =====//
#define CHANNEL 1
#define FILENAME "raw"
#define SAVE_INTERVAL 10 //save new file every 30s
#define CHANNEL_HOPPING true //if true it will scan on all channels
#define MAX_CHANNEL 11 //(only necessary if channelHopping is true)
#define HOP_INTERVAL 214 //in ms (only necessary if channelHopping is true)


//===== Run-Time variables =====//
unsigned long lastTime = 0;
unsigned long lastChannelChange = 0;
int counter = 0;
int ch = CHANNEL;
bool fileOpen = false;

//PCAP pcap = PCAP();
PCAP pcap;
String filename = "/" + (String)FILENAME + ".pcap";

//===== FUNCTIONS =====//

bool openFile(){
  	//String filename = "capture.cap";

	
    uint32_t magic_number = 0xa1b2c3d4;
    uint16_t version_major = 2;
    uint16_t version_minor = 4;
    uint32_t thiszone = 0;
    uint32_t sigfigs = 0;
    uint32_t snaplen = 2500;
    uint32_t network = 105;

	  //if(SD.exists(filename.c_str())) removeFile(SD);
	  file = SD.open(filename, FILE_WRITE);
	  if(file) {

		filewrite_32(magic_number);
		filewrite_16(version_major);
		filewrite_16(version_minor);
		filewrite_32(thiszone);
		filewrite_32(sigfigs);
		filewrite_32(snaplen);
		filewrite_32(network);
		return true;
	  }
	  return false;
	}


/* will be executed on every packet the ESP32 gets while beeing in promiscuous mode */
void sniffer(void *buf, wifi_promiscuous_pkt_type_t type){
  
  if(fileOpen){
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;
  
    uint32_t timestamp = now(); //current timestamp 
    uint32_t microseconds = (unsigned int)(micros() - millis() * 1000); //micro seconds offset (0 - 999)
    newPacketSD(timestamp, microseconds, ctrl.sig_len, pkt->payload); //write packet to file
    
  }
  
}

//esp_err_t event_handler(void *ctx, system_event_t *event){ return ESP_OK; }
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                // Ação para quando a estação WiFi inicia
                break;
            // Outros casos...
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP:
                // Ação para quando a estação WiFi obtém um endereço IP
                break;
            // Outros casos...
        }
    }
}

/* opens a new file */
void openFile2(){

  //searches for the next non-existent file name
  int c = 0;
  while(SD.open(filename)){
    filename = "/" + (String)FILENAME + "_" + (String)c + ".pcap";
    c++;
  }
  
  fileOpen = openFile();

  Serial.println("opened: "+filename);

  //reset counter (counter for saving every X seconds)
  counter = 0;
}


//===== SETUP =====//
void sniffer_setup() {
  tft.fillScreen(BGCOLOR);
  tft.setCursor(0, 0);
  Serial.begin(115200);
  //delay(2000);
  Serial.println();

  sdcardSPI.begin(SDCARD_SCK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS);
  delay(10);
  SD.begin(SDCARD_CS, sdcardSPI);
  
  uint8_t cardType = SD.cardType();
  
  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      displayRedStripe("No SD card");
      return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }

  int64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  openFile2();

  /* setup wifi */
  nvs_flash_init();
  //tcpip_adapter_init();             //velho
  ESP_ERROR_CHECK(esp_netif_init());  //novo
  //ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );                                          //velho
  ESP_ERROR_CHECK(esp_event_loop_create_default());                                                       // novo
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));        // novo
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));       // novo
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
  ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
  ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );  
  ESP_ERROR_CHECK( esp_wifi_start() );
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(sniffer);
  wifi_second_chan_t secondCh = (wifi_second_chan_t)NULL;
  esp_wifi_set_channel(ch,secondCh);

  Serial.println("Sniffer started!");
  
  displayRedStripe("Sniffer started!", TFT_WHITE, TFT_DARKGREEN );
 
  sniffer_loop();

}

void sniffer_loop() {
    for(;;) {
     // if ((checkSelPress())) {
        unsigned long currentTime = millis();
        
        /* Channel Hopping */
        if(CHANNEL_HOPPING){
          if(currentTime - lastChannelChange >= HOP_INTERVAL){
            lastChannelChange = currentTime;
            ch++; //increase channel
            if(ch > MAX_CHANNEL) ch = 1;
            wifi_second_chan_t secondCh = (wifi_second_chan_t)NULL;
            esp_wifi_set_channel(ch,secondCh);
          }
        }

        if(fileOpen && currentTime - lastTime > 1000){
          file.flush(); //save file
          lastTime = currentTime; //update time
          counter++; //add 1 to counter
        }
        
        /* when counter > 30s interval */
        if(counter > SAVE_INTERVAL){
          //closeFile(); //save & close the file
          file.close();
          fileOpen = false; //update flag
          Serial.println("==================");
          Serial.println(filename + " saved!");
          Serial.println("==================");
          tft.setCursor(0, 20);
          tft.setTextColor(TFT_WHITE, BGCOLOR);
          tft.setTextSize(2);
          tft.println("Saved to file in SD card, filename:");
          tft.setTextSize(2);
          displayRedStripe(filename, TFT_WHITE, FGCOLOR);
          // tft.println(filename);
          tft.setTextColor(FGCOLOR, BGCOLOR);
          openFile2(); //open new file
        }
       // }

          #ifndef CARDPUTER
          if(checkPrevPress()) { // Apertar o botão power dos sticks
            tft.fillScreen(BGCOLOR);
            returnToMenu=true;
            break;
            //goto Exit;
          }
          #else
          Keyboard.update();
          if(Keyboard.isKeyPressed('`')) {
            tft.fillScreen(BGCOLOR);
            returnToMenu=true;
            break;
            //goto Exit;
          }   // apertar ESC no Cardputer
          #endif
       
    }
}