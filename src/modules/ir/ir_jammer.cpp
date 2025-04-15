/**
 * Enhanced IR Jammer with Advanced Basic Mode
 * Based on the original by Bruce Lee/EA7KDO with significant improvements
 */

#include "ir_jammer.h"
#include "TV-B-Gone.h" // for checkIrTxPin()
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/settings.h"
#include "core/utils.h"

// Common IR remote control frequencies in Hz
const uint16_t IR_FREQUENCIES[] = {30000, 33000, 36000, 38000, 40000, 42000, 56000};
const int NUM_FREQS = sizeof(IR_FREQUENCIES) / sizeof(IR_FREQUENCIES[0]);

const char *IR_MODE_NAMES[] = {"BASIC", "ENH. BASIC", "SWEEP", "RANDOM", "EMPTY"};

// Initialize the jammer state
void initJammerState(JammerState &state) {
    // Initialize timing values with safe defaults
    state.markTiming = 12;
    state.spaceTiming = 12;
    state.minTiming = 8;
    state.maxTiming = 70;
    state.dutyCycle = 50;
    state.jamDensity = 5;
    state.sweepSpeed = 1;
    state.sweepDirection = 1;
    state.current_freq_idx = 3; // Start with 38kHz (most common)

    // Initialize basic pattern
    for (int i = 0; i < 20; i += 2) {
        state.basicPattern[i] = state.markTiming;
        state.basicPattern[i + 1] = state.spaceTiming;
    }

    // Initialize random pattern
    randomSeed(millis());
    for (int i = 0; i < 30; i++) { state.randomPattern[i] = random(10, 1000); }

    // Reset stats
    state.jamCount = 0;
    state.startTime = millis();

    // Update settings based on mode
    updateMaxSettings(state);
}

void adjustModeSpecificSetting(JammerState &state, uint8_t settingIndex, int adjustment) {
    switch (state.currentMode) {
        case BASIC:
            if (settingIndex == 3) {
                state.markTiming = constrain(state.markTiming + adjustment, 5, 100);
                state.spaceTiming = state.markTiming; // Keep in sync for basic mode
            }
            break;

        case ENHANCED_BASIC:
            switch (settingIndex) {
                case 3: state.markTiming = constrain(state.markTiming + adjustment, 5, 100); break;
                case 4: state.spaceTiming = constrain(state.spaceTiming + adjustment, 1, 100); break;
                case 5: state.jamDensity = constrain(state.jamDensity + adjustment, 1, 20); break;
            }
            break;

        case SWEEP:
            switch (settingIndex) {
                case 3:
                    state.minTiming = constrain(state.minTiming + adjustment, 1, state.maxTiming - 5);
                    break;
                case 4:
                    state.maxTiming = constrain(state.maxTiming + adjustment, state.minTiming + 5, 150);
                    break;
                case 5: state.sweepSpeed = constrain(state.sweepSpeed + adjustment, 1, 10); break;
                case 6: state.jamDensity = constrain(state.jamDensity + adjustment, 1, 20); break;
            }
            break;

        case RANDOM:
        case EMPTY:
            if (settingIndex == 3) { state.jamDensity = constrain(state.jamDensity + adjustment, 1, 20); }
            break;
    }
}

// Rendering mode-specific settings
void renderModeSettings(JammerState &state) {
    switch (state.currentMode) {
        case BASIC:
            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 3) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("TIMING: ");
            tft.print(state.markTiming);
            tft.println(" us    ");
            break;

        case ENHANCED_BASIC:
            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 3) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("MARK: ");
            tft.print(state.markTiming);
            tft.println(" us    ");

            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 4) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("SPACE: ");
            tft.print(state.spaceTiming);
            tft.println(" us    ");

            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 5) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("POWER: ");
            tft.println(state.jamDensity);
            break;

        case SWEEP:
            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 3) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("MIN: ");
            tft.print(state.minTiming);
            tft.println(" us    ");

            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 4) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("MAX: ");
            tft.print(state.maxTiming);
            tft.println(" us    ");

            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 5) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("SPEED: ");
            tft.println(state.sweepSpeed);

            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 6) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("POWER: ");
            tft.println(state.jamDensity);
            break;

        case RANDOM:
        case EMPTY:
            tft.setCursor(10, tft.getCursorY() + 5);
            tft.setTextColor(
                (state.settingIndex == 3) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor
            );
            tft.print("POWER: ");
            tft.println(state.jamDensity);
            break;
    }
}

