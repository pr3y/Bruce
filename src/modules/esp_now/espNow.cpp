#include "WiFi.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "esp_now.h"

String macAddress = "";

void initEspNow() {
    esp_now_init();
    // ...
    // ...
}

void Pair() {
    macAddress = keyboard("", 5, "MAC ADDRESS");
    //...
    //...
}
