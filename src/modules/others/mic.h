/**
 * @file test.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-05-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <fft.h>
#include "driver/i2s.h"
#include "core/display.h"
#include "core/globals.h"

/* Mic */
void mic_init();
void mic_test();
void mic_test_one_task();

void DisplayMicro();

void new_mic_test();
void new_mic_test_fft();

