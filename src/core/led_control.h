#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__
#include <globals.h>

#ifdef HAS_RGB_LED
#include <Arduino.h>
#include <FastLED.h>

#define LED_EFFECT_SOLID 0
#define LED_COLOR_BREATHE 1
#define LED_EFFECT_COLOR_CYCLE 2
#define LED_EFFECT_COLOR_WHEEL 3
#define LED_EFFECT_CHASE 4
#define LED_EFFECT_CHASE_TAIL 5

/**
 * @brief Converts HSV color to RGB color.
 *
 * @param h Hue (0-359).
 * @param s Saturation (0-255).
 * @param v Value (0-255).
 * @return CRGB The RGB color.
 */
CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v);

/**
 * @brief Alters one color channel of a color.
 *
 * @param color The color to alter.
 * @param newR The new red value (0-255).
 * @param newG The new green value (0-255).
 * @param newB The new blue value (0-255).
 * @return uint32_t The altered color.
 */
uint32_t alterOneColorChannel(uint32_t color, uint16_t newR, uint16_t newG, uint16_t newB);

/**
 * @brief Initializes the LED.
 */
void beginLed();

/**
 * @brief Blinks the LED.
 *
 * @param blinkTime The time in milliseconds to blink the LED.
 */
void blinkLed(int blinkTime = 50);

/**
 * @brief Sets the LED color.
 *
 * @param color The color to set.
 */
void setLedColor(CRGB color);

/**
 * @brief Sets the LED effect.
 *
 * @param effect The effect to set.
 */
void setLedEffect(int effect);

/**
 * @brief Sets the LED color configuration.
 */
void setLedColorConfig();

/**
 * @brief Sets the custom color menu.
 */
void setCustomColorMenu();

/**
 * @brief Sets the custom color setting menu for the red channel.
 */
void setCustomColorSettingMenuR();

/**
 * @brief Sets the custom color setting menu for the green channel.
 */
void setCustomColorSettingMenuG();

/**
 * @brief Sets the custom color setting menu for the blue channel.
 */
void setCustomColorSettingMenuB();

/**
 * @brief Sets the LED effect configuration.
 */
void setLedEffectConfig();

/**
 * @brief Sets the LED effect speed configuration.
 */
void setLedEffectSpeedConfig();

/**
 * @brief Sets the LED effect direction configuration.
 */
void setLedEffectDirectionConfig();

/**
 * @brief Sets up the LED.
 */
void ledSetup();

/**
 * @brief Enables or disables the LED effects.
 *
 * @param enable True to enable, false to disable.
 */
void ledEffects(bool enable);

/**
 * @brief Enables or disables the LED preview mode.
 *
 * @param enable True to enable, false to disable.
 */
void ledPreviewMode(bool enable);

/**
 * @brief Sets the LED brightness.
 *
 * @param value The brightness value (0-100).
 */
void setLedBrightness(int value);

/**
 * @brief Sets the LED brightness configuration.
 */
void setLedBrightnessConfig();

#else
inline void blinkLed(int blinkTime = 50) {};
#endif

#endif
