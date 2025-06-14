#include "led_control.h"
#ifdef HAS_RGB_LED
#include "core/display.h"
#include "core/utils.h"
#include <globals.h>

#define FASTLED_RMT_BUILTIN_DRIVER 1  // Use the ESP32 RMT built-in driver
#define FASTLED_RMT_MAX_CHANNELS 1    // Maximum number of RMT channels
#define FASTLED_ESP32_RMT_CHANNEL_0 0 // Use RMT channel 0 for FastLED
#include "driver/rmt.h"
#include <FastLED.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

CRGB leds[LED_COUNT];

CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v) {
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch ((h / 60) % 6) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
            r = v;
            g = p;
            b = q;
            break;
    }

    CRGB c;
    c.red = r;
    c.green = g;
    c.blue = b;
    return c;
}

uint32_t alterOneColorChannel(uint32_t color, uint16_t newR, uint16_t newG, uint16_t newB) {
    uint8_t r = ((color >> 16) & 0xFF);
    uint8_t g = ((color >> 8) & 0xFF);
    uint8_t b = (color & 0xFF);

    if (newR != 256) r = newR;
    if (newG != 256) g = newG;
    if (newB != 256) b = newB;

    return ((r << 16) | (g << 8) | b);
}

TaskHandle_t colorWheelTaskHandle = NULL;

