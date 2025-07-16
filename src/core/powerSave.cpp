// powerSave.cpp
#include "powerSave.h"
#include "led_control.h"
#include "settings.h"

// How long to wait after dimming before full sleep
#define SCREEN_OFF_DELAY 5000
// Fade step delay in ms
#define FADE_STEP_DELAY 5

// --- Non‑blocking fade state ---
static bool isFading = false;
static unsigned long fadeStartTime = 0;
static int fadeStartDisp, fadeStartLed;
static int fadeEndDisp, fadeEndLed;
static int fadeDuration;
static bool fadingIn;

// Forward declaration
static void fadeAll(int startDisp, int startLed, int endDisp, int endLed);

/**
 * @brief Call this every loop (or task) to step an in‑progress fade.
 */
void updatePowerSave() {
    if (!isFading) return;

    unsigned long elapsed = millis() - fadeStartTime;
    if (elapsed >= (unsigned long)fadeDuration) {
        // final state
        setBrightness(fadeEndDisp, false);
        setLedBrightness(fadeEndLed);

        if (fadingIn) {
            // After wake‑up fade completes:
            ledSetup();
            enableCore0WDT();
            enableCore1WDT();
            enableLoopWDT();
            feedLoopWDT();
        } else {
            // Only turn off display when we've faded all the way to zero
            if (fadeEndDisp == 0 && fadeEndLed == 0) { turnOffDisplay(); }
        }
        isFading = false;
    } else {
        // intermediate blend
        float t = (float)elapsed / fadeDuration;
        int d = fadeStartDisp + int((fadeEndDisp - fadeStartDisp) * t);
        int l = fadeStartLed + int((fadeEndLed - fadeStartLed) * t);
        setBrightness(d, false);
        setLedBrightness(l);
    }
}

/**
 * @brief Schedule a fade from (startDisp,startLed) → (endDisp,endLed).
 *        updatePowerSave() must be running to drive it.
 */
static void fadeAll(int startDisp, int startLed, int endDisp, int endLed) {
    int steps = max(abs(endDisp - startDisp), abs(endLed - startLed));
    if (steps == 0) {
        // no animation
        setBrightness(endDisp, false);
        setLedBrightness(endLed);
        if (!fadingIn) turnOffDisplay();
        isFading = false;
        return;
    }

    // init state
    isFading = true;
    fadingIn = (endDisp > startDisp || endLed > startLed);
    fadeStartTime = millis();
    fadeDuration = steps * FADE_STEP_DELAY;
    fadeStartDisp = startDisp;
    fadeStartLed = startLed;
    fadeEndDisp = endDisp;
    fadeEndLed = endLed;

    // immediately apply start level
    setBrightness(startDisp, false);
    setLedBrightness(startLed);
}

/**
 * @brief Called periodically to trigger dimming/fading.
 */
void checkPowerSaveTime() {
    if (bruceConfig.dimmerSet == 0) return;
    if (isFading) return;

    unsigned long elapsed = millis() - previousMillis;
    int dDisp = bruceConfig.bright / 3;
    int dLed = bruceConfig.ledBright / 3;
    unsigned long thresh = (unsigned long)bruceConfig.dimmerSet * 1000;

    // 1) fade down into dim‑mode
    if (elapsed >= thresh && !dimmer && !isSleeping) {
        dimmer = true;
        if (!bruceConfig.smoothSleep) {
            setBrightness(dDisp, false);
            setLedBrightness(dLed);
        } else {
            fadeAll(
                bruceConfig.bright,    // full start
                bruceConfig.ledBright, // full start
                dDisp,                 // dim end
                dLed                   // dim end
            );
        }
    }
    // 2) fade‑out from dim→off
    else if (elapsed >= (thresh + SCREEN_OFF_DELAY) && !isScreenOff && !isSleeping) {
        isScreenOff = true;
        if (!bruceConfig.smoothSleep) {
            setBrightness(0, false);
            setLedBrightness(0);
            turnOffDisplay();
        } else {
            fadeAll(dDisp, dLed, 0, 0);
        }
    }
}

/**
 * @brief Enter sleep: fade‑out then sleep.
 */
void sleepModeOn() {
    isSleeping = true;
    setCpuFrequencyMhz(80);

    int dDisp = bruceConfig.bright / 3;
    int dLed = bruceConfig.ledBright / 3;

    if (!bruceConfig.smoothSleep) {
        setBrightness(0, false);
        setLedBrightness(0);
        turnOffDisplay();
    } else {
        fadeAll(dDisp, dLed, 0, 0);
    }

    panelSleep(true);
    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
}

/**
 * @brief Exit sleep: wake panel, optionally fade‑in, then re‑enable WDT.
 */
void sleepModeOff(bool fade) {
    // Capture whether we were only dimmed (not fully off)
    bool wasDimOnly = (dimmer && !isScreenOff);

    // Exit “sleep” state
    isSleeping = false;
    setCpuFrequencyMhz(240);

    // Wake panel hardware (doesn't change brightness)
    panelSleep(false);

    // Clear flags now so checkPowerSaveTime can start a new cycle
    dimmer = false;
    isScreenOff = false;

    if (!fade || !bruceConfig.smoothSleep) {
        // Instant restore
        setBrightness(bruceConfig.bright, false);
        setLedBrightness(bruceConfig.ledBright);
        ledSetup();
        enableCore0WDT();
        enableCore1WDT();
        enableLoopWDT();
        feedLoopWDT();
    } else {
        // Compute the one‑third dim levels
        int dimDisp = bruceConfig.bright / 3;
        int dimLed = bruceConfig.ledBright / 3;

        // Start fade from dim level if we were only dimmed; otherwise from 0
        int startDisp = wasDimOnly ? dimDisp : 0;
        int startLed = wasDimOnly ? dimLed : 0;

        fadeAll(
            startDisp,            // current brightness
            startLed,             // current LED
            bruceConfig.bright,   // full display
            bruceConfig.ledBright // full LEDs
        );
    }
}
