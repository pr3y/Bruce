/**
 * @file timer.cpp
 * @author Aleksei Gor (https://github.com/AlekseiGor)
 * @brief Timer
 * @version 0.1
 * @date 2024-11-26
 */

#include "core/display.h"
#include "modules/others/audio.h"

void timerLoop(int16_t duration) {
    unsigned long startMillis = millis();
    unsigned long currentMillis;
    unsigned long elapsedMillis;

    char timeString[9];

    tft.fillScreen(bruceConfig.bgColor);
    delay(300);

    Serial.println("Timer started");

    for (;;) {
        currentMillis = millis();
        elapsedMillis = currentMillis - startMillis;

        Serial.print("Current millis: ");
        Serial.println(currentMillis);
        Serial.print("Elapsed millis: ");
        Serial.println(elapsedMillis);

        if (elapsedMillis >= duration) {
            Serial.println("Timer expired");
            tft.fillScreen(bruceConfig.bgColor);
            _tone(500, 500);
            returnToMenu = true;
            break;
        }

        unsigned long remainingMillis = duration - elapsedMillis;
        int seconds = (remainingMillis / 1000) % 60;
        int minutes = (remainingMillis / 60000) % 60;
        int hours = (remainingMillis / 3600000);

        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);

        Serial.print("Remaining time: ");
        Serial.println(timeString);

        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawRect(10, 10, WIDTH - 15, HEIGHT - 15, bruceConfig.priColor);
        tft.setCursor(64, HEIGHT / 3 + 5);
        tft.setTextSize(4);
        tft.drawCentreString(timeString, WIDTH / 2, HEIGHT / 2 - 13, 1);

        if (checkSelPress() || checkEscPress()) {
            Serial.println("Button pressed: Exiting timer");
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu = true;
            break;
        }

        delay(100);
    }
}

void timerSetup() {

    tft.fillScreen(bruceConfig.bgColor);
    delay(300);

    for (;;) {
        
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawRect(10, 10, WIDTH - 15, HEIGHT - 15, bruceConfig.priColor);
        tft.drawCentreString("Set a timer", WIDTH / 2, HEIGHT / 4 - 13, 1);

        if (checkSelPress()) {
            timerLoop(10000);
        }

        if (checkEscPress()) {
            Serial.println("Button pressed: Exiting timer");
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu = true;
            break;
        }

        delay(100);
    }
}