// Display statistics
void displayStats(JammerState &state) {
    tft.setTextSize(FP);
    tft.setCursor(150, 85);
    tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);

    uint32_t runtime = (millis() - state.startTime) / 1000; // seconds

    if (state.jamming_active) { state.runtime = runtime; }

    float jps = state.runtime > 0 ? (float)state.jamCount / state.runtime : 0;

    tft.print("Jams : ");
    tft.println(state.jamCount);
    tft.setCursor(150, tft.getCursorY() + 5);
    tft.printf("Time : %02d:%02d", state.runtime / 60, state.runtime % 60);
    tft.setCursor(150, tft.getCursorY() + 12);
    tft.printf("J/s  : %.1f", jps);
}

// Handle input for jammer controls
void handleJammerInput(JammerState &state) {
    // Handle Select press to cycle through settings
    if (check(SelPress)) {
        if (!state.selPressHandled) {
            // Cycle to next setting
            state.settingIndex = (state.settingIndex + 1) % state.maxSettings;
            state.redraw = true;
            state.selPressHandled = true;
            delay(150); // Debounce
        }
    } else {
        state.selPressHandled = false;
    }

    // Handle Next/Prev to change the selected setting
    handleSettingChange(state, check(NextPress), check(PrevPress));
}

// Handle changing setting values
void handleSettingChange(JammerState &state, bool nextPressed, bool prevPressed) {
    if (!nextPressed && !prevPressed) return;

    // Determine adjustment direction
    int adjustment = nextPressed ? 1 : -1;

    switch (state.settingIndex) {
        case 0: // Status
            state.jamming_active = !state.jamming_active;
            if (state.jamming_active) {
                state.startTime = millis(); // Reset timer when starting
                state.jamCount = 0;
            }
            break;

        case 1: // Frequency
            state.current_freq_idx = (state.current_freq_idx + adjustment + NUM_FREQS) % NUM_FREQS;
            break;

        case 2: // Mode
            state.currentMode = (JamMode)((state.currentMode + adjustment + 5) % 5);
            updateMaxSettings(state);
            updatePatterns(state);
            break;

        // Mode-specific settings
        default: adjustModeSpecificSetting(state, state.settingIndex, adjustment); break;
    }

    state.redraw = true;
    updatePatterns(state);
    delay(100);
}

// Update pattern arrays based on current state
void updatePatterns(JammerState &state) {
    // Update basic pattern
    for (int i = 0; i < 20; i += 2) {
        state.basicPattern[i] = state.markTiming;
        state.basicPattern[i + 1] = state.spaceTiming;
    }
}

// Update maximum settings based on current mode
void updateMaxSettings(JammerState &state) {
    switch (state.currentMode) {
        case BASIC:
            state.maxSettings = 4; // status, freq, mode, timing
            break;
        case ENHANCED_BASIC:
            state.maxSettings = 6; // status, freq, mode, mark, space, duty
            break;
        case SWEEP:
            state.maxSettings = 7; // status, freq, mode, min, max, speed, power
            break;
        case RANDOM:
        case EMPTY:
            state.maxSettings = 4; // status, freq, mode, power
            break;
    }
}

// Setup the jammer hardware
void setupJammer(IRsend &irsend) {
    checkIrTxPin();
    irsend.begin();
    pinMode(bruceConfig.irTx, OUTPUT);
    drawMainBorder();
}

