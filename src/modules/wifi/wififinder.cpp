#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"
#include "globals.h"
#include "modules/others/audio.h"
#include <vector>

std::vector<String> wifinames;
String ssid;
int32_t range;
bool found = false;
bool sound = false;

void wififinder() {
    tft.fillScreen(bruceConfig.bgColor);
    tft.setCursor(0, 0);

    tft.print("Finding Networks...");
    displayInfo("Searching...");

    while (true) {
        int nets = WiFi.scanNetworks();
        for (int i = 0; i < nets; i++) {
            ssid = WiFi.SSID(i);
            range = WiFi.RSSI(i);

            tft.setCursor(0, 0);

            if (!found) displayTextLine(String(ssid));

            for (const String &name2 : wifinames) {
                if (ssid == name2) {
                    found = true;
                    tft.fillScreen(bruceConfig.bgColor);
                    tft.setCursor(0, 0);
                    if (!sound) {
#if defined(BUZZ_PIN)
                        for (int i = 0; i < 5; i++) {
                            _tone(5000, 150);
                            delay(500);
                        }
#elif defined(HAS_NS4168_SPKR)
                        tts("Wifi Found Wifi Found Wifi Found");
#endif
                    }
                    sound = true;
                    displayTextLine(String(name2) + " has been founded. Range: " + String(range));

                    if (check(EscPress)) {
                        sound = false;
                        found = false;
                        returnToMenu = true;
                        return;
                    }
                }
            }
        }

        if (check(EscPress)) {
            sound = false;
            found = false;
            returnToMenu = true;
            return;
        }
    }
}

void wififindersetup() {
    File file;
    FS *fs;
    int selectedmode;
    String ExampleFile = "";

    options = {
        {"Single Wifi",   [&]() { selectedmode = 0; }},
        {"From The File", [&]() { selectedmode = 1; }},
    };
    addOptionToMainMenu();
    loopOptions(options);

    if (selectedmode == 0) {
        String name = keyboard("", 32, "Wifi Name");
        wifinames.push_back(name);

        if (wifinames[0] == "" || wifinames[0] == " " || wifinames[0] == "  ") {
            displayError("Wifi Name Can't be Empty.");
            delay(2500);
            return;
        }
    }

    if (selectedmode == 1) {
        if (!file) {
            options = {};
            fs = nullptr;

            if (setupSdCard()) {
                options.push_back({"SD Card", [&]() { fs = &SD; }});
            }
            options.push_back({"LittleFS", [&]() { fs = &LittleFS; }});
            addOptionToMainMenu();

            loopOptions(options);
            if (fs != nullptr) {
                ExampleFile = loopSD(*fs, true, "TXT");
                file = fs->open(ExampleFile, FILE_READ);
                ExampleFile = file.readString();
                ExampleFile.replace("\r\n", "\n");
            }
        }

        int start = 0;
        int end = ExampleFile.indexOf('\n', start);

        while (end != -1) {
            String line = ExampleFile.substring(start, end);
            line.trim();

            if (line.length() > 0) { wifinames.push_back(line); }

            start = end + 1;
            end = ExampleFile.indexOf('\n', start);
        }

        if (start < ExampleFile.length()) {
            String line = ExampleFile.substring(start);
            line.trim();
            if (line.length() > 0) { wifinames.push_back(line); }
        }
    }
    tft.fillScreen(bruceConfig.bgColor);
    tft.setCursor(0, 0);
    wififinder();
}
