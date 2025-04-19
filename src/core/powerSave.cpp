#include "powerSave.h"
#include "display.h"
#include "settings.h"

/* Check if it's time to put the device to sleep */
#define SCREEN_OFF_DELAY 5000
#define FADE_OUT_STEPS 10  // Number of fade-out steps
#define FADE_OUT_DELAY 30  // Delay between each fade-out step (milliseconds)

void checkPowerSaveTime() {
    if (bruceConfig.dimmerSet == 0) return;
    unsigned long elapsed = millis() - previousMillis;

    if (elapsed >= (bruceConfig.dimmerSet * 1000) && !dimmer && !isSleeping) {
        dimmer = true;
        setBrightness(5, false);
    } else if (elapsed >= ((bruceConfig.dimmerSet * 1000) + SCREEN_OFF_DELAY) && !isScreenOff &&
               !isSleeping) {
        isScreenOff = true;

        // Smooth fade-out effect
        for (int brightness = 100; brightness >= 0; brightness -= (100 / FADE_OUT_STEPS)) {
            setBrightness(brightness, false);
            delay(FADE_OUT_DELAY);
        }

        turnOffDisplay();
    }
}

/* Put device on sleep mode */
void sleepModeOn() {
    isSleeping = true;
    setCpuFrequencyMhz(80);

    // Smooth fade-out before going to sleep
    for (int brightness = 100; brightness >= 0; brightness -= (100 / FADE_OUT_STEPS)) {
        setBrightness(brightness, false);
        delay(FADE_OUT_DELAY);
    }

    turnOffDisplay();
    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
    delay(200);
}

/* Wake up device */
void sleepModeOff() {
    isSleeping = false;
    setCpuFrequencyMhz(240);
    getBrightness();
    enableCore0WDT();
    enableCore1WDT();
    enableLoopWDT();
    feedLoopWDT();
    delay(200);
}
