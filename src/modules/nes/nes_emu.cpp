/* Arduino Nofrendo
 * Please check hw_config.h and display.cpp for configuration details
 */
#include <FFat.h>
#include <SD.h>
#include <SD_MMC.h>
#include <SPIFFS.h>
#include <core/sd_functions.h>
#include <esp_task_wdt.h>
#include <esp_wifi.h>

#include <nofrendo.h>

void setup_nes() {

    // // turn off WiFi
    // esp_wifi_deinit();

    // // disable Core 0 WDT
    // TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    // esp_task_wdt_delete(idle_0);

    char *argv[1];

    bool foundRom = false;
    String romName = loopSD(LittleFS, true, "nes", "/nes");

    argv[0] = (char *)romName.c_str();

    if (romName == "") {
        Serial.println("Failed to read rom file");
        return;
    }

    Serial.println("NoFrendo start!\n");
    nofrendo_main(1, argv);
    Serial.println("NoFrendo end!\n");
}