void renderJammerUI(JammerState &state) {
    // Only update UI if needed or after minimum interval to reduce flicker
    uint32_t currentMillis = millis();
    if (!state.redraw && currentMillis - state.lastUIUpdate < 300) return;

    bool blinkState = (currentMillis % 600 < 300);
    state.lastUIUpdate = currentMillis;

    // Only do full redraw when needed
    if (state.redraw) {
        tft.fillRect(10, 35, tftWidth - 20, tftHeight - 55, bruceConfig.bgColor);
        tft.setCursor(10, 35);
        tft.setTextSize(FM);
        tft.setTextColor(TFT_CYAN, bruceConfig.bgColor);
        tft.println("IR Jammer");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    }

    // Draw settings with appropriate highlight for current selection
    tft.setCursor(10, 55);
    tft.setTextSize(FP);
    tft.setTextColor((state.settingIndex == 0) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor);
    tft.print("STATUS: ");
    tft.setTextColor(state.jamming_active ? TFT_RED : TFT_WHITE, bruceConfig.bgColor);
    tft.println(state.jamming_active ? "ACTIVE " : "PAUSED ");

    // Visual indicator for jamming
    if (state.jamming_active && blinkState) {
        tft.setCursor(155, 55);
        tft.setTextColor(TFT_MAGENTA, bruceConfig.bgColor);
        tft.print("*");
    } else {
        tft.setCursor(155, 55);
        tft.print(" ");
    }

    tft.setCursor(10, tft.getCursorY() + 15);
    tft.setTextColor((state.settingIndex == 1) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor);
    tft.print("FREQ: ");
    tft.print(getFrequency(state.current_freq_idx) / 1000);
    tft.println(" kHz    ");

    tft.setCursor(10, tft.getCursorY() + 5);
    tft.setTextColor((state.settingIndex == 2) ? TFT_YELLOW : bruceConfig.priColor, bruceConfig.bgColor);
    tft.print("MODE: ");
    tft.println(getModeName(state.currentMode));

    // Mode-specific settings renderer
    renderModeSettings(state);

    // Show small stats at bottom
    displayStats(state);

    // Instructions
    tft.setCursor(10, 150);
    tft.setTextSize(FP);
    tft.setTextColor(TFT_BLUE, bruceConfig.bgColor);
    tft.println("[SEL] Change Set. | [NEXT/PREV] Adjust Val. ");
    tft.setTextColor(TFT_RED, bruceConfig.bgColor);
    tft.setCursor(250, 30);
    tft.println("[ESC] Exit");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    state.redraw = false;
}


// Update stats
void updateStats(JammerState &state) {
    state.jamCount++;

    // Only update UI every 10 jams to reduce flicker
    if (state.jamCount % 10 == 0) { state.redraw = true; }
}

// Perform jamming based on current mode
void performJamming(JammerState &state, IRsend &irsend) {
    if (!state.jamming_active) return;

    uint32_t currentMillis = millis();

    // Control jam signal frequency based on density setting
    if (currentMillis - state.lastJamTime >= (10 / state.jamDensity)) {
        state.lastJamTime = currentMillis;

        // Call appropriate jamming function based on mode
        switch (state.currentMode) {
            case BASIC: performBasicJamming(state, irsend); break;
            case ENHANCED_BASIC: performEnhancedBasicJamming(state, irsend); break;
            case SWEEP: performSweepJamming(state, irsend); break;
            case RANDOM: performRandomJamming(state, irsend); break;
            case EMPTY: performEmptyJamming(state, irsend); break;
        }

        updateStats(state);
    }
}

// Perform original basic jamming mode
void performBasicJamming(JammerState &state, IRsend &irsend) {
    uint32_t currentMillis = millis();

    if (currentMillis - state.last_update > 20) {
        // Direct LED control approach - simple square wave
        for (int i = 0; i < 50 * state.jamDensity; i++) {
            digitalWrite(bruceConfig.irTx, HIGH);
            delayMicroseconds(state.markTiming);
            digitalWrite(bruceConfig.irTx, LOW);
            delayMicroseconds(state.markTiming);
        }

        // Also use the IR library approach
        irsend.sendRaw(state.basicPattern, 20, getFrequency(state.current_freq_idx));
        state.last_update = currentMillis;
    }
}

