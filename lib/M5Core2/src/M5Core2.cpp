#if defined (CORE2)
// Copyright (c) M5Core2. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "M5Core2.h"

M5Core2::M5Core2() : isInited(0) {
}

void M5Core2::begin(bool LCDEnable, bool SDEnable, bool SerialEnable,
                    bool I2CEnable, mbus_mode_t mode, bool SpeakerEnable) {
    // Correct init once
    if (isInited == true) {
        return;
    } else {
        isInited = true;
    }

    // UART
    if (SerialEnable == true) {
        Serial.begin(115200);
        Serial.flush();
        delay(50);
        Serial.print("M5Core2 initializing...");
    }

    // I2C init
    if (I2CEnable == true) {
        Wire.begin(32, 33);
    }

    Axp.begin(mode);

    // LCD INIT
    if (LCDEnable == true) {
        Lcd.begin();
    }

    // Touch init
    Touch.begin();  // Touch begin after AXP begin. (Reset at the start of AXP)

    // TF Card
    if (SDEnable == true) {
        SD.begin(TFCARD_CS_PIN, SPI, 40000000);
    }

    // TONE

//    if (SpeakerEnable == true) {
//        Spk.begin();
//    }

    if (SerialEnable == true) {
        Serial.println("OK");
    }

//    Rtc.begin();
}

void M5Core2::update() {
    Touch.update();
    Buttons.update();
    yield();
}

void M5Core2::shutdown() {
    Axp.PowerOff();
}
/*
int M5Core2::shutdown(int seconds) {
    Rtc.clearIRQ();
    Rtc.SetAlarmIRQ(seconds);
    delay(10);
    Axp.PowerOff();
    return 0;
}

int M5Core2::shutdown(const RTC_TimeTypeDef &RTC_TimeStruct) {
    Rtc.clearIRQ();
    Rtc.SetAlarmIRQ(RTC_TimeStruct);
    delay(10);
    Axp.PowerOff();
    return 0;
}

int M5Core2::shutdown(const RTC_DateTypeDef &RTC_DateStruct,
                      const RTC_TimeTypeDef &RTC_TimeStruct) {
    Rtc.clearIRQ();
    Rtc.SetAlarmIRQ(RTC_DateStruct, RTC_TimeStruct);
    delay(10);
    Axp.PowerOff();
    return 0;
}
*/
M5Core2 M5;
#endif