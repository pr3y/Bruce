/**
 * @file timer.cpp
 * @author Aleksei Gor (https://github.com/AlekseiGor)
 * @brief Timer
 * @version 0.1
 * @date 2024-11-26
 */

#include "core/display.h"

int iconX = WIDTH/2 - 40;
int iconY = 27 + (HEIGHT-134)/2;

void timerLoop() {

    tft.fillScreen(bruceConfig.bgColor);
    delay(300);

    for (;;) {

        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        tft.drawRect(10, 10, WIDTH-15,HEIGHT-15, bruceConfig.priColor);
        tft.setCursor(64, HEIGHT/3+5);
        tft.setTextSize(4);
        tft.drawCentreString("00:00:00", WIDTH/2, HEIGHT/2-13, 1);

        if(checkSelPress() or checkEscPress()) {
            tft.fillScreen(bruceConfig.bgColor);
            returnToMenu=true;
            break;
        }

        delay(10);
    }
}

void timerSetup() {

    timerLoop();

}
