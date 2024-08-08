#ifdef CARDPUTER
  #include "M5Cardputer.h"
#endif

#include "M5Unified.h"
#include "ui.h"

#define STATE_INIT 0
#define STATE_WAKE 1
#define STATE_HALT 255

void advertise(uint8_t channel);
void wakeUp();
void initM5();

uint8_t state;
uint8_t current_channel = 1;
uint32_t last_mood_switch = 10001;

void initM5() {
  auto cfg = M5.config();
  M5.begin();
  M5.Display.begin();
  #ifdef CARDPUTER
    M5Cardputer.begin(cfg);
    M5Cardputer.Keyboard.begin();
  #endif
}

void palnagotchi_setup() {
    initM5();
    Serial.println("M5Stack Initialized");
    initPwngrid();
    initUi();
    state = STATE_INIT;
}

void palnagotchi_update() {
    M5.update();
    #ifdef CARDPUTER
      M5Cardputer.update();
    #endif

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
