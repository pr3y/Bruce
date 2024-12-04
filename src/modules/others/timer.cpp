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
int delayValue = 150;

void timerLoop() {
    unsigned long startMillis = millis();
    unsigned long currentMillis;
    unsigned long elapsedMillis;

    char timeString[9];

    tft.fillScreen(bruceConfig.bgColor);

    for (;;) {
        currentMillis = millis();
        elapsedMillis = currentMillis - startMillis;

        unsigned long remainingMillis = duration - elapsedMillis;

        int seconds = (remainingMillis / 1000) % 60 + 1;
        int minutes = (remainingMillis / 60000) % 60;
        int hours = (remainingMillis / 3600000);

        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);

        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawRect(10, 10, WIDTH - 15, HEIGHT - 15, bruceConfig.priColor);
        tft.setCursor(64, HEIGHT / 3 + 5);
        tft.setTextSize(4);
        tft.drawCentreString(timeString, WIDTH / 2, HEIGHT / 2 - 13, 1);

        if (checkEscPress()) {
            duration = 0;
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu = true;
            break;
        }

        if (elapsedMillis >= duration) {
            tft.fillScreen(bruceConfig.bgColor);
            _tone(500, 500);
            returnToMenu = true;
            break;
        }

        delay(delayValue);
    }
}

void timerSetup() {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int settingMode = 0;

    char timeString[9];

    tft.fillScreen(bruceConfig.bgColor);

    delay(delayValue);

    for (;;) {
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);

        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.setTextSize(2);
        tft.drawRect(10, 10, WIDTH - 15, HEIGHT - 15, bruceConfig.priColor);
        tft.drawCentreString("Set a timer", WIDTH / 2, HEIGHT / 4 - 13, 1);
        tft.setTextSize(4);
        tft.drawCentreString(timeString, WIDTH / 2, HEIGHT / 2 - 13, 1);

        if (settingMode == 0) {
            tft.drawLine(WIDTH / 10 * 7, HEIGHT / 3 * 2, WIDTH / 10 * 7 + tft.textWidth("00", 1), HEIGHT / 3 * 2, bruceConfig.bgColor);
            tft.drawLine(WIDTH / 10, HEIGHT / 3 * 2, WIDTH / 10 + tft.textWidth("00", 1), HEIGHT / 3 * 2, bruceConfig.priColor);
        } else if (settingMode == 1) {
            tft.drawLine(WIDTH / 10, HEIGHT / 3 * 2, WIDTH / 10 + tft.textWidth("00", 1), HEIGHT / 3 * 2, bruceConfig.bgColor);
            tft.drawLine(WIDTH / 10 * 4, HEIGHT / 3 * 2, WIDTH / 10 * 4 + tft.textWidth("00", 1), HEIGHT / 3 * 2, bruceConfig.priColor);
        } else if (settingMode == 2) {
            tft.drawLine(WIDTH / 10 * 4, HEIGHT / 3 * 2, WIDTH / 10 * 4 + tft.textWidth("00", 1), HEIGHT / 3 * 2, bruceConfig.bgColor);
            tft.drawLine(WIDTH / 10 * 7, HEIGHT / 3 * 2, WIDTH / 10 * 7 + tft.textWidth("00", 1), HEIGHT / 3 * 2, bruceConfig.priColor);
        }

        if (checkNextPress()) {
            if (settingMode == 0) {
                if (++hours > 99) hours = 0;
            } else if (settingMode == 1) {
                if (++minutes >= 60) minutes = 0;
            } else if (settingMode == 2) {
                if (++seconds >= 60) seconds = 0;
            }
        }

        if (checkEscPress()) {
            if (settingMode == 0) {
                if (--hours < 0) hours = 99;
            } else if (settingMode == 1) {
                if (--minutes < 0) minutes = 59;
            } else if (settingMode == 2) {
                if (--seconds < 0) seconds = 59;
            }
        }

        if (checkSelPress()) {
            if (++settingMode > 2 && (hours > 0 || minutes > 0 || seconds > 0)) {
                duration = (hours * 3600 + minutes * 60 + seconds) * 1000;
                timerLoop();
                break;
            }

            if (settingMode > 2 && hours == 0 && minutes == 0 && seconds == 0) {
                settingMode = 0;
            }
        }

        delay(delayValue);
    }
}
