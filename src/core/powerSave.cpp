#include "powerSave.h"
#include "settings.h"
#include "display.h"

/* Check if it's time to put the device to sleep */
#define SCREEN_OFF_DELAY 5000

void checkPowerSaveTime() {
  if (bruceConfig.dimmerSet == 0) return;
  unsigned long elapsed = millis() - previousMillis;

  if (elapsed >= (bruceConfig.dimmerSet * 1000) && !dimmer && !isSleeping) {
    dimmer = true;
    setBrightness(5, false);
  } 
  else if (elapsed >= ((bruceConfig.dimmerSet * 1000) + SCREEN_OFF_DELAY) && !isScreenOff && !isSleeping) {
    isScreenOff = true;
    turnOffDisplay();
  }
}

/* Put device on sleep mode */
void sleepModeOn(){
  isSleeping = true;
  setCpuFrequencyMhz(80);
  turnOffDisplay();
  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();
  delay(200);
}

/* Wake up device */
void sleepModeOff(){
  isSleeping = false;
  setCpuFrequencyMhz(240);
  getBrightness();
  enableCore0WDT();
  enableCore1WDT();
  enableLoopWDT();
  feedLoopWDT();
  delay(200);
}
