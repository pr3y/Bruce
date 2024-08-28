#pragma once
#if defined (CORE2)
#ifndef _M5Core2_H_
#define _M5Core2_H_

#if defined(ESP32)

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "FS.h"
#include "SD.h"

#include "M5Display.h"
#include "M5Touch.h"           // M5Touch
#include "utility/M5Button.h"  // M5Buttons, M5Events, Button, Gesture
#include "utility/Config.h"
#include "utility/CommUtil.h"
//#include "utility/MPU6886.h"
//#include "Speaker.h"
#include "AXP.h"
//#include "RTC.h"

class M5Core2 {
   public:
    M5Core2();
    //[[deprecated("It is recommended to use M5Unified Lib")]]

    void begin(bool LCDEnable = true, bool SDEnable = true,
               bool SerialEnable = true, bool I2CEnable = false,
               mbus_mode_t mode = kMBusModeOutput, bool SpeakerEnable = false);
    void update();

    void shutdown();
    int shutdown(int seconds);
    //int shutdown(const RTC_TimeTypeDef &RTC_TimeStruct);
    //int shutdown(const RTC_DateTypeDef &RTC_DateStruct,
    //             const RTC_TimeTypeDef &RTC_TimeStruct);

    // LCD
    M5Display Lcd;

    // Power
    AXP Axp;

    // Touch
    M5Touch Touch;

    // Buttons (global button and gesture functions)
    M5Buttons Buttons;

    // Default "button" that gets events where there is no button.
    Button background = Button(0, 0, TOUCH_W, TOUCH_H, true, "background");

    // Touch version of the buttons on older M5stack cores, below screen
    Button BtnA = Button(10, 220, 110, 60, true, "BtnA"); // from 240 to 220 and from 40 to 60
    Button BtnB = Button(130, 220, 70, 60, true, "BtnB"); // from 240 to 220 and from 40 to 60
    Button BtnC = Button(230, 220, 80, 60, true, "BtnC"); // from 240 to 220 and from 40 to 60

    //MPU6886 IMU;

    // I2C
    CommUtil I2C;

    //RTC Rtc;

    //Speaker Spk;

    /**
     * Functions have been moved to Power class for compatibility.
     * These will be removed in a future release.
     */
    void setPowerBoostKeepOn(bool en) __attribute__((deprecated));
    void setWakeupButton(uint8_t button) __attribute__((deprecated));
    void powerOFF() __attribute__((deprecated));

   private:
    bool isInited;
};

extern M5Core2 M5;
#define m5  M5
#define lcd Lcd
#else
#error "This library only supports boards with ESP32 processor."
#endif
#endif
#endif