/**
 * @file timer.h
 * @author Aleksei Gor (https://github.com/AlekseiGor)
 * @brief Timer
 * @version 0.1
 * @date 2024-12-07
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <globals.h>
class Timer {
private:
    int fontSize = 4;
    int duration = 0;
    int timerX = tftWidth / 2;
    int timerY = tftHeight / 2;
    int underlineY = timerY + (fontSize + 1) * LH;

    void clearUnderline();
    void underlineHours();
    void underlineMinutes();
    void underlineSeconds();

public:
    Timer();
    ~Timer();

    void setup();
    void loop();
};

#endif
