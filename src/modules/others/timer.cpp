/**
 * @file timer.cpp
 * @author Aleksei Gor (https://github.com/AlekseiGor)
 * @brief Timer
 * @version 0.1
 * @date 2024-12-07
 */

#include "timer.h"
#include "core/display.h"
#include "core/utils.h"
#include "modules/others/audio.h"

#define DELAY_VALUE 150

Timer::Timer() { setup(); }

Timer::~Timer() {
    tft.fillScreen(bruceConfig.bgColor);
    backToMenu();
}

void Timer::setup() {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int settingMode = 0;

    char timeString[12];

    tft.fillScreen(bruceConfig.bgColor);

    delay(DELAY_VALUE);

    while (true) {
        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours % 100, minutes % 100, seconds % 100);

        drawMainBorderWithTitle("Set a timer", false);
        tft.setTextSize(fontSize);
        tft.drawCentreString(timeString, timerX, timerY, 1);

        clearUnderline();

        if (settingMode == 0) underlineHours();
        else if (settingMode == 1) underlineMinutes();
        else if (settingMode == 2) underlineSeconds();

        if (check(NextPress)) {
            if (settingMode == 0 && ++hours > 99) {
                hours = 0;
            } else if (settingMode == 1 && ++minutes >= 60) {
                minutes = 0;
            } else if (settingMode == 2 && ++seconds >= 60) {
                seconds = 0;
            }
        }

        if (check(PrevPress)) {
            if (settingMode == 0 && --hours < 0) {
                hours = 99;
            } else if (settingMode == 1 && --minutes < 0) {
                minutes = 59;
            } else if (settingMode == 2 && --seconds < 0) {
                seconds = 59;
            }
        }

        if (check(SelPress)) {
            if (++settingMode > 2 && (hours > 0 || minutes > 0 || seconds > 0)) {
                duration = (hours * 3600 + minutes * 60 + seconds) * 1000;
                break;
            }

            if (settingMode > 2) settingMode = 0;
        }
    }

    return loop();
}

void Timer::loop() {
    unsigned long startMillis = millis();
    unsigned long currentMillis;
    unsigned long elapsedMillis;

    char timeString[12];

    tft.fillScreen(bruceConfig.bgColor);

    while (true) {
        currentMillis = millis();
        elapsedMillis = currentMillis - startMillis;

        unsigned long remainingMillis = duration + 1000 - elapsedMillis;

        int seconds = (remainingMillis / 1000) % 60;
        int minutes = (remainingMillis / 60000) % 60;
        int hours = (remainingMillis / 3600000) % 100;

        snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", hours, minutes, seconds);
        uint8_t f_size = 4;
        for (uint8_t i = 4; i > 0; i--) {
            if (i * LW * 8 < (tftWidth - BORDER_PAD_X * 2)) {
                f_size = i;
                break;
            }
        }
        drawMainBorder(false);

        tft.setTextSize(f_size);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawCentreString(timeString, timerX, timerY, 1);

        if (check(EscPress)) { break; }

        if (elapsedMillis >= duration) {
            _tone(2000, 1000);
            break;
        }
    }
}

void Timer::clearUnderline() {
    tft.drawLine(BORDER_PAD_X, underlineY, tftWidth - BORDER_PAD_X, underlineY, bruceConfig.bgColor);
}

void Timer::underlineHours() {
    tft.drawLine(
        timerX - (4 * LW * fontSize),
        underlineY,
        timerX - (2 * LW * fontSize),
        underlineY,
        bruceConfig.priColor
    );
}

void Timer::underlineMinutes() {
    tft.drawLine(
        timerX - (LW * fontSize), underlineY, timerX + (LW * fontSize), underlineY, bruceConfig.priColor
    );
}

void Timer::underlineSeconds() {
    tft.drawLine(
        timerX + (2 * LW * fontSize),
        underlineY,
        timerX + (4 * LW * fontSize),
        underlineY,
        bruceConfig.priColor
    );
}
