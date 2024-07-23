#include "powerSave.h"
#include "settings.h"

/* Turn off the display */
void turnOffDisplay() {
  #if !defined(STICK_C_PLUS)
  analogWrite(BACKLIGHT, 0);
  #else
  axp192.ScreenBreath(0);
  #endif
}

/* If the device screen is off, turn on, else just refresh sleep timer */
bool wakeUpScreen(){
  previousMillis = millis();
  if(isScreenOff){
    isScreenOff = false;
    dimmer = false;
    getBrightness();
    delay(200);
    return true;
  }else if(dimmer){
    dimmer = false;
    getBrightness();
    delay(200);
    return true;
  } 
  return false;
}

/* Check if it's time to put the device to sleep */
void checkPowerSaveTime(){
  if(dimmerSet!=0){
    if((millis() - previousMillis) >= (dimmerSet * 1000) && dimmer == false && isSleeping == false){
      dimmer = true;
      setBrightness(5, false);
    }else if((millis() - previousMillis) >= ((dimmerSet * 1000) + 5000) && isScreenOff == false && isSleeping == false){
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
  delay(200);
}

/* Wake up device */
void sleepModeOff(){
  isSleeping = false;
  setCpuFrequencyMhz(240);
  getBrightness();
  delay(200);
}
