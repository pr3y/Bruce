#if defined (CORE)
#ifndef _CONFIG_H_
#define _CONFIG_H_

// Screen
#define TFT_LED_PIN  32
#define TFT_DC_PIN   27
#define TFT_CS_PIN   14
#define TFT_MOSI_PIN 23
#define TFT_CLK_PIN  18
#define TFT_RST_PIN  33
#define TFT_MISO_PIN 19

// SD card
#define TFCARD_CS_PIN 4

// Buttons
#define BTN_A        0
#define BTN_B        1
#define BTN_C        2
#define BUTTON_A     0
#define BUTTON_B     1
#define BUTTON_C     2
#define BUTTON_A_PIN 39
#define BUTTON_B_PIN 38
#define BUTTON_C_PIN 37

// BEEP PIN
#define SPEAKER_PIN      25
#define TONE_PIN_CHANNEL 0

// LORA
#define LORA_CS_PIN  5
#define LORA_RST_PIN 26
#define LORA_IRQ_PIN 36

// UART
#define USE_SERIAL Serial

#endif /* SETTINGS_C */
#endif