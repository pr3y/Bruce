#include "serialcmds.h"
#include "utils.h"
#include <globals.h>

void handleSerialCommands() {
    if (!Serial.available()) return;

    String cmd_str = Serial.readStringUntil('\n');
    serialCli.parse(cmd_str);
    Serial.print("# "); // prompt
    backToMenu();       // forced menu redrawn
}

void _serialCmdsTaskLoop(void *pvParameters) {
    Serial.begin(115200);

    while (1) {
        handleSerialCommands();
        vTaskDelay(500);
    }
}

void startSerialCommandsHandlerTask() {
    TaskHandle_t serialcmdsTaskHandle;

    xTaskCreatePinnedToCore(
        _serialCmdsTaskLoop, // Function to implement the task
        "serialcmds",        // Name of the task (any string)
        20000,               // Stack size in bytes
        NULL, // This is a pointer to the parameter that will be passed to the new task. We are not using it
              // here and therefore it is set to NULL.
        2,    // Priority of the task
        &serialcmdsTaskHandle, // Task handle (optional, can be NULL).
#if SOC_CPU_CORES_NUM > 1
        1 // Core where the task should run. By default, all your Arduino code runs on Core 1 and the Wi-Fi
          // and RF functions
#else
        0 // Core where the task should run. ESP32-C5 has only one core
#endif
    ); // (these are usually hidden from the Arduino environment) use the Core 0.
}
