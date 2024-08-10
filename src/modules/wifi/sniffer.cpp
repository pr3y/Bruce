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
#include "core/display.h"
#include "core/globals.h"
#include "core/sd_functions.h"
#include "core/wifi_common.h"
#include "core/mykeyboard.h"
#if defined(ESP32)
	#include "FS.h"
	//#include "SD.h"
#else
	#include <SPI.h>
	#include <SdFat.h>
#endif

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
bool isLittleFS = true;
uint32_t packet_counter = 0;

File file;

String filename = "/BrucePCAP/" + (String)FILENAME + ".pcap";

//===== FUNCTIONS =====//

/* write packet to file */
void newPacketSD(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, uint8_t* buf){
  if(file){

    uint32_t orig_len = len;
    uint32_t incl_len = len;
    //if(incl_len > snaplen) incl_len = snaplen; /* safty check that the packet isn't too big (I ran into problems here) */

    file.write((uint8_t*)&ts_sec, sizeof(ts_sec));
    file.write((uint8_t*)&ts_usec, sizeof(ts_usec));
    file.write((uint8_t*)&incl_len, sizeof(incl_len));
    file.write((uint8_t*)&orig_len, sizeof(orig_len));

    file.write(buf, incl_len);
  }
}

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
  
    file.write((uint8_t*)&magic_number, sizeof(magic_number));
    file.write((uint8_t*)&version_major, sizeof(version_major));
    file.write((uint8_t*)&version_minor, sizeof(version_minor));
    file.write((uint8_t*)&thiszone, sizeof(thiszone));
    file.write((uint8_t*)&sigfigs, sizeof(sigfigs));
    file.write((uint8_t*)&snaplen, sizeof(snaplen));
    file.write((uint8_t*)&network, sizeof(network));

    return true;
  }
  return false;
}

/* will be executed on every packet the ESP32 gets while beeing in promiscuous mode */
void sniffer(void *buf, wifi_promiscuous_pkt_type_t type){
  // If using LittleFS to save .pcaps and there's no room for data, don't do anything whith new packets
  if(isLittleFS && !checkLittleFsSizeNM()) {
    returnToMenu = true;
    esp_wifi_set_promiscuous(false);
    return;
  } 

  if(fileOpen){  
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

    uint32_t timestamp = now(); // current timestamp
    uint32_t microseconds = (unsigned int)(micros() - millis() * 1000); // microseconds offset (0 - 999)

    uint32_t len = ctrl.sig_len;
    if(type == WIFI_PKT_MGMT) {
      len -= 4; // Need to remove last 4 bytes (for checksum) or packet gets malformed # https://github.com/espressif/esp-idf/issues/886
    }

    newPacketSD(timestamp, microseconds, len, pkt->payload); // write packet to file
    packet_counter++;
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
int c = 0;

void openFile2(FS &Fs){
  //searches for the next non-existent file name
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
  String FileSys="LittleFS";
  if(setupSdCard()) { 
    Fs = &SD; // if SD is present and mounted, start writing on SD Card
    FileSys="SD";
    isLittleFS=false;
  }
  else Fs = &LittleFS;        // if not, use the internal memory.

  openFile2(*Fs);
  displayRedStripe("Sniffing Started", TFT_WHITE, FGCOLOR );
  tft.setTextSize(FP);
  tft.setCursor(80, 100);          
  tft.print("Enter to save"); 
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

  if(isLittleFS && !checkLittleFsSize()) goto Exit;

  for(;;) {
    if (returnToMenu) { // if it happpend, liffle FS is full;
      Serial.println("Not enough space on LittleFS");
      displayError("LittleFS Full");
      delay(5000);
      break;
    }
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

    if(checkSelPress()) { // Apertar o botão OK ou ENTER
      delay(200);
      file.flush(); //save file
      file.close();
      //closeFile();
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
      tft.setTextSize(FM);
      tft.setCursor(80, 100);          
      tft.print("Packets");
      displayRedStripe(filename, TFT_WHITE, FGCOLOR);
      tft.setTextSize(FM);
      tft.setCursor(80, 100);          
      tft.setTextColor(FGCOLOR, BGCOLOR);
      tft.setCursor(170, 100);          
      tft.print(packet_counter);
      //packet_counter=0;
      c++; //add to filename
      openFile2(*Fs); //open new file
    }
    if(checkEscPress()) { // Apertar o botão power ou Esc
      returnToMenu=true;
      file.close();
      goto Exit;
    }

  }
  Exit:
  esp_wifi_set_promiscuous(false);
  wifiDisconnect();
  delay(1);
}

