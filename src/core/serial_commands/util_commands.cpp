#include "util_commands.h"
#include "core/wifi_common.h" //to return MAC addr
#include <Wire.h>
#include <globals.h>

uint32_t uptimeCallback(cmd *c) {
    // https://github.com/espressif/arduino-esp32/blob/66c9c0b1a6a36b85d27cdac0fb52098368de1a09/libraries/WebServer/examples/AdvancedWebServer/AdvancedWebServer.ino#L64

    int sec = millis() / 1000;
    int hr = sec / 3600;
    int min = (sec / 60) % 60;
    sec = sec % 60;
    char temp[400];
    snprintf(temp, 400, "Uptime: %02d:%02d:%02d", hr, min, sec);
    Serial.println(temp);
    return true;
}

uint32_t dateCallback(cmd *c) {
    if (!clock_set) {
        Serial.println("Clock not set");
        return false;
    }

    Serial.print("Current time: ");
#if !defined(HAS_RTC)
    Serial.println(rtc.getDateTime());
    // Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
#else
    _rtc.begin();
    _rtc.GetTime(&_time);
    _rtc.GetDate(&_date);
    char stimeStr[100] = {0};
    snprintf(
        stimeStr,
        sizeof(stimeStr),
        "%02d %02d %04d %02d:%02d:%02d",
        _date.Month,
        _date.Date,
        _date.Year,
        _time.Hours,
        _time.Minutes,
        _time.Seconds
    );
    Serial.println(stimeStr);
#endif

    return true;
}

uint32_t i2cCallback(cmd *c) {
    // scan for connected i2c modules
    // derived from https://learn.adafruit.com/scanning-i2c-addresses/arduino
    Wire.begin(GROVE_SDA, GROVE_SCL);
    byte error, address;
    int nDevices;
    Serial.println("Scanning...");
    nDevices = 0;
    for (address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    if (nDevices == 0) {
        Serial.println("No I2C devices found");
        return false;
    }

    return true;
}

uint32_t freeCallback(cmd *c) {
    Serial.print("Total heap: ");
    Serial.println(ESP.getHeapSize());
    Serial.print("Free heap: ");
    Serial.println(ESP.getFreeHeap());

    if (psramFound()) {
        Serial.print("Total PSRAM: ");
        Serial.println(ESP.getPsramSize());
        Serial.print("Free PSRAM: ");
        Serial.println(ESP.getFreePsram());
    }

    return true;
}

uint32_t infoCallback(cmd *c) {
    Serial.print("Bruce v");
    Serial.println(BRUCE_VERSION);
    Serial.println(GIT_COMMIT_HASH);
    Serial.printf("SDK: %s\n", ESP.getSdkVersion());
    Serial.println("MAC addr: " + String(WiFi.macAddress()));

    // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ChipID/GetChipID/GetChipID.ino
    // Serial.printf("Chip is %s (revision v%d)\n", ESP.getChipModel(), ESP.getChipRevision());
    // Serial.printf("Detected flash size: %d\n", ESP.getFlashChipSize());
    // Serial.printf("This chip has %d cores\n", ESP.getChipCores());
    // Serial.printf("CPU Freq is %d\n", ESP.getCpuFreqMHz());
    // Features: WiFi, BLE, Embedded Flash 8MB (GD)
    // Crystal is 40MHz
    // MAC: 24:58:7c:5b:24:5c

    if (wifiConnected) {
        Serial.println("Wifi: connected");
        Serial.println("Ip: " + wifiIP); // read global var
    } else {
        Serial.println("Wifi: not connected");
    }

    return true;
}

void createUtilCommands(SimpleCLI *cli) {
    cli->addCommand("uptime", uptimeCallback);
    cli->addCommand("date", dateCallback);
    cli->addCommand("i2c", i2cCallback);
    cli->addCommand("free", freeCallback);
    cli->addCommand("info,!", infoCallback);
}
