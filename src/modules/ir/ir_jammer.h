/**
 * IR Jammer Header File
 * Defines the structure and functions for IR signal jamming operations
 * on ESP8266-based hardware with IRremoteESP8266 library.
 */

#include <Arduino.h>
#include <FS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRutils.h>
#include <SD.h>
#include <globals.h>

// Jamming modes available to the user
enum JamMode {
    BASIC,          // Simple fixed-timing jamming
    ENHANCED_BASIC, // Advanced jamming with separate mark/space control
    SWEEP,          // Continuously varying timing values within a range
    RANDOM,         // Randomized patterns and frequencies
    EMPTY           // Minimal IR packets to maximize disruption
};

// Main control structure that holds the complete state of the jammer
struct JammerState {
    int current_freq_idx = 3;    // Index into IR_FREQUENCIES array (starts with 38kHz - most common)
    JamMode currentMode = BASIC; // Current jamming algorithm
    bool jamming_active = true;  // Indicates if actively jamming
    uint8_t jamDensity = 5;      // Controls jamming intensity/frequency (1-10)
    uint32_t lastJamTime = 0;    // Timestamp of last jam signal
    uint32_t last_update = 0;    // Timestamp for timing control

    // Timing parameters for signal generation
    uint16_t markTiming = 12;  // Mark (ON) pulse duration in microseconds
    uint16_t spaceTiming = 12; // Space (OFF) interval in microseconds
    uint8_t dutyCycle = 50;    // Duty cycle percentage (ratio of ON to total period)

    // Parameters specific to sweep mode
    uint16_t minTiming = 8;    // Minimum timing value for sweep mode (microseconds)
    uint16_t maxTiming = 70;   // Maximum timing value for sweep mode (microseconds)
    int8_t sweepDirection = 1; // Direction of timing change (1=increasing, -1=decreasing)
    uint8_t sweepSpeed = 1;    // Step size for timing changes in sweep mode

    // UI state management
    uint8_t settingIndex = 0;     // Currently selected setting (0=status, 1=frequency, etc.)
    uint8_t maxSettings = 4;      // Total number of settings available in current mode
    bool selPressHandled = false; // Prevents multiple triggers on a single button press
    bool redraw = true;           // Flag to indicate UI needs complete redraw
    uint32_t lastUIUpdate = 0;    // For controlling UI update frequency

    // Performance statistics
    uint32_t jamCount = 0;  // Total number of jamming signals sent
    uint32_t startTime = 0; // Timestamp when jamming began
    uint32_t runtime = 0;   // Total running time in seconds

    // Jamming pattern arrays for different modes
    uint16_t basicPattern[20];               // Pattern for basic/enhanced modes (alternating mark/space)
    uint16_t emptyPattern[4] = {1, 1, 1, 1}; // Minimal pattern for maximum disruption
    uint16_t randomPattern[30];              // Buffer for dynamically generated random patterns
};

// Function prototypes

/**
 * Initialize the jammer state with default values
 * @param state Reference to the JammerState structure to initialize
 */
void initJammerState(JammerState &state);

/**
 * Configure hardware for IR transmission
 * @param irsend Reference to IRsend object that controls the IR LED
 */
void setupJammer(IRsend &irsend);

/**
 * Display the jammer interface on the device screen
 * @param state Current jammer state to display
 */
void renderJammerUI(JammerState &state);

/**
 * Process button presses and update jammer settings
 * @param state Reference to jammer state to be modified based on input
 */
void handleJammerInput(JammerState &state);

/**
 * Execute the currently selected jamming mode
 * @param state Current jammer configuration
 * @param irsend IR transmitter interface
 */
void performJamming(JammerState &state, IRsend &irsend);

/**
 * Implement basic jamming mode with fixed timing
 * @param state Current jammer configuration
 * @param irsend IR transmitter interface
 */
void performBasicJamming(JammerState &state, IRsend &irsend);

/**
 * Implement enhanced jamming with independent mark/space timing
 * @param state Current jammer configuration
 * @param irsend IR transmitter interface
 */
void performEnhancedBasicJamming(JammerState &state, IRsend &irsend);

/**
 * Implement jamming with continuously varying timing values
 * @param state Current jammer configuration
 * @param irsend IR transmitter interface
 */
void performSweepJamming(JammerState &state, IRsend &irsend);

/**
 * Implement jamming with randomized patterns
 * @param state Current jammer configuration
 * @param irsend IR transmitter interface
 */
void performRandomJamming(JammerState &state, IRsend &irsend);

/**
 * Implement jamming with minimal empty packets
 * @param state Current jammer configuration
 * @param irsend IR transmitter interface
 */
void performEmptyJamming(JammerState &state, IRsend &irsend);

/**
 * Update pattern arrays based on current settings
 * @param state Current jammer configuration
 */
void updatePatterns(JammerState &state);

/**
 * Clean up resources when exiting jammer mode
 * @param irsend IR transmitter interface to reset
 */
void cleanupJammer(IRsend &irsend);

/**
 * Update the maxSettings value based on the current mode
 * @param state Current jammer configuration to update
 */
void updateMaxSettings(JammerState &state);

/**
 * Update jamming statistics (count, runtime)
 * @param state Current jammer state to update
 */
void updateStats(JammerState &state);

/**
 * Show performance statistics on the display
 * @param state Current jammer state with statistics
 */
void displayStats(JammerState &state);

/**
 * Main entry point for the IR jammer functionality
 */
void startIrJammer();

/**
 * Get the frequency in Hz from the frequency index
 * @param index Index into the IR_FREQUENCIES array
 * @return Frequency value in Hz
 */
uint16_t getFrequency(uint8_t index);

/**
 * Get the human-readable name for a jamming mode
 * @param index Index of the mode (from JamMode enum)
 * @return String containing the mode name
 */
const char *getModeName(uint8_t index);

/**
 * Process changes to the currently selected setting
 * @param state Current jammer state to modify
 * @param nextPressed Whether the "next" button was pressed
 * @param prevPressed Whether the "previous" button was pressed
 */
void handleSettingChange(JammerState &state, bool nextPressed, bool prevPressed);