void colorWheelTask(void *pvParameters) {
    uint16_t hueOffset = 0;

    while (1) {
        hueOffset = (hueOffset + (36 / LED_COUNT)) % 360; // Increment and wrap around at 360 degrees

        // Loop through each LED and set its color
        for (int i = 0; i < LED_COUNT; i++) {
            uint16_t hue = (hueOffset + (i * 360 / LED_COUNT)) % 360;
            CRGB color = hsvToRgb(hue, 255, 255);
            leds[i] = color;
        }

        FastLED.show();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void beginLed() {
#ifdef RGB_LED_CLK
    FastLED.addLeds<LED_TYPE, RGB_LED, RGB_LED_CLK, LED_ORDER>(leds, LED_COUNT);
#else
    FastLED.addLeds<LED_TYPE, RGB_LED, LED_ORDER>(leds, LED_COUNT); // Initialize the LED Object. Only 1 LED.
#endif

    /* The default FastLED driver takes over control of the RMT interrupt
     * handler, making it hard to use the RMT device for other
     * (non-FastLED) purposes. You can change it's behavior to use the ESP
     * core driver instead, allowing other RMT applications to
     * co-exist. To switch to this mode, add the following directive
     * before you include FastLED.h:
     *
     *      #define FASTLED_RMT_BUILTIN_DRIVER 1
     *  RMT is also used for RF Spectrum (and for RF readings in the future),
     *  So it is needed to restart the driver in case it had been turned off
     *  by the RF functions, in this case, we are restarting it all the time
     */
    // -- RMT configuration for transmission

    // These configurations made T-Embed (non CC1101) stop working
    // Commented to test if with the FASTLED_RMT_MAX_CHANNELS 1 was sufficient for the other devices to work
    // LED and RF Spectrum and RAW capture and it is working well without it for now.. So I'll keep the code
    // below for the case we find some issue and need to rollback

    /*
        rmt_config_t rmt_tx;
        memset(&rmt_tx, 0, sizeof(rmt_config_t));
        rmt_tx.channel = rmt_channel_t(FASTLED_ESP32_RMT_CHANNEL_0);
        rmt_tx.rmt_mode = RMT_MODE_TX;
        rmt_tx.gpio_num = (gpio_num_t)RGB_LED;
        rmt_tx.mem_block_num = 2;
        rmt_tx.clk_div = 2;
        rmt_tx.tx_config.loop_en = false;
        rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
        rmt_tx.tx_config.carrier_en = false;
        rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
        rmt_tx.tx_config.idle_output_en = true;

        // -- Apply the configuration
        rmt_config(&rmt_tx);
        rmt_driver_uninstall(rmt_channel_t(FASTLED_ESP32_RMT_CHANNEL_0));
        rmt_driver_install(rmt_channel_t(FASTLED_ESP32_RMT_CHANNEL_0), 0, 0);
    */
    if (bruceConfig.ledColor == LED_COLOR_WHEEL && colorWheelTaskHandle == NULL) {
        xTaskCreate(colorWheelTask, "ColorWheel", 2048, NULL, 1, &colorWheelTaskHandle);
    } else setLedColor(bruceConfig.ledColor);

    setLedBrightness(bruceConfig.ledBright);
}

void blinkLed(int blinkTime) {
    if (!bruceConfig.ledBlinkEnabled) return;

    int ledBrightFrom = bruceConfig.ledBright;
    int ledBrightTo = ledBrightFrom > 0 ? 0 : 50;

    beginLed();
    setLedBrightness(ledBrightTo);
    ioExpander.turnPinOnOff(IO_EXP_VIBRO, HIGH);
    delay(blinkTime);
    setLedBrightness(ledBrightFrom);
    ioExpander.turnPinOnOff(IO_EXP_VIBRO, LOW);
}

void setLedColor(CRGB color) {
    for (int i = 0; i < LED_COUNT; i++) leds[i] = color;
    FastLED.show();
}

void setLedBrightness(int value) {
    value = max(0, min(100, value));
    int bright = 255 * value / 100;
    FastLED.setBrightness(bright);
    FastLED.show();
}

void setLedColorConfig() {
    int idx;
    if (bruceConfig.ledColor == CRGB::Black) idx = 0;
    else if (bruceConfig.ledColor == CRGB::Purple) idx = 1;
    else if (bruceConfig.ledColor == CRGB::White) idx = 2;
    else if (bruceConfig.ledColor == CRGB::Red) idx = 3;
    else if (bruceConfig.ledColor == CRGB::Green) idx = 4;
    else if (bruceConfig.ledColor == CRGB::Blue) idx = 5;
    else if (bruceConfig.ledColor == CRGB::Orange) idx = 6;
    else if (bruceConfig.ledColor == LED_COLOR_WHEEL) idx = 7; // colorwheel
    else idx = 8;                                              // custom color

    options = {
        {"OFF",
         [=]() { bruceConfig.setLedColor(CRGB::Black); },
         bruceConfig.ledColor == CRGB::Black,
         [](void *pointer, bool shouldRender) {
             setLedColor(CRGB::Black);
             return false;
         }},
        {"Purple",
         [=]() { bruceConfig.setLedColor(CRGB::Purple); },
         bruceConfig.ledColor == CRGB::Purple,
         [](void *pointer, bool shouldRender) {
             setLedColor(CRGB::Purple);
             return false;
         }},
        {"White",
         [=]() { bruceConfig.setLedColor(CRGB::White); },
         bruceConfig.ledColor == CRGB::White,
         [](void *pointer, bool shouldRender) {
             setLedColor(CRGB::White);
             return false;
         }},
        {"Red",
         [=]() { bruceConfig.setLedColor(CRGB::Red); },
         bruceConfig.ledColor == CRGB::Red,
         [](void *pointer, bool shouldRender) {
             setLedColor(CRGB::Red);
             return false;
         }},
        {"Green",
         [=]() { bruceConfig.setLedColor(CRGB::Green); },
         bruceConfig.ledColor == CRGB::Green,
         [](void *pointer, bool shouldRender) {
             setLedColor(CRGB::Green);
             return false;
         }},
        {"Blue",
         [=]() { bruceConfig.setLedColor(CRGB::Blue); },
         bruceConfig.ledColor == CRGB::Blue,
         [](void *pointer, bool shouldRender) {
             setLedColor(CRGB::Blue);
             return false;
         }},
        {"Orange",
         [=]() { bruceConfig.setLedColor(CRGB::Orange); },
         bruceConfig.ledColor == CRGB::Orange,
         [](void *pointer, bool shouldRender) {
             setLedColor(CRGB::Orange);
             return false;
         }},
        {"Color Wheel",
         [=]() { bruceConfig.setLedColor(LED_COLOR_WHEEL); },
         bruceConfig.ledColor == LED_COLOR_WHEEL},
        {"Custom Color",
         [=]() { setCustomColorMenu(); },
         idx == 8,
         [](void *pointer, bool shouldRender) {
             setLedColor(bruceConfig.ledColor);
             return false;
         }},
    };

    addOptionToMainMenu();

    loopOptions(options, idx);

    if (bruceConfig.ledColor != LED_COLOR_WHEEL && colorWheelTaskHandle != NULL) {
        vTaskDelete(colorWheelTaskHandle);
        colorWheelTaskHandle = NULL;
    }

    if (bruceConfig.ledColor == LED_COLOR_WHEEL && colorWheelTaskHandle == NULL) {
        xTaskCreate(colorWheelTask, "ColorWheel", 2048, NULL, 1, &colorWheelTaskHandle);
    } else setLedColor(bruceConfig.ledColor);
}

void setCustomColorMenu() {
    options = {
        {"Red Channel",   setCustomColorSettingMenuR},
        {"Green Channel", setCustomColorSettingMenuG},
        {"Blue Channel",  setCustomColorSettingMenuB},
    };
    addOptionToMainMenu();

    loopOptions(options);
}

void setCustomColorSettingMenu(std::function<uint32_t(uint32_t, int)> colorGenerator) {
    uint32_t color = bruceConfig.ledColor;
    Serial.println("StartColor: " + String(color));

    options.clear();

    static auto hoverFunction = [](void *pointer, bool shouldRender) -> bool {
        uint32_t colorToSet = *static_cast<uint32_t *>(pointer);
        setLedColor(colorToSet);
        return false;
    };

    static uint32_t colorStorage[(int)(255 / LED_COLOR_STEP) + 1];
    int selectedIndex = 0;
    int i = 0;
    int index = 0;
    while (i <= 255) {
        if (i % LED_COLOR_STEP == 0 || i == 255) {
            uint32_t updatedColor = colorGenerator(color, i);
            colorStorage[index] = updatedColor;

            String label = String(i);
            bool isSelected = (bruceConfig.ledColor == updatedColor);
            if (isSelected) selectedIndex = index;

            options.emplace_back(
                label,
                [updatedColor]() { bruceConfig.setLedColor(updatedColor); },
                isSelected,
                hoverFunction,
                &colorStorage[index]
            );
            ++index;
        }
        ++i;
    }

    addOptionToMainMenu();
    loopOptions(options, selectedIndex);
}

void setCustomColorSettingMenuR() {
    setCustomColorSettingMenu([](uint32_t baseColor, int i) {
        return alterOneColorChannel(baseColor, i, 256, 256);
    });
}

void setCustomColorSettingMenuG() {
    setCustomColorSettingMenu([](uint32_t baseColor, int i) {
        return alterOneColorChannel(baseColor, 256, i, 256);
    });
}

void setCustomColorSettingMenuB() {
    setCustomColorSettingMenu([](uint32_t baseColor, int i) {
        return alterOneColorChannel(baseColor, 256, 256, i);
    });
}

void setLedBrightnessConfig() {
    int idx = 0;
    if (bruceConfig.ledBright == 0) idx = 0;
    else if (bruceConfig.ledBright == 10) idx = 1;
    else if (bruceConfig.ledBright == 25) idx = 2;
    else if (bruceConfig.ledBright == 50) idx = 3;
    else if (bruceConfig.ledBright == 75) idx = 4;
    else if (bruceConfig.ledBright == 100) idx = 5;

    options = {
        {"OFF",
         [=]() { bruceConfig.setLedBright(0); },
         bruceConfig.ledBright == 0,
         [](void *pointer, bool shouldRender) {
             setLedBrightness(0);
             return false;
         }},
        {"10 %",
         [=]() { bruceConfig.setLedBright(10); },
         bruceConfig.ledBright == 10,
         [](void *pointer, bool shouldRender) {
             setLedBrightness(10);
             return false;
         }},
        {"25 %",
         [=]() { bruceConfig.setLedBright(25); },
         bruceConfig.ledBright == 25,
         [](void *pointer, bool shouldRender) {
             setLedBrightness(25);
             return false;
         }},
        {"50 %",
         [=]() { bruceConfig.setLedBright(50); },
         bruceConfig.ledBright == 50,
         [](void *pointer, bool shouldRender) {
             setLedBrightness(50);
             return false;
         }},
        {"75 %",
         [=]() { bruceConfig.setLedBright(75); },
         bruceConfig.ledBright == 75,
         [](void *pointer, bool shouldRender) {
             setLedBrightness(75);
             return false;
         }},
        {"100%",
         [=]() { bruceConfig.setLedBright(100); },
         bruceConfig.ledBright == 100,
         [](void *pointer, bool shouldRender) {
             setLedBrightness(100);
             return false;
         }},
    };
    addOptionToMainMenu();

    loopOptions(options, idx);
    setLedBrightness(bruceConfig.ledBright);
}
#endif
