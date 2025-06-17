#include "powerSave.h"
#include "display.h"
#include "settings.h"

/* Check if it's time to put the device to sleep */
#define SCREEN_OFF_DELAY 5000

void fadeOutScreen(int startValue) {
    for (int brightValue = startValue; brightValue >= 0; brightValue -= 1) {
        setBrightness(max(brightValue, 0), false);
        delay(5);
    }
    turnOffDisplay();
}

void checkPowerSaveTime() {
    if (bruceConfig.dimmerSet == 0) return;

    unsigned long elapsed = millis() - previousMillis;
    int startDimmerBright = bruceConfig.bright / 3;
    int dimmerSetMs = bruceConfig.dimmerSet * 1000;

    if (elapsed >= dimmerSetMs && !dimmer && !isSleeping) {
        dimmer = true;
        setBrightness(startDimmerBright, false);
    } else if (elapsed >= (dimmerSetMs + SCREEN_OFF_DELAY) && !isScreenOff && !isSleeping) {
        isScreenOff = true;
        fadeOutScreen(startDimmerBright);
    }
}

void sleepModeOn() {
    isSleeping = true;
    setCpuFrequencyMhz(80);

    int startDimmerBright = bruceConfig.bright / 3;

    fadeOutScreen(startDimmerBright);

#ifdef HAS_TFTSCREEN
    panelSleep(true); //  power down screen
#endif

    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
    delay(200);
}

void sleepModeOff() {
    isSleeping = false;
    setCpuFrequencyMhz(240);

#ifdef HAS_TFTSCREEN
    panelSleep(false); // wake the screen back up
#endif

    getBrightness();
    enableCore0WDT();
    enableCore1WDT();
    enableLoopWDT();
    feedLoopWDT();
    delay(200);
}
