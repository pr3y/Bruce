#include "powerSave.h"
#include "display.h"
#include "settings.h"

/* Check if it's time to put the device to sleep */
#define SCREEN_OFF_DELAY 5000
#define FADE_OUT_STEPS 5  // Number of fade-out steps ( reduced from 10 to 5 )
#define FADE_OUT_DELAY 3  // Delay between each fade-out step (milliseconds) ( reduced from 30 to 3)

void checkPowerSaveTime() {
    if (bruceConfig.dimmerSet == 0) return;
    unsigned long elapsed = millis() - previousMillis;

    if (elapsed >= (bruceConfig.dimmerSet * 1000) && !dimmer && !isSleeping) {
        dimmer = true;
        setBrightness(5, false);
    } else if (elapsed >= ((bruceConfig.dimmerSet * 1000) + SCREEN_OFF_DELAY) && !isScreenOff &&
               !isSleeping) {
        isScreenOff = true;

        int current = bruceConfig.bright;

        // If brightness is at 1% or lower, immediately turn off the screen without fading
        if (current <= 1) {
            setBrightness(0, false);
            turnOffDisplay();
        } else {
            int step = current / FADE_OUT_STEPS;

            // Ensure brightness doesn't go below 0
            for (int b = current; b >= 0; b -= step) {
                if (b < 0) b = 0;
                setBrightness(b, false);
                delay(FADE_OUT_DELAY);
            }

            turnOffDisplay();
        }
    }
}

void sleepModeOn() {
    isSleeping = true;
    setCpuFrequencyMhz(80);

    int current = bruceConfig.bright;

    // If brightness is 1% or lower, immediately turn off the screen without fading
    if (current <= 1) {
        setBrightness(0, false);
        turnOffDisplay();
    } else {
        int half_brightness = current / 2;  // Start fading from 50% of the current brightness
        int step = half_brightness / FADE_OUT_STEPS;

        // Ensure brightness doesn't go below 0
        for (int b = half_brightness; b >= 0; b -= step) {
            if (b < 0) b = 0;
            setBrightness(b, false);
            delay(FADE_OUT_DELAY);
        }

        turnOffDisplay();
    }

    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
    delay(200);
}

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
