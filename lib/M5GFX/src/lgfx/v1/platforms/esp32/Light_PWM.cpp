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
#if defined (ESP_PLATFORM)
#include <sdkconfig.h>

#include "Light_PWM.hpp"

#if defined ( ARDUINO )
 #include <esp32-hal-ledc.h>
 #if __has_include(<esp_arduino_version.h>)
  #include <esp_arduino_version.h>
 #endif
#else
 #include <driver/ledc.h>
#endif

namespace lgfx
{
 inline namespace v1
 {
//----------------------------------------------------------------------------

  static constexpr const uint8_t PWM_BITS = 9;


  bool Light_PWM::init(uint8_t brightness)
  {

#if defined ( ARDUINO )

#if defined ESP_ARDUINO_VERSION
  #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    #define LEDC_USE_IDF_V5 // esp32-arduino core 3.x.x uses the new ledC syntax
  #endif   
#endif

#if defined LEDC_USE_IDF_V5
    ledcAttach(_cfg.pin_bl, _cfg.freq, PWM_BITS); 
    setBrightness(brightness);
#else
    ledcSetup(_cfg.pwm_channel, _cfg.freq, PWM_BITS);
    ledcAttachPin(_cfg.pin_bl, _cfg.pwm_channel);
    setBrightness(brightness);
#endif
#else

    static ledc_channel_config_t ledc_channel;
    {
     ledc_channel.gpio_num   = (gpio_num_t)_cfg.pin_bl;
#if SOC_LEDC_SUPPORT_HS_MODE
     ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
#else
     ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
#endif
     ledc_channel.channel    = (ledc_channel_t)_cfg.pwm_channel;
     ledc_channel.intr_type  = LEDC_INTR_DISABLE;
     ledc_channel.timer_sel  = (ledc_timer_t)((_cfg.pwm_channel >> 1) & 3);
     ledc_channel.duty       = _cfg.invert ? (1 << PWM_BITS) : 0;
     ledc_channel.hpoint     = 0;
    };
    ledc_channel_config(&ledc_channel);
    static ledc_timer_config_t ledc_timer;
    {
#if SOC_LEDC_SUPPORT_HS_MODE
      ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;     // timer mode
#else
      ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
#endif
      ledc_timer.duty_resolution = (ledc_timer_bit_t)PWM_BITS; // resolution of PWM duty
      ledc_timer.freq_hz = _cfg.freq;                        // frequency of PWM signal
      ledc_timer.timer_num = ledc_channel.timer_sel;    // timer index
    };
    ledc_timer_config(&ledc_timer);

    setBrightness(brightness);

#endif

    return true;
  }

  void Light_PWM::setBrightness(uint8_t brightness)
  {
    uint32_t duty = 0;
    if (brightness)
    {
      uint_fast16_t ofs = _cfg.offset;
      if (ofs) { ofs = ofs * 259 >> 8; }
      duty = brightness * (257 - ofs);
      duty += ofs * 255;
      duty += 1 << (15 - PWM_BITS);
      duty >>= 16 - PWM_BITS;
    }
    if (_cfg.invert) duty = (1 << PWM_BITS) - duty;

#if defined ( ARDUINO )
#if defined LEDC_USE_IDF_V5
      ledcWrite(_cfg.pin_bl, duty);
#else
      ledcWrite(_cfg.pwm_channel, duty);
#endif
#elif SOC_LEDC_SUPPORT_HS_MODE
      ledc_set_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)_cfg.pwm_channel, duty);
      ledc_update_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)_cfg.pwm_channel);
#else
      ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_cfg.pwm_channel, duty);
      ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_cfg.pwm_channel);
#endif
  }


//----------------------------------------------------------------------------
 }
}

#endif
