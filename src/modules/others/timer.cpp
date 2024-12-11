/**
 * @file timer.cpp
 * @author Aleksei Gor (https://github.com/AlekseiGor)
 * @brief Timer
 * @version 0.1
 * @date 2024-12-07
 */

#include "timer.h"
#include "core/display.h"
#include "modules/others/audio.h"

#define DELAY_VALUE 150

Timer::Timer() {
    setup();
}

void Timer::loop() {
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
        drawMainBorder(false);
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
            _tone(2000, 1000);
            returnToMenu = true;
            break;
        }

        delay(DELAY_VALUE);
    }
}

void Timer::setup() {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int settingMode = 0;
    int underlineHeight = HEIGHT / 3 * 2;
    int underlightWidth = WIDTH / 5;

    char timeString[9];

    tft.fillScreen(bruceConfig.bgColor);

    delay(DELAY_VALUE);

    for (;;) {
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);

        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.setTextSize(2);
        drawMainBorderWithTitle("Set a timer", false);
        tft.setTextSize(4);
        tft.drawCentreString(timeString, WIDTH / 2, HEIGHT / 2 - 13, 1);

    if (settingMode == 0) {
            tft.drawLine(WIDTH / 10 * 7, underlineHeight, WIDTH / 10 * 7 + underlightWidth, underlineHeight, bruceConfig.bgColor);
            tft.drawLine(WIDTH / 10, underlineHeight, WIDTH / 10 + underlightWidth, underlineHeight, bruceConfig.priColor);
        } else if (settingMode == 1) {
            tft.drawLine(WIDTH / 10, underlineHeight, WIDTH / 10 + underlightWidth, underlineHeight, bruceConfig.bgColor);
            tft.drawLine(WIDTH / 10 * 4, underlineHeight, WIDTH / 10 * 4 + underlightWidth, underlineHeight, bruceConfig.priColor);
        } else if (settingMode == 2) {
            tft.drawLine(WIDTH / 10 * 4, underlineHeight, WIDTH / 10 * 4 + underlightWidth, underlineHeight, bruceConfig.bgColor);
            tft.drawLine(WIDTH / 10 * 7, underlineHeight, WIDTH / 10 * 7 + underlightWidth, underlineHeight, bruceConfig.priColor);
        }

        if (checkNextPress()) {
            if (settingMode == 0 && ++hours > 99) {
                hours = 0;
            } else if (settingMode == 1 && ++minutes >= 60) {
                minutes = 0;
            } else if (settingMode == 2 && ++seconds >= 60) {
                seconds = 0;
            }
        }

        if (checkEscPress()) {
            if (settingMode == 0 && --hours < 0) {
                hours = 99;
            } else if (settingMode == 1 && --minutes < 0) {
                minutes = 59;
            } else if (settingMode == 2 && --seconds < 0) {
                seconds = 59;
            }
        }

        if (checkSelPress()) {
            if (++settingMode > 2 && (hours > 0 || minutes > 0 || seconds > 0)) {
                duration = (hours * 3600 + minutes * 60 + seconds) * 1000;
                loop();
                break;
            }

            if (settingMode > 2 && hours == 0 && minutes == 0 && seconds == 0) {
                settingMode = 0;
            }
        }

        delay(DELAY_VALUE);
    }
}
