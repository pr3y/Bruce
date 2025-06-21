#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#ifndef CC1101_GDO2_PIN
#define CC1101_GDO2_PIN -1
#endif

class BruceConfigPins {
public:
    struct SPIPins {
        gpio_num_t sck = GPIO_NUM_NC;
        gpio_num_t miso = GPIO_NUM_NC;
        gpio_num_t mosi = GPIO_NUM_NC;
        gpio_num_t cs = GPIO_NUM_NC;
        gpio_num_t io0 = GPIO_NUM_NC;
        gpio_num_t io2 = GPIO_NUM_NC;

        SPIPins()
            : sck(GPIO_NUM_NC), miso(GPIO_NUM_NC), mosi(GPIO_NUM_NC), cs(GPIO_NUM_NC), io0(GPIO_NUM_NC),
              io2(GPIO_NUM_NC) {}

        SPIPins(
            gpio_num_t sck_val, gpio_num_t miso_val, gpio_num_t mosi_val, gpio_num_t cs_val,
            gpio_num_t io0_val = GPIO_NUM_NC, gpio_num_t io2_val = GPIO_NUM_NC
        )
            : sck(sck_val), miso(miso_val), mosi(mosi_val), cs(cs_val), io0(io0_val), io2(io2_val) {}

        void fromJson(JsonObject obj) {
            sck = (gpio_num_t)(obj["sck"] | (int)GPIO_NUM_NC);
            miso = (gpio_num_t)(obj["miso"] | (int)GPIO_NUM_NC);
            mosi = (gpio_num_t)(obj["mosi"] | (int)GPIO_NUM_NC);
            cs = (gpio_num_t)(obj["cs"] | (int)GPIO_NUM_NC);
            io0 = (gpio_num_t)(obj["io0"] | (int)GPIO_NUM_NC);
            io2 = (gpio_num_t)(obj["io2"] | (int)GPIO_NUM_NC);
        }

        void toJson(JsonObject obj) const {
            obj["sck"] = sck;
            obj["miso"] = miso;
            obj["mosi"] = mosi;
            obj["cs"] = cs;
            obj["io0"] = io0;
            obj["io2"] = io2;
        }
    };

    const char *filepath = "/brucePins.conf";

    // SPI Buses
#ifdef CC1101_SCK_PIN
    SPIPins CC1101_bus = {
        (gpio_num_t)CC1101_SCK_PIN,
        (gpio_num_t)CC1101_MISO_PIN,
        (gpio_num_t)CC1101_MOSI_PIN,
        (gpio_num_t)CC1101_SS_PIN,
        (gpio_num_t)CC1101_GDO0_PIN,
        (gpio_num_t)CC1101_GDO2_PIN
    };
#else
    SPIPins CC1101_bus;
#endif

#ifdef NRF24_SCK_PIN
    SPIPins NRF24_bus = {
        (gpio_num_t)NRF24_SCK_PIN,
        (gpio_num_t)NRF24_MISO_PIN,
        (gpio_num_t)NRF24_MOSI_PIN,
        (gpio_num_t)NRF24_SS_PIN,
        (gpio_num_t)NRF24_CE_PIN
    };
#else
    SPIPins NRF24_bus;
#endif

#ifdef SDCARD_SCK
    SPIPins SDCARD_bus = {
        (gpio_num_t)SDCARD_SCK, (gpio_num_t)SDCARD_MISO, (gpio_num_t)SDCARD_MOSI, (gpio_num_t)SDCARD_CS
    };
#else
    SPIPins SDCARD_bus;
#endif

#if defined(W5500_SCK_PIN)
    SPIPins W5500_bus = {
        (gpio_num_t)W5500_SCK_PIN,
        (gpio_num_t)W5500_MISO_PIN,
        (gpio_num_t)W5500_MOSI_PIN,
        (gpio_num_t)W5500_SS_PIN,
        (gpio_num_t)W5500_INT_PIN
    };
#elif !defined(LITE_VERSION)
    SPIPins W5500_bus;
#endif

    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    BruceConfigPins() {};

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    void createFile();
    void saveFile();
    void fromFile(bool checkFS = true);
    void loadFile(JsonDocument &jsonDoc, bool checkFS = true);
    void factoryReset();
    void validateConfig();
    void fromJson(JsonObject obj);
    void toJson(JsonObject obj) const;

    void setCC1101Pins(SPIPins value);
    void setNrf24Pins(SPIPins value);
    void setSDCardPins(SPIPins value);

    void setSpiPins(SPIPins value);
    void validateSpiPins(SPIPins value);
};
