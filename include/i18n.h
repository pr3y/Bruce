#ifndef __I18N_H__
#define __I18N_H__

#include <Arduino.h>

// Language codes
enum Language {
    LANG_EN = 0,
    LANG_PT_BR = 1
};

// Forward declaration
class BruceConfig;
extern BruceConfig bruceConfig;

// String getter function - returns appropriate string based on current language
const char* getString(int stringId);

// String IDs for translation
enum StringID {
    // Main Menu Items
    STR_WIFI = 0,
    STR_BLE,
    STR_RF,
    STR_IR,
    STR_RFID,
    STR_NRF24,
    STR_FM,
    STR_FILES,
    STR_GPS,
    STR_SCRIPTS,
    STR_OTHERS,
    STR_CLOCK,
    STR_CONNECT,
    STR_CONFIG,
    
    // WiFi Menu
    STR_CONNECT_WIFI,
    STR_WIFI_AP,
    STR_DISCONNECT,
    STR_WIFI_ATKS,
    STR_EVIL_PORTAL,
    STR_LISTEN_TCP,
    STR_CLIENT_TCP,
    STR_TELNET,
    STR_SSH,
    STR_DPWO,
    STR_RAW_SNIFFER,
    STR_SCAN_HOSTS,
    STR_WIREGUARD,
    STR_BRUCEGOTCHI,
    STR_ADD_EVIL_WIFI,
    STR_REMOVE_EVIL_WIFI,
    STR_AP_INFO,
    
    // Config Menu
    STR_BRIGHTNESS,
    STR_DIM_TIME,
    STR_ORIENTATION,
    STR_UI_COLOR,
    STR_UI_THEME,
    STR_LED_COLOR,
    STR_LED_BRIGHTNESS,
    STR_LED_BLINK_ON_OFF,
    STR_SOUND_ON_OFF,
    STR_STARTUP_WIFI,
    STR_STARTUP_APP,
    STR_NETWORK_CREDS,
    STR_SLEEP,
    STR_FACTORY_RESET,
    STR_RESTART,
    STR_TURN_OFF,
    STR_DEEP_SLEEP,
    STR_ABOUT,
    STR_DEV_MODE,
    STR_I2C_FINDER,
    STR_CC1101_PINS,
    STR_NRF24_PINS,
    STR_SDCARD_PINS,
    STR_LANGUAGE,
    
    // Common Messages
    STR_BACK,
    STR_CANCEL,
    STR_SELECT,
    STR_SAVE,
    STR_DELETE,
    STR_ERROR,
    STR_SUCCESS,
    STR_LOADING,
    STR_DISABLED,
    STR_ENABLE,
    STR_DISABLE,
    
    // Settings strings
    STR_DEFAULT,
    STR_WHITE,
    STR_RED,
    STR_GREEN,
    STR_BLUE,
    STR_LIGHT_BLUE,
    STR_YELLOW,
    STR_MAGENTA,
    STR_ORANGE,
    STR_CUSTOM_UI_COLOR,
    STR_INVERT_COLOR,
    STR_SOUND_OFF,
    STR_SOUND_ON,
    STR_LED_BLINK_OFF,
    STR_LED_BLINK_ON,
};

// Define macros that call the getString function
#define LANG_WIFI getString(STR_WIFI)
#define LANG_BLE getString(STR_BLE)
#define LANG_RF getString(STR_RF)
#define LANG_IR getString(STR_IR)
#define LANG_RFID getString(STR_RFID)
#define LANG_NRF24 getString(STR_NRF24)
#define LANG_FM getString(STR_FM)
#define LANG_FILES getString(STR_FILES)
#define LANG_GPS getString(STR_GPS)
#define LANG_SCRIPTS getString(STR_SCRIPTS)
#define LANG_OTHERS getString(STR_OTHERS)
#define LANG_CLOCK getString(STR_CLOCK)
#define LANG_CONNECT getString(STR_CONNECT)
#define LANG_CONFIG getString(STR_CONFIG)

