#include "powerSave.h"
#include "settings.h"
#include "display.h"

/* Check if it's time to put the device to sleep */
void checkPowerSaveTime(){
  if(bruceConfig.dimmerSet!=0){
    if((millis() - previousMillis) >= (bruceConfig.dimmerSet * 1000) && dimmer == false && isSleeping == false){
      dimmer = true;
      setBrightness(5, false);
    }else if((millis() - previousMillis) >= ((bruceConfig.dimmerSet * 1000) + 5000) && isScreenOff == false && isSleeping == false){
      isScreenOff = true;
      turnOffDisplay();
    }
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
