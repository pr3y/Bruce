#if defined(M5STACK)
// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef __M5_Log_Class_H__
#define __M5_Log_Class_H__

#include "m5unified_common.h"

#if !defined ( M5UNIFIED_PC_BUILD )
#include <esp_log.h>
#endif

#include <stdarg.h>
#include <functional>

#include <M5GFX.h>

/// Output log with source info.
#ifndef M5UNIFIED_LOG_FORMAT
#define M5UNIFIED_LOG_FORMAT(letter, format) "[%6u][" #letter "][%s:%u] %s(): " format, m5gfx::millis(), m5::Log_Class::pathToFileName(__FILE__), __LINE__, __FUNCTION__
#endif

/// Output Error log with source info.
#define M5_LOGE(format, ...) M5.Log(ESP_LOG_ERROR  , M5UNIFIED_LOG_FORMAT(E, format), ##__VA_ARGS__)

/// Output Warn log with source info.
#define M5_LOGW(format, ...) M5.Log(ESP_LOG_WARN   , M5UNIFIED_LOG_FORMAT(W, format), ##__VA_ARGS__)

/// Output Info log with source info.
#define M5_LOGI(format, ...) M5.Log(ESP_LOG_INFO   , M5UNIFIED_LOG_FORMAT(I, format), ##__VA_ARGS__)

/// Output Debug log with source info.
#define M5_LOGD(format, ...) M5.Log(ESP_LOG_DEBUG  , M5UNIFIED_LOG_FORMAT(D, format), ##__VA_ARGS__)

/// Output Verbose log with source info.
#define M5_LOGV(format, ...) M5.Log(ESP_LOG_VERBOSE, M5UNIFIED_LOG_FORMAT(V, format), ##__VA_ARGS__)

namespace m5
{
  enum log_target_t : uint8_t
  {
    log_target_serial,
    log_target_display,
    log_target_callback,
    log_target_max,
  };

  class Log_Class
  {
  public:
    /// Output log.
    void operator() (esp_log_level_t level, const char* format, ...);

    /// Output text in a specified format.
    /// @attention Output regardless of log level setting.
    void printf(const char* format, ...);

    /// Output text.
    /// @attention Output regardless of log level setting.
    void print(const char* string) { return printf("%s", string); }

    /// Output text with line feeds.
    /// @attention Output regardless of log level setting.
    void println(const char* string) { return printf("%s\n", string); }

    /// Output line feeds.
    /// @attention Output regardless of log level setting.
    void println(void) { return printf(str_crlf); }

    /// Set whether or not to change the color for each log level.
    void setEnableColor(log_target_t target, bool enable) { if (target < log_target_max) { _use_color[target] = enable; } }

    /// Get whether or not to change the color for each log level.
    bool getEnableColor(log_target_t target) const { return _use_color[target]; }

    /// Set log level.
    void setLogLevel(log_target_t target, esp_log_level_t level) { if (target < log_target_max) { _log_level[target] = level; update_level(); } }

    /// Get log level for serial output.
    esp_log_level_t getLogLevel(log_target_t target) const { return _log_level[target]; }

    /// Set the text to be added to the end of the log.
    void setSuffix(log_target_t target, const char* suffix) { if (target < log_target_max) { _suffix[target] = suffix; } }

    /// set logging callback function / functor .
    /// @param function Pointer to a user-defined function that takes three arguments: esp_log_level_t , bool, const char*.
    void setCallback(std::function<void(esp_log_level_t log_level, bool use_color, const char* log_text)> function) { _callback = function; };

    /// Set the display to show logs.
    /// @param target target display.
    void setDisplay(M5GFX* target);

    /// Set the display to show logs.
    /// @param target target display.
    void setDisplay(M5GFX& target) { setDisplay(&target); }


    void dump(const void* addr, uint32_t len, esp_log_level_t level = esp_log_level_t::ESP_LOG_NONE);

    /// not for use.
    static const char* pathToFileName(const char * path);

  private:
    M5GFX* _display = nullptr;

    static constexpr const char str_crlf[3] = "\r\n";
    static constexpr const char *str_lf = &str_crlf[1];

    void output(esp_log_level_t level, bool suffix, const char* __restrict format, va_list arg);
    void update_level(void);

    std::function<void(esp_log_level_t log_level, bool use_color, const char* log_text)> _callback;

#if defined ( CORE_DEBUG_LEVEL )
    esp_log_level_t _level_maximum  = (esp_log_level_t)CORE_DEBUG_LEVEL;
#elif defined ( CONFIG_LOG_DEFAULT_LEVEL )
    esp_log_level_t _level_maximum  = (esp_log_level_t)CONFIG_LOG_DEFAULT_LEVEL;
#else
    esp_log_level_t _level_maximum = esp_log_level_t::ESP_LOG_VERBOSE;
#endif
    esp_log_level_t _log_level[log_target_max]   = { _level_maximum, _level_maximum, _level_maximum };

    const char* _suffix[log_target_max] = { str_lf, str_lf, str_crlf };

    bool _use_color[log_target_max] = { true, true, true };
  };
}
#endif
#endif