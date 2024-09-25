#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if !defined ( ESP_PLATFORM )

#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#define M5UNIFIED_PC_BUILD
#elif __has_include(<SDL.h>)
#include <SDL.h>
#define M5UNIFIED_PC_BUILD
#endif

#if defined ( M5UNIFIED_PC_BUILD )

typedef int esp_err_t;
typedef int i2c_port_t;

#define ESP_OK          0
#define ESP_FAIL        -1

typedef enum {
  ESP_LOG_NONE,       /*!< No log output */
  ESP_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
  ESP_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
  ESP_LOG_INFO,       /*!< Information messages which describe normal flow of events */
  ESP_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
  ESP_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} esp_log_level_t;

typedef enum {
  I2S_NUM_0 = 0,
  I2S_NUM_MAX,
} i2s_port_t;

#endif
#endif
#endif