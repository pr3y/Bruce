#ifndef __BRUCE_CONFIG_H__
#define __BRUCE_CONFIG_H__

#include "theme.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <set>
#include <vector>

/**
 * @brief Enum for RFID modules.
 */
enum RFIDModules {
    M5_RFID2_MODULE = 0,
    PN532_I2C_MODULE = 1,
    PN532_SPI_MODULE = 2,
    RC522_SPI_MODULE = 3,
    ST25R3916_SPI_MODULE = 4,
    PN532_I2C_SPI_MODULE = 5
};

/**
 * @brief Enum for RF modules.
 */
enum RFModules {
    M5_RF_MODULE = 0,
    CC1101_SPI_MODULE = 1,
};

/**
 * @brief The BruceConfig class.
 *
 * This class is responsible for managing the configuration of the Bruce firmware.
 */
class BruceConfig : public BruceTheme {
public:
    /**
     * @brief Struct for WiFi credentials.
     */
    struct WiFiCredential {
        String ssid;
        String pwd;
    };
    /**
     * @brief Struct for credentials.
     */
    struct Credential {
        String user;
        String pwd;
    };
    /**
     * @brief Struct for QR code entries.
     */
    struct QrCodeEntry {
        String menuName;
        String content;
    };

    const char *filepath = "/bruce.conf";

    // Settings
    int rotation = ROTATION > 1 ? 3 : 1;
    int dimmerSet = 10;
    int bright = 100;
    int tmz = 0;
    int soundEnabled = 1;
    int soundVolume = 100;
    int wifiAtStartup = 0;
    int instantBoot = 0;

    // Led
    int ledBright = 75;
    uint32_t ledColor = 0;
    int ledBlinkEnabled = 1;
    int ledEffect = 0;
    int ledEffectSpeed = 5;
    int ledEffectDirection = 1;

    // Wifi
    Credential webUI = {"admin", "bruce"};
    WiFiCredential wifiAp = {"BruceNet", "brucenet"};
    std::map<String, String> wifi = {};
    std::set<String> evilWifiNames = {};
    String wifiMAC = ""; //@IncursioHack

    /**
     * @brief Sets the WiFi MAC address.
     *
     * @param mac The MAC address to set.
     */
    void setWifiMAC(const String &mac) {
        wifiMAC = mac;
        saveFile(); // opcional, para salvar imediatamente
    }

    // BLE
    String bleName = String("Keyboard_" + String((uint8_t)(ESP.getEfuseMac() >> 32), HEX));

    // IR
    int irTx = LED;
    uint8_t irTxRepeats = 0;
    int irRx = GROVE_SCL;

    // RF
    int rfTx = GROVE_SDA;
    int rfRx = GROVE_SCL;
    int rfModule = M5_RF_MODULE;
    float rfFreq = 433.92;
    int rfFxdFreq = 1;
    int rfScanRange = 3;

    // iButton Pin
    int iButton = 0;

    // RFID
    int rfidModule = M5_RFID2_MODULE;
    std::set<String> mifareKeys = {};

    // GPS
    int gpsBaudrate = 9600;

    // Misc
    String startupApp = "";
    String wigleBasicToken = "";
    int devMode = 0;
    int colorInverted = 1;

    std::vector<String> disabledMenus = {};

    std::vector<QrCodeEntry> qrCodes = {
        {"Bruce AP",   "WIFI:T:WPA;S:BruceNet;P:brucenet;;"},
        {"Bruce Wiki", "https://github.com/pr3y/Bruce/wiki"},
        {"Bruce Site", "https://bruce.computer"            },
        {"Rickroll",   "https://youtu.be/dQw4w9WgXcQ"      }
    };

    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    BruceConfig() {};
    // ~BruceConfig();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Saves the configuration to a file.
     */
    void saveFile();
    /**
     * @brief Loads the configuration from a file.
     *
     * @param checkFS Whether to check the file system.
     */
    void fromFile(bool checkFS = true);
    /**
     * @brief Resets the configuration to the factory settings.
     */
    void factoryReset();
    /**
     * @brief Validates the configuration.
     */
    void validateConfig();
    /**
     * @brief Converts the configuration to a JSON document.
     *
     * @return JsonDocument The JSON document.
     */
    JsonDocument toJson() const;

    // UI Color
    /**
     * @brief Sets the UI color.
     *
     * @param primary The primary color.
     * @param secondary The secondary color.
     * @param background The background color.
     */
    void setUiColor(uint16_t primary, uint16_t *secondary = nullptr, uint16_t *background = nullptr);

    // Settings
    /**
     * @brief Sets the rotation.
     *
     * @param value The rotation value.
     */
    void setRotation(int value);
    /**
     * @brief Validates the rotation value.
     */
    void validateRotationValue();
    /**
     * @brief Sets the dimmer.
     *
     * @param value The dimmer value.
     */
    void setDimmer(int value);
    /**
     * @brief Validates the dimmer value.
     */
    void validateDimmerValue();
    /**
     * @brief Sets the brightness.
     *
     * @param value The brightness value.
     */
    void setBright(uint8_t value);
    /**
     * @brief Validates the brightness value.
     */
    void validateBrightValue();
    /**
     * @brief Sets the timezone.
     *
     * @param value The timezone value.
     */
    void setTmz(int value);
    /**
     * @brief Validates the timezone value.
     */
    void validateTmzValue();
    /**
     * @brief Sets whether the sound is enabled.
     *
     * @param value Whether the sound is enabled.
     */
    void setSoundEnabled(int value);
    /**
     * @brief Sets the sound volume.
     *
     * @param value The sound volume.
     */
    void setSoundVolume(int value);
    /**
     * @brief Validates the sound enabled value.
     */
    void validateSoundEnabledValue();
    /**
     * @brief Validates the sound volume value.
     */
    void validateSoundVolumeValue();
    /**
     * @brief Sets whether WiFi is enabled at startup.
     *
     * @param value Whether WiFi is enabled at startup.
     */
    void setWifiAtStartup(int value);
    /**
     * @brief Validates the WiFi at startup value.
     */
    void validateWifiAtStartupValue();

    // Led
    /**
     * @brief Sets the LED brightness.
     *
     * @param value The LED brightness.
     */
    void setLedBright(int value);
    /**
     * @brief Validates the LED brightness value.
     */
    void validateLedBrightValue();
    /**
     * @brief Sets the LED color.
     *
     * @param value The LED color.
     */
    void setLedColor(uint32_t value);
    /**
     * @brief Validates the LED color value.
     */
    void validateLedColorValue();
    /**
     * @brief Sets whether the LED blink is enabled.
     *
     * @param value Whether the LED blink is enabled.
     */
    void setLedBlinkEnabled(int value);
    /**
     * @brief Validates the LED blink enabled value.
     */
    void validateLedBlinkEnabledValue();
    /**
     * @brief Sets the LED effect.
     *
     * @param value The LED effect.
     */
    void setLedEffect(int value);
    /**
     * @brief Validates the LED effect value.
     */
    void validateLedEffectValue();
    /**
     * @brief Sets the LED effect speed.
     *
     * @param value The LED effect speed.
     */
    void setLedEffectSpeed(int value);
    /**
     * @brief Validates the LED effect speed value.
     */
    void validateLedEffectSpeedValue();
    /**
     * @brief Sets the LED effect direction.
     *
     * @param value The LED effect direction.
     */
    void setLedEffectDirection(int value);
    /**
     * @brief Validates the LED effect direction value.
     */
    void validateLedEffectDirectionValue();

    // Wifi
    /**
     * @brief Sets the web UI credentials.
     *
     * @param usr The username.
     * @param pwd The password.
     */
    void setWebUICreds(const String &usr, const String &pwd);
    /**
     * @brief Sets the WiFi AP credentials.
     *
     * @param ssid The SSID.
     * @param pwd The password.
     */
    void setWifiApCreds(const String &ssid, const String &pwd);
    /**
     * @brief Adds a WiFi credential.
     *
     * @param ssid The SSID.
     * @param pwd The password.
     */
    void addWifiCredential(const String &ssid, const String &pwd);
    /**
     * @brief Adds a QR code entry.
     *
     * @param menuName The menu name.
     * @param content The content.
     */
    void addQrCodeEntry(const String &menuName, const String &content);
    /**
     * @brief Removes a QR code entry.
     *
     * @param menuName The menu name.
     */
    void removeQrCodeEntry(const String &menuName);
    /**
     * @brief Gets the WiFi password for a given SSID.
     *
     * @param ssid The SSID.
     * @return String The password.
     */
    String getWifiPassword(const String &ssid) const;
    /**
     * @brief Adds an evil WiFi name.
     *
     * @param value The evil WiFi name.
     */
    void addEvilWifiName(String value);
    /**
     * @brief Removes an evil WiFi name.
     *
     * @param value The evil WiFi name.
     */
    void removeEvilWifiName(String value);

    // BLE
    /**
     * @brief Sets the BLE name.
     *
     * @param name The BLE name.
     */
    void setBleName(const String name);

    // IR
    /**
     * @brief Sets the IR TX pin.
     *
     * @param value The IR TX pin.
     */
    void setIrTxPin(int value);
    /**
     * @brief Sets the IR TX repeats.
     *
     * @param value The IR TX repeats.
     */
    void setIrTxRepeats(uint8_t value);
    /**
     * @brief Sets the IR RX pin.
     *
     * @param value The IR RX pin.
     */
    void setIrRxPin(int value);

    // RF
    /**
     * @brief Sets the RF TX pin.
     *
     * @param value The RF TX pin.
     */
    void setRfTxPin(int value);
    /**
     * @brief Sets the RF RX pin.
     *
     * @param value The RF RX pin.
     */
    void setRfRxPin(int value);
    /**
     * @brief Sets the RF module.
     *
     * @param value The RF module.
     */
    void setRfModule(RFModules value);
    /**
     * @brief Validates the RF module value.
     */
    void validateRfModuleValue();
    /**
     * @brief Sets the RF frequency.
     *
     * @param value The RF frequency.
     * @param fxdFreq The fixed frequency.
     */
    void setRfFreq(float value, int fxdFreq = 2);
    /**
     * @brief Sets the RF fixed frequency.
     *
     * @param value The RF fixed frequency.
     */
    void setRfFxdFreq(float value);
    /**
     * @brief Sets the RF scan range.
     *
     * @param value The RF scan range.
     * @param fxdFreq The fixed frequency.
     */
    void setRfScanRange(int value, int fxdFreq = 0);
    /**
     * @brief Validates the RF scan range value.
     */
    void validateRfScanRangeValue();

    // iButton
    /**
     * @brief Sets the iButton pin.
     *
     * @param value The iButton pin.
     */
    void setiButtonPin(int value);

    // RFID
    /**
     * @brief Sets the RFID module.
     *
     * @param value The RFID module.
     */
    void setRfidModule(RFIDModules value);
    /**
     * @brief Validates the RFID module value.
     */
    void validateRfidModuleValue();
    /**
     * @brief Adds a Mifare key.
     *
     * @param value The Mifare key.
     */
    void addMifareKey(String value);
    /**
     * @brief Validates the Mifare keys.
     */
    void validateMifareKeysItems();

    // GPS
    /**
     * @brief Sets the GPS baudrate.
     *
     * @param value The GPS baudrate.
     */
    void setGpsBaudrate(int value);
    /**
     * @brief Validates the GPS baudrate value.
     */
    void validateGpsBaudrateValue();

    // Misc
    /**
     * @brief Sets the startup app.
     *
     * @param value The startup app.
     */
    void setStartupApp(String value);
    /**
     * @brief Sets the Wigle basic token.
     *
     * @param value The Wigle basic token.
     */
    void setWigleBasicToken(String value);
    /**
     * @brief Sets whether dev mode is enabled.
     *
     * @param value Whether dev mode is enabled.
     */
    void setDevMode(int value);
    /**
     * @brief Validates the dev mode value.
     */
    void validateDevModeValue();
    /**
     * @brief Sets whether the color is inverted.
     *
     * @param value Whether the color is inverted.
     */
    void setColorInverted(int value);
    /**
     * @brief Validates the color inverted value.
     */
    void validateColorInverted();
    /**
     * @brief Adds a disabled menu.
     *
     * @param value The disabled menu.
     */
    void addDisabledMenu(String value);
    // TODO: removeDisabledMenu(String value);
};

#endif
