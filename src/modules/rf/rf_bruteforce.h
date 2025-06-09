#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>

struct SavedFrequency {
    uint32_t code;
    float frequency;
    String protocol;
    unsigned long timestamp;
    String custom_name;
};

// Global variables
extern float brute_frequency;
extern String brute_protocol;
extern int brute_repeats;
extern bool is_paused;
extern uint32_t current_code;
extern std::vector<SavedFrequency> saved_frequencies;

// Function declarations
void rf_brute_frequency();
void rf_brute_protocol();
void rf_brute_repeats();
bool rf_brute_start();
void rf_bruteforce();
void rf_save_frequency();
void rf_load_frequencies();
void rf_replay_frequency();
void rf_step_forward();
void rf_step_backward();
void rf_toggle_pause();
