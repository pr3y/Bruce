#ifndef LITE_VERSION
#include <Arduino.h>
#include "core/mykeyboard.h"
#include "ui.h"
#include "spam.h"

#define STATE_INIT 0
#define STATE_WAKE 1
#define STATE_HALT 255

void advertise(uint8_t channel);
void wakeUp();

uint8_t state;
uint8_t current_channel = 1;
uint32_t last_mood_switch = 10001;
bool pwnagotchi_exit = false;

void pwnagotchi_setup() {
    initPwngrid();
    initUi();
    state = STATE_INIT;
    Serial.println("Pwnagotchi Initialized");
}

void pwnagotchi_update() {
    if (state == STATE_HALT) {
      return;
    }

    if (state == STATE_INIT) {
      state = STATE_WAKE;
      wakeUp();
    }

    if (state == STATE_WAKE) {
      checkPwngridGoneFriends();
      advertise(current_channel++);
      if (current_channel == 15) {
        current_channel = 1;
      }
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
  if (elapsed > 8000) {
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

void pwnagotchi_start() {
  tft.fillScreen(BGCOLOR);
  options = {
      {"Find frens",     [=]()  {  }},
      {"Pwngrid spam",   [=]()  { send_pwnagotchi_beacon_main(); }},
      {"Main Menu",      [=]()  { set_pwnagotchi_exit(true); }},
  };

  pwnagotchi_setup();
  delay(300); // Due to select button pressed to enter / quit this feature
  while(true) {
    pwnagotchi_update();
    if (checkSelPress()) {
      loopOptions(options);
    }
    if (pwnagotchi_exit) {
      break;
    }
    delay(100);
  }
  // Free memory
  deInitUi();
}
#endif
