/**
 * @file timer.cpp
 * @author Aleksei Gor (https://github.com/AlekseiGor)
 * @brief Timer
 * @version 0.1
 * @date 2024-11-26
 */

#include "core/display.h"
#include "modules/others/audio.h"

int duration = 0;

void timerLoop() {
    unsigned long startMillis = millis();
    unsigned long currentMillis;
    unsigned long elapsedMillis;

    char timeString[9];

    tft.fillScreen(bruceConfig.bgColor);

    for (;;) {
        currentMillis = millis();
        elapsedMillis = currentMillis - startMillis;

        if (elapsedMillis >= duration) {
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

        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawRect(10, 10, WIDTH - 15, HEIGHT - 15, bruceConfig.priColor);
        tft.setCursor(64, HEIGHT / 3 + 5);
        tft.setTextSize(4);
        tft.drawCentreString(timeString, WIDTH / 2, HEIGHT / 2 - 13, 1);

        if (checkSelPress() || checkEscPress()) {
            duration = 0;
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu = true;
            break;
        }

        delay(100);
    }
}

void timerSetup() {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    char timeString[9];

    tft.fillScreen(bruceConfig.bgColor);
    delay(300);

    for (;;) {
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);

        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.setTextSize(2);
        tft.drawRect(10, 10, WIDTH - 15, HEIGHT - 15, bruceConfig.priColor);
        tft.drawCentreString("Set Timer", WIDTH / 2, HEIGHT / 4 - 13, 1);
        tft.setTextSize(4);
        tft.drawCentreString(timeString, WIDTH / 2, HEIGHT / 2 - 13, 1);

        if (checkNextPress()) {
            if (++seconds >= 60) {
                seconds = 0;
                if (++minutes >= 60) {
                    minutes = 0;
                    if (++hours > 99) hours = 0;
                }
            }
        }

        if (checkSelPress()) {
            duration = (hours * 3600 + minutes * 60 + seconds + 1) * 1000;
            timerLoop();
        }

        if (checkEscPress()) {
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu = true;
            break;
        }

        delay(200);
    }
}
