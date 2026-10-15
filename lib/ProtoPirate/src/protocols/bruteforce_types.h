#pragma once

#include <furi.h>
#include <flipper_format/flipper_format.h>

#define BRUTEFORCE_STATUS_IDLE      0
#define BRUTEFORCE_STATUS_RUNNING   1
#define BRUTEFORCE_STATUS_FOUND     2
#define BRUTEFORCE_STATUS_NOT_FOUND 3
#define BRUTEFORCE_STATUS_CANCELLED 4
typedef struct BruteForceState {
    void (*on_done)(void* context);
    void* on_done_ctx;
    volatile uint32_t progress_current;
    volatile uint32_t progress_total;
    uint32_t decrypted_serial;
    uint32_t decrypted_seed;
    uint32_t decrypted_counter;
    uint32_t key1_low;
    uint32_t key1_high;
    uint16_t key2_low;
    uint16_t decrypted_crc;
    volatile uint8_t cancel;
    volatile uint8_t status;
    uint8_t decrypted_button;
    uint8_t decrypted_type;
} BruteForceState;
