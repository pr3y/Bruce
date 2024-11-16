#ifdef HAS_WS2812_LED

#include <FastLED.h>

void ws2812_init(void);
void ws2812_setup(void);
void ws2812_set_color(CRGB c);
void ws2812_set_light(uint8_t light);
void ws2812_TurnOff(void);
void ws2812_TurnOn(void);

#endif