#include "util_commands.h"
#include "core/utils.h"            // to return optionsJSON
#include "core/wifi/wifi_common.h" //to return MAC addr
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

    Serial.println("Device: " + String(DEVICE_NAME));

    return true;
}

void optionsList() {
    int i = 0;
    Serial.println("\nActual Menu: " + menuOptionLabel);
    Serial.println("Options available: ");
    for (auto opt : options) {
        String txt = (opt.hovered ? ">" : " ") + String(i) + " - " + opt.label;
        Serial.println(txt);
        i++;
    }
}

uint32_t navCallback(cmd *c) {
    Command cmd(c);
    volatile bool *var = &NextPress;
    Argument arg = cmd.getArgument("command");
    Argument arg2 = cmd.getArgument("duration");
    String _dur = arg2.getValue();
    _dur.trim();
    int dur = 1;

    if (_dur.length() > 0) {
        dur = arg2.getValue().toInt();
        if (dur < 0) dur = 1; // heavy fingers on the remote interface
    }
    String nav = arg.getValue();
    nav.trim();

    if (nav == "next") {
        Serial.println("Next Pressed");
        var = &NextPress;
    } else if (nav == "prev") {
        Serial.println("Prev Pressed");
        var = &PrevPress;
    } else if (nav == "esc") {
        Serial.println("Esc Pressed");
        var = &EscPress;
    } else if (nav == "up") {
        Serial.println("Up Pressed");
        var = &UpPress;
    } else if (nav == "down") {
        Serial.println("Down Pressed");
        var = &DownPress;
    } else if (nav == "select" || nav == "sel") {
        Serial.println("Select Pressed");
        var = &SelPress;
    } else if (nav == "nextpage") {
        Serial.println("Next Page Pressed");
        var = &NextPagePress;
    } else if (nav == "prevpage") {
        Serial.println("Prev Page Pressed");
        var = &PrevPagePress;
    } else {
        Serial.println(
            "Unknown command, use: \n\"nav Next\" or \n\"nav Prev\" or \n\"nav Esc\" or \n\"nav Select\" or "
            "\n\"nav Up\" or \n\"nav Down\" or \n\"nav NextPage\" or \n\"nav PrevPage\""
        );
        return false;
    }
    // wakeUpScreen(); // Do not wakeup screen if it is dimmed and using Remote control
    unsigned long tmp = millis();
    while (millis() <= tmp + dur) {
        if (*var == false) {
            AnyKeyPress = true;
            SerialCmdPress = true;
            *var = true;
            if (!LongPress) vTaskDelay(190 / portTICK_PERIOD_MS);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    tmp = millis() - tmp;
    Serial.printf("and Released after %lums", tmp);
    optionsList();

    return true;
}

uint32_t optionsCallback(cmd *c) {
    Command cmd(c);
    Argument arg = cmd.getArgument("run");
    // int opt = arg.getValue().startsWith("-") ? -1 : arg.getValue().toInt();
    int opt = arg.getValue().toInt();

    if (opt >= 0 && opt < options.size()) {
        // wakeUpScreen(); // Do not wakeup screen if it is dimmed and using Remote control
        forceMenuOption = opt;
        Serial.printf("Selected option %d: %s\n", forceMenuOption, options[forceMenuOption].label.c_str());
        vTaskDelay(30 / portTICK_PERIOD_MS);
        optionsList();
    } else if (options.size() > 0) {
        optionsList();
    } else Serial.println("No options Available");
    return true;
}
uint32_t optionsJsonCallback(cmd *c) {
    String response = getOptionsJSON(); // core/utils.h
    Serial.println(response);
    return true;
}

uint32_t displayCallback(cmd *c) {
    Command cmd(c);
    Argument arg = cmd.getArgument("option");
    String opt = arg.getValue();
    if (opt == "start") {
        Serial.println("Display: Started logging tft");
        tft.setLogging(true);
    } else if (opt == "stop") {
        Serial.println("Display: Stopped logging tft");
        tft.setLogging(false);
    } else if (opt == "status") {
        if (tft.getLogging()) Serial.println("Display: Logging tft is ACTIVATED");
        else Serial.println("Display: Logging tft is DEACTIVATED");
    } else if (opt == "dump") {
        uint8_t binData[MAX_LOG_ENTRIES * MAX_LOG_SIZE];
        size_t binSize = 0;

        tft.getBinLog(binData, binSize);

        Serial.println("Binary Dump:");
        for (size_t i = 0; i < binSize; i++) {
            if (i % 16 == 0) Serial.println();
            // if (i % 16 == 0) Serial.printf("\n%04X: ", i);
            Serial.printf("%02X ", binData[i]);
        }
        Serial.println("\n[End of Dump]");
    } else {
        Serial.println(
            "Display command accept:\n"
            "display start : Start Logging\n"
            "display stop  : Stop Logging\n"
            "display status: Get Logging state\n"
            "display dump  : Dumps binary log"
        );
        return false;
    }
    return true;
}

void createUtilCommands(SimpleCLI *cli) {
    cli->addCommand("uptime", uptimeCallback);
    cli->addCommand("date", dateCallback);
    cli->addCommand("i2c", i2cCallback);
    cli->addCommand("free", freeCallback);
    cli->addCommand("info,!", infoCallback);
    cli->addCommand("optionsJSON", optionsJsonCallback);
    Command display = cli->addCommand("display", displayCallback);
    display.addPosArg("option", "dump");

    Command navigation = cli->addCommand("nav,navigate,navigation", navCallback);
    navigation.addPosArg("command");
    navigation.addPosArg("duration", "1");

    Command opt = cli->addCommand("options,option", optionsCallback);
    opt.addPosArg("run", "-1");
}
