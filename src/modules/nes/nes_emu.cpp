/* Arduino Nofrendo
 * Please check hw_config.h and display.cpp for configuration details
 */
#include <FFat.h>
#include <SD.h>
#include <SD_MMC.h>
#include <SPIFFS.h>
#include <core/display.h>
#include <core/sd_functions.h>
#include <core/utils.h>
#include <esp_task_wdt.h>
#include <esp_wifi.h>
#include <globals.h>

extern "C" {
#include <nofrendo.h>
}

void setup_nes() {

    // // turn off WiFi
    // esp_wifi_deinit();

    // // disable Core 0 WDT
    // TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    // esp_task_wdt_delete(idle_0);

    char *argv[1];

    bool foundRom = false;

    bool sdCard = false;

    options.clear();
    if (sdcardMounted) options.push_back({"SD Card", [&]() { sdCard = true; }});
    options.push_back({"LittleFS", [&]() { sdCard = false; }});

    loopOptions(options, MENU_TYPE_SUBMENU, "Files");

    String romName = loopSD(LittleFS, true, "nes", "/nes");

    String fullRomName = (sdCard ? "/sdcard" : "/littlefs") + romName;

    argv[0] = (char *)(fullRomName.c_str());

    if (fullRomName == "") {
        Serial.println("Failed to read rom file");
        return;
    }

    Serial.printf("NoFrendo start! %s\n", argv[0]);
    nofrendo_main(1, argv);
    Serial.println("NoFrendo end!\n");
}