#define LANG_CONNECT_WIFI getString(STR_CONNECT_WIFI)
#define LANG_WIFI_AP getString(STR_WIFI_AP)
#define LANG_DISCONNECT getString(STR_DISCONNECT)
#define LANG_WIFI_ATKS getString(STR_WIFI_ATKS)
#define LANG_EVIL_PORTAL getString(STR_EVIL_PORTAL)
#define LANG_LISTEN_TCP getString(STR_LISTEN_TCP)
#define LANG_CLIENT_TCP getString(STR_CLIENT_TCP)
#define LANG_TELNET getString(STR_TELNET)
#define LANG_SSH getString(STR_SSH)
#define LANG_DPWO getString(STR_DPWO)
#define LANG_RAW_SNIFFER getString(STR_RAW_SNIFFER)
#define LANG_SCAN_HOSTS getString(STR_SCAN_HOSTS)
#define LANG_WIREGUARD getString(STR_WIREGUARD)
#define LANG_BRUCEGOTCHI getString(STR_BRUCEGOTCHI)
#define LANG_ADD_EVIL_WIFI getString(STR_ADD_EVIL_WIFI)
#define LANG_REMOVE_EVIL_WIFI getString(STR_REMOVE_EVIL_WIFI)
#define LANG_AP_INFO getString(STR_AP_INFO)

#define LANG_BRIGHTNESS getString(STR_BRIGHTNESS)
#define LANG_DIM_TIME getString(STR_DIM_TIME)
#define LANG_ORIENTATION getString(STR_ORIENTATION)
#define LANG_UI_COLOR getString(STR_UI_COLOR)
#define LANG_UI_THEME getString(STR_UI_THEME)
#define LANG_LED_COLOR getString(STR_LED_COLOR)
#define LANG_LED_BRIGHTNESS getString(STR_LED_BRIGHTNESS)
#define LANG_LED_BLINK_ON_OFF getString(STR_LED_BLINK_ON_OFF)
#define LANG_SOUND_ON_OFF getString(STR_SOUND_ON_OFF)
#define LANG_STARTUP_WIFI getString(STR_STARTUP_WIFI)
#define LANG_STARTUP_APP getString(STR_STARTUP_APP)
#define LANG_NETWORK_CREDS getString(STR_NETWORK_CREDS)
#define LANG_SLEEP getString(STR_SLEEP)
#define LANG_FACTORY_RESET getString(STR_FACTORY_RESET)
#define LANG_RESTART getString(STR_RESTART)
#define LANG_TURN_OFF getString(STR_TURN_OFF)
#define LANG_DEEP_SLEEP getString(STR_DEEP_SLEEP)
#define LANG_ABOUT getString(STR_ABOUT)
#define LANG_DEV_MODE getString(STR_DEV_MODE)
#define LANG_I2C_FINDER getString(STR_I2C_FINDER)
#define LANG_CC1101_PINS getString(STR_CC1101_PINS)
#define LANG_NRF24_PINS getString(STR_NRF24_PINS)
#define LANG_SDCARD_PINS getString(STR_SDCARD_PINS)
#define LANG_LANGUAGE getString(STR_LANGUAGE)

#define LANG_BACK getString(STR_BACK)
#define LANG_CANCEL getString(STR_CANCEL)
#define LANG_SELECT getString(STR_SELECT)
#define LANG_SAVE getString(STR_SAVE)
#define LANG_DELETE getString(STR_DELETE)
#define LANG_ERROR getString(STR_ERROR)
#define LANG_SUCCESS getString(STR_SUCCESS)
#define LANG_LOADING getString(STR_LOADING)
#define LANG_DISABLED getString(STR_DISABLED)
#define LANG_ENABLE getString(STR_ENABLE)
#define LANG_DISABLE getString(STR_DISABLE)

#define LANG_DEFAULT getString(STR_DEFAULT)
#define LANG_WHITE getString(STR_WHITE)
#define LANG_RED getString(STR_RED)
#define LANG_GREEN getString(STR_GREEN)
#define LANG_BLUE getString(STR_BLUE)
#define LANG_LIGHT_BLUE getString(STR_LIGHT_BLUE)
#define LANG_YELLOW getString(STR_YELLOW)
#define LANG_MAGENTA getString(STR_MAGENTA)
#define LANG_ORANGE getString(STR_ORANGE)
#define LANG_CUSTOM_UI_COLOR getString(STR_CUSTOM_UI_COLOR)
#define LANG_INVERT_COLOR getString(STR_INVERT_COLOR)
#define LANG_SOUND_OFF getString(STR_SOUND_OFF)
#define LANG_SOUND_ON getString(STR_SOUND_ON)
#define LANG_LED_BLINK_OFF getString(STR_LED_BLINK_OFF)
#define LANG_LED_BLINK_ON getString(STR_LED_BLINK_ON)

#endif