// Perform enhanced basic jamming mode with independent mark/space control
void performEnhancedBasicJamming(JammerState &state, IRsend &irsend) {
    uint32_t currentMillis = millis();

    if (currentMillis - state.last_update > 20) {
        // Direct LED control approach with independent mark/space timing
        for (int i = 0; i < 25 * state.jamDensity; i++) {
            digitalWrite(bruceConfig.irTx, HIGH);
            delayMicroseconds(state.markTiming);
            digitalWrite(bruceConfig.irTx, LOW);
            delayMicroseconds(state.spaceTiming);
        }

        // Also use the IR library approach
        irsend.sendRaw(state.basicPattern, 20, getFrequency(state.current_freq_idx));
        state.last_update = currentMillis;
    }
}

// Perform sweeping jamming mode
void performSweepJamming(JammerState &state, IRsend &irsend) {
    uint32_t currentMillis = millis();

    if (currentMillis - state.last_update > 30) {
        // Update timing values based on sweep
        state.markTiming += state.sweepDirection * state.sweepSpeed;

        // Change direction if we hit the limits
        if (state.markTiming > state.maxTiming || state.markTiming < state.minTiming) {
            state.sweepDirection *= -1; // Reverse direction
            state.markTiming = constrain(state.markTiming, state.minTiming, state.maxTiming);
        }

        // Match space timing to mark for simplicity
        state.spaceTiming = state.markTiming;

        // Update pattern
        updatePatterns(state);

        // Direct LED control approach
        for (int i = 0; i < 20 * state.jamDensity; i++) {
            digitalWrite(bruceConfig.irTx, HIGH);
            delayMicroseconds(state.markTiming);
            digitalWrite(bruceConfig.irTx, LOW);
            delayMicroseconds(state.markTiming);
        }

        // Also use the IR library approach
        irsend.sendRaw(state.basicPattern, 20, getFrequency(state.current_freq_idx));
        state.last_update = currentMillis;
    }
}

// Perform random jamming mode
void performRandomJamming(JammerState &state, IRsend &irsend) {
    uint32_t currentMillis = millis();

    if (currentMillis - state.last_update > 100) {
        // Update random pattern
        for (int i = 0; i < 30; i++) { state.randomPattern[i] = random(5, 1000); }

        // Send random pattern
        for (int i = 0; i < state.jamDensity / 2 + 1; i++) {
            // Randomly change frequency sometimes
            if (random(10) < 3) { state.current_freq_idx = random(NUM_FREQS); }
            irsend.sendRaw(state.randomPattern, 30, getFrequency(state.current_freq_idx));
        }

        state.last_update = currentMillis;
    }
}

// Perform empty packet jamming mode
void performEmptyJamming(JammerState &state, IRsend &irsend) {
    uint32_t currentMillis = millis();

    if (currentMillis - state.last_update > 50) {
        // Send empty packets at different frequencies
        for (int i = 0; i < state.jamDensity; i++) {
            irsend.sendRaw(state.emptyPattern, 4, getFrequency(state.current_freq_idx));
        }

        // Cycle through frequencies
        if (random(5) < 2) { state.current_freq_idx = (state.current_freq_idx + 1) % NUM_FREQS; }

        state.last_update = currentMillis;
    }
}


// Clean up when exiting the jammer
void cleanupJammer(IRsend &irsend) {
    digitalWrite(bruceConfig.irTx, LOW);
    displayRedStripe("IR Jamming Stopped");
    delay(1000);
}

// Main IR jammer function
void startIrJammer() {
    IRsend irsend(bruceConfig.irTx);
    JammerState state;

    setupJammer(irsend);
    initJammerState(state);

    while (!check(EscPress)) {
        renderJammerUI(state);
        performJamming(state, irsend);
        handleJammerInput(state);

        // Small delay to prevent button handling from being too fast
        delay(5);
    }

    cleanupJammer(irsend);
}

// Get frequency from PROGMEM
uint16_t getFrequency(uint8_t index) { return pgm_read_word(&IR_FREQUENCIES[index]); }

// Get mode name from PROGMEM
const char *getModeName(uint8_t index) { return IR_MODE_NAMES[index]; }
