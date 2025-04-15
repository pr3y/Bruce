#include <Arduino.h>
#include <FS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <SD.h>
#include <globals.h>

// Jamming modes
enum JamMode { BASIC, ENHANCED_BASIC, SWEEP, RANDOM, EMPTY };

// Control structure to hold jammer state
struct JammerState {
    int current_freq_idx = 3; // Start with 38kHz (most common)
    JamMode currentMode = BASIC;
    bool jamming_active = true;
    uint8_t jamDensity = 5; // Controls jamming intensity (1-10)
    uint32_t lastJamTime = 0;
    uint32_t last_update = 0;

    // Enhanced timing controls
    uint16_t markTiming = 12;  // Mark timing in microseconds
    uint16_t spaceTiming = 12; // Space timing in microseconds
    uint8_t dutyCycle = 50;    // Duty cycle percentage

    // Sweep mode controls
    uint16_t minTiming = 8;    // Min timing for sweep
    uint16_t maxTiming = 70;   // Max timing for sweep
    int8_t sweepDirection = 1; // Direction of sweep
    uint8_t sweepSpeed = 1;    // How fast to sweep

    // UI Control
    uint8_t settingIndex = 0; // 0=status, 1=frequency, 2=mode, 3+=mode specific settings
    uint8_t maxSettings = 4;  // Number of settings available (changes based on mode)
    bool selPressHandled = false;
    bool redraw = true;
    uint32_t lastUIUpdate = 0; // For blinking and UI updates

    // Statistics
    uint32_t jamCount = 0;  // Count of jam signals sent
    uint32_t startTime = 0; // When jamming started
    uint32_t runtime = 0;

    // Jamming patterns
    uint16_t basicPattern[20];               // For basic mode
    uint16_t emptyPattern[4] = {1, 1, 1, 1}; // For empty mode
    uint16_t randomPattern[30];              // For random mode
};

// Function prototypes
void initJammerState(JammerState &state);
void setupJammer(IRsend &irsend);
void renderJammerUI(JammerState &state);
void handleJammerInput(JammerState &state);
void performJamming(JammerState &state, IRsend &irsend);
void performBasicJamming(JammerState &state, IRsend &irsend);
void performEnhancedBasicJamming(JammerState &state, IRsend &irsend);
void performSweepJamming(JammerState &state, IRsend &irsend);
void performRandomJamming(JammerState &state, IRsend &irsend);
void performEmptyJamming(JammerState &state, IRsend &irsend);
void updatePatterns(JammerState &state);
void cleanupJammer(IRsend &irsend);
void updateMaxSettings(JammerState &state);
void updateStats(JammerState &state);
void displayStats(JammerState &state);
void startIrJammer();
uint16_t getFrequency(uint8_t index);
const char *getModeName(uint8_t index);
void handleSettingChange(JammerState &state, bool nextPressed, bool prevPressed);
