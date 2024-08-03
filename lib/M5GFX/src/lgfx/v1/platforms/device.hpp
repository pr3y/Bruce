/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#if defined (ESP_PLATFORM)

 #include <sdkconfig.h>

 #if defined (CONFIG_IDF_TARGET_ESP32C6)

  #include "esp32/Light_PWM.hpp"
  #include "esp32/Bus_SPI.hpp"
  #include "esp32/Bus_I2C.hpp"

 #elif defined (CONFIG_IDF_TARGET_ESP32C3)

  #include "esp32/Light_PWM.hpp"
  #include "esp32/Bus_SPI.hpp"
  #include "esp32/Bus_I2C.hpp"
  #include "esp32c3/Bus_Parallel8.hpp"

 #elif defined (CONFIG_IDF_TARGET_ESP32S2)

  #include "esp32/Light_PWM.hpp"
  #include "esp32/Bus_SPI.hpp"
  #include "esp32/Bus_I2C.hpp"
  #include "esp32s2/Bus_Parallel8.hpp"
  #include "esp32s2/Bus_Parallel16.hpp"

 #elif defined (CONFIG_IDF_TARGET_ESP32S3)

  #include "esp32/Light_PWM.hpp"
  #include "esp32/Bus_SPI.hpp"
  #include "esp32/Bus_I2C.hpp"
  #include "esp32s3/Bus_Parallel8.hpp"
  #include "esp32s3/Bus_Parallel16.hpp"

 #else

  #include "esp32/Light_PWM.hpp"
  #include "esp32/Bus_SPI.hpp"
  #include "esp32/Bus_I2C.hpp"
  #include "esp32/Bus_Parallel8.hpp"
  #include "esp32/Panel_CVBS.hpp"

 #endif

#elif defined (ESP8266)

#include "esp8266/Bus_SPI.hpp"
#include "esp8266/Bus_I2C.hpp"

#elif defined (__SAMD21__) || defined(__SAMD21G18A__) || defined(__SAMD21J18A__) || defined(__SAMD21E17A__) || defined(__SAMD21E18A__)

#include "samd21/Bus_SPI.hpp"
#include "samd21/Bus_I2C.hpp"

#elif defined (__SAMD51__)

#include "samd51/Bus_SPI.hpp"
#include "samd51/Bus_I2C.hpp"

#elif defined (STM32F2xx) || defined (STM32F4xx) || defined (STM32F7xx)

#include "stm32/Bus_SPI.hpp"

#elif defined (ARDUINO_ARCH_SPRESENSE)

#include "spresense/Bus_SPI.hpp"

#elif defined (ARDUINO_ARCH_MBED_RP2040) || defined(ARDUINO_ARCH_RP2040)

#include "rp2040/Light_PWM.hpp"
#include "rp2040/Bus_I2C.hpp"
#include "rp2040/Bus_SPI.hpp"

#elif defined (ARDUINO)

#include "arduino_default/Bus_SPI.hpp"

#elif __has_include(<SDL2/SDL.h>) || __has_include(<SDL.h>)

#include "sdl/Bus_I2C.hpp"
#include "sdl/Panel_sdl.hpp"

#endif

