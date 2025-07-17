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

// Forward declarations
static void fadeScreen(int startLevel, int endLevel);
static void fadeLed(int startLevel, int endLevel);

void updatePowerSave() {
    if (!isFading) return;

    unsigned long elapsed = millis() - fadeStartTime;
    if (elapsed >= (unsigned long)fadeDuration) {
        // final state
        fadeScreen(fadeEndDisp, fadeEndDisp);
#ifdef HAS_RGB_LED
        fadeLed(fadeEndLed, fadeEndLed);
#endif

        if (fadingIn) {
#ifdef HAS_RGB_LED
            ledSetup();
#endif
            enableCore0WDT();
            enableCore1WDT();
            enableLoopWDT();
            feedLoopWDT();
        } else {
            if (fadeEndDisp == 0) turnOffDisplay();
        }
        isFading = false;
    } else {
        float t = (float)elapsed / fadeDuration;
        int currDisp = fadeStartDisp + int((fadeEndDisp - fadeStartDisp) * t);
        int currLed = fadeStartLed + int((fadeEndLed - fadeStartLed) * t);
        fadeScreen(currDisp, currDisp);
#ifdef HAS_RGB_LED
        fadeLed(currLed, currLed);
#endif
    }
}

// Single fade routines for screen and LED
static void fadeScreen(int startLevel, int endLevel) {
    setBrightness(startLevel, false);
    // schedule next step via updatePowerSave
}

static void fadeLed(int startLevel, int endLevel) {
#ifdef HAS_RGB_LED
    setLedBrightness(startLevel);
#endif
    // schedule next step via updatePowerSave
}

void checkPowerSaveTime() {
    if (bruceConfig.dimmerSet == 0 || isFading) return;

    unsigned long elapsed = millis() - previousMillis;
    int dimDisp = bruceConfig.bright / 3;
    int dimLed = bruceConfig.ledBright / 3;
    unsigned long threshold = (unsigned long)bruceConfig.dimmerSet * 1000;

    // fade down into dim‑mode
    if (elapsed >= threshold && !dimmer && !isSleeping) {
        dimmer = true;
        if (!bruceConfig.smoothSleep) {
            setBrightness(dimDisp, false);
#ifdef HAS_RGB_LED
            setLedBrightness(dimLed);
#endif
        } else {
            fadeStartTime = millis();
            fadeDuration = abs(bruceConfig.bright - dimDisp) * FADE_STEP_DELAY;
            fadingIn = false;
            fadeStartDisp = bruceConfig.bright;
            fadeEndDisp = dimDisp;
            fadeStartLed = bruceConfig.ledBright;
            fadeEndLed = dimLed;
            isFading = true;
        }
    }
    // fade‑out from dim→off
    else if (elapsed >= (threshold + SCREEN_OFF_DELAY) && !isScreenOff && !isSleeping) {
        isScreenOff = true;
        if (!bruceConfig.smoothSleep) {
            setBrightness(0, false);
#ifdef HAS_RGB_LED
            setLedBrightness(0);
#endif
            turnOffDisplay();
        } else {
            fadeStartTime = millis();
            fadeDuration = abs(dimDisp - 0) * FADE_STEP_DELAY;
            fadingIn = false;
            fadeStartDisp = dimDisp;
            fadeEndDisp = 0;
            fadeStartLed = dimLed;
            fadeEndLed = 0;
            isFading = true;
        }
    }
}

void sleepModeOn() {
    isSleeping = true;
    setCpuFrequencyMhz(80);

    int dimDisp = bruceConfig.bright / 3;
    int dimLed = bruceConfig.ledBright / 3;

    if (!bruceConfig.smoothSleep) {
        setBrightness(0, false);
#ifdef HAS_RGB_LED
        setLedBrightness(0);
#endif
        turnOffDisplay();
    } else {
        fadeStartTime = millis();
        fadeDuration = abs(dimDisp - 0) * FADE_STEP_DELAY;
        fadingIn = false;
        fadeStartDisp = dimDisp;
        fadeEndDisp = 0;
        fadeStartLed = dimLed;
        fadeEndLed = 0;
        isFading = true;
    }

    panelSleep(true);
    disableCore0WDT();
    disableCore1WDT();
    disableLoopWDT();
}

void sleepModeOff(bool fade) {
    bool wasDimOnly = (dimmer && !isScreenOff);

    isSleeping = false;
    setCpuFrequencyMhz(240);
    panelSleep(false);
    dimmer = false;
    isScreenOff = false;

    if (!fade || !bruceConfig.smoothSleep) {
        setBrightness(bruceConfig.bright, false);
#ifdef HAS_RGB_LED
        setLedBrightness(bruceConfig.ledBright);
        ledSetup();
#endif
        enableCore0WDT();
        enableCore1WDT();
        enableLoopWDT();
        feedLoopWDT();
    } else {
        int dimDisp = bruceConfig.bright / 3;
        int dimLed = bruceConfig.ledBright / 3;
        int startDisp = wasDimOnly ? dimDisp : 0;
        int startLed = wasDimOnly ? dimLed : 0;

        fadeStartTime = millis();
        fadeDuration = abs(bruceConfig.bright - startDisp) * FADE_STEP_DELAY;
        fadingIn = true;
        fadeStartDisp = startDisp;
        fadeEndDisp = bruceConfig.bright;
        fadeStartLed = startLed;
        fadeEndLed = bruceConfig.ledBright;
        isFading = true;
    }
}
