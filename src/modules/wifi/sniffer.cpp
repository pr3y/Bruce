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
#include "core/display.h"
#include "core/globals.h"
#include "core/sd_functions.h"



//===== SETTINGS =====//
#define CHANNEL 1
#define FILENAME "raw_"
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
String filename = "/BrucePCAP/" + (String)FILENAME + ".pcap";

//===== FUNCTIONS =====//

bool openFile(FS &Fs){
    uint32_t magic_number = 0xa1b2c3d4;
    uint16_t version_major = 2;
    uint16_t version_minor = 4;
    uint32_t thiszone = 0;
    uint32_t sigfigs = 0;
    uint32_t snaplen = 2500;
    uint32_t network = 105;

	  file = Fs.open(filename, FILE_WRITE);
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

    uint32_t timestamp = now(); // current timestamp
    uint32_t microseconds = (unsigned int)(micros() - millis() * 1000); // microseconds offset (0 - 999)

    uint32_t len = ctrl.sig_len;
    if(type == WIFI_PKT_MGMT) {
      len -= 4; // Need to remove last 4 bits (for checksum) or packet gets malformed # https://github.com/espressif/esp-idf/issues/886
    }

    newPacketSD(timestamp, microseconds, len, pkt->payload); // write packet to file

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
void openFile2(FS &Fs){
  //searches for the next non-existent file name
  int c = 0;
  if (!Fs.exists("/BrucePCAP")) Fs.mkdir("/BrucePCAP");
  filename = "/BrucePCAP/" + (String)FILENAME + (String)c + ".pcap";
  while(Fs.open(filename)){
    filename = "/BrucePCAP/" + (String)FILENAME + (String)c + ".pcap";
    c++;
  }

  fileOpen = openFile(Fs);

  Serial.println("opened: "+filename);

  //reset counter (counter for saving every X seconds)
  counter = 0;
}


//===== SETUP =====//
void sniffer_setup() {
  FS* Fs;
  drawMainBorder();
  tft.setCursor(10, 26);
  Serial.begin(115200);
  //delay(2000);
  Serial.println();
  closeSdCard();
  if(setupSdCard()) Fs = &SD; // if SD is present and mounted, start writing on SD Card
  else Fs = &LittleFS;        // if not, use the internal memory.

  openFile2(*Fs);
  displayRedStripe("Sniffing Started", TFT_WHITE, FGCOLOR );
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

  sniffer_loop(*Fs);

}

void sniffer_loop(FS &Fs) {
    String FileSys="LittleFS";
    if(&Fs == &SD) FileSys="SD";

    if(FileSys=="LittleFS" && !checkLittleFsSize()) goto Exit;

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
          tft.setTextSize(FP);
          tft.setTextColor(FGCOLOR, BGCOLOR);
          tft.setCursor(10, 30);          
          tft.println("RAW SNIFFER");
          tft.setCursor(10, 30);          
          tft.println("RAW SNIFFER");          
          tft.setCursor(10, tft.getCursorY()+3);
          tft.println("Saved file into " + FileSys);
          displayRedStripe(filename, TFT_WHITE, FGCOLOR);
          // tft.println(filename);
          tft.setTextColor(FGCOLOR, BGCOLOR);
          openFile2(Fs); //open new file
        }
       // }

          if(checkEscPress()) { // Apertar o botão power dos sticks
            tft.fillScreen(BGCOLOR);
            file.flush(); //save file
            file.close();
            returnToMenu=true;
            break;
            //goto Exit;
          }
    }
    Exit:
    delay(1); // just to Exit Work
}
