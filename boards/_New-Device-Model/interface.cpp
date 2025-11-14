#include "core/powerSave.h"
#include <interface.h>

// يجب أن يكون هذا الملف قد تم تضمينه تلقائيًا إذا كانت #define USE_TFT_eSPI_TOUCH مفعلة
#if defined(USE_TFT_eSPI_TOUCH)
// TFT_eSPI و XPT2046 يتم التعامل معها في ملفات أخرى ويتم استدعاء tft.getTouch()
#define XPT2046_CS TOUCH_CS // نعتمد على TOUCH_CS (GPIO 33) المعرّف في platformio.ini
#endif

// دالة وهمية لتعريف بنية النقطة المطلوبة لـ tft.getTouch
struct TouchPoint {
    int x;
    int y;
    bool pressed = false;
};
TouchPoint touchPoint;
extern void touchHeatMap(TouchPoint); // دالة خارجية محتملة

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    // إعداد دبوس Touch CS و تهيئته (رغم أن TFT_eSPI قد تفعله داخليًا)
    #if defined(USE_TFT_eSPI_TOUCH)
        pinMode(XPT2046_CS, OUTPUT);
        digitalWrite(XPT2046_CS, HIGH);
    #endif
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() {
    // تهيئة دبابيس الإضاءة الخلفية PWM (يعتمد على TFT_BL=27 المعرّف في platformio.ini)
    pinMode(TFT_BL, OUTPUT);
    ledcSetup(TFT_BRIGHT_CHANNEL, TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits);
    ledcAttachPin(TFT_BL, TFT_BRIGHT_CHANNEL);
    ledcWrite(TFT_BRIGHT_CHANNEL, 255); // أعلى إضاءة عند التشغيل
}

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() {
    // لوحة CrowPanel لا تحتوي على دائرة قراءة للبطارية افتراضيًا، لذا نتركها صفر
    return 0;
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    // منطق التحكم في الإضاءة باستخدام PWM
    int dutyCycle;
    if (brightval == 100) dutyCycle = 255;
    else if (brightval == 75) dutyCycle = 130;
    else if (brightval == 50) dutyCycle = 70;
    else if (brightval == 25) dutyCycle = 20;
    else if (brightval == 0) dutyCycle = 0;
    else dutyCycle = ((brightval * 255) / 100);

    // log_i("dutyCycle for bright 0-255: %d", dutyCycle); // تم التعليق لتجنب log_i غير الضروري
    ledcWrite(TFT_BRIGHT_CHANNEL, dutyCycle); // القناة المعرفة في platformio.ini
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static long d_tmp = 0;
    checkPowerSaveTime();
    
    // reset inputs
    PrevPress = false;
    NextPress = false;
    SelPress = false;
    AnyKeyPress = false;
    EscPress = false;
    
    // زمن انتظار 200ms للتحكم في حساسية اللمس
    if (millis() - d_tmp > 200 || LongPress) {

        #if defined(USE_TFT_eSPI_TOUCH)
        
            TouchPoint t;
            // استخدام دالة المكتبة للقراءة (تعتمد على GPIO 33)
            bool _IH_touched = tft.getTouch(&t.x, &t.y); 

            if (_IH_touched) {
                // ... (نقوم بتصفير جميع المدخلات الأخرى هنا)
                
                // CrowPanel تستخدم دوران 1. يجب تعديل الإحداثيات هنا إذا كان هناك دوران آخر
                // الكود الأصلي يحتوي على منطق تحويل الدوران، نعتمد هنا على أن tft.getTouch يقوم بمعظمه
                
                if (!wakeUpScreen()) AnyKeyPress = true;
                else goto END;

                // تحديث النقطة العالمية
                touchPoint.x = t.x;
                touchPoint.y = t.y;
                touchPoint.pressed = true;
                touchHeatMap(touchPoint); // دالة خارجية لتسجيل اللمس
                
                d_tmp = millis(); // تحديث وقت آخر لمسة
            }
        #endif

        // شروط وهمية لأزرار الأجهزة الأخرى (نتركها false لأن CrowPanel تعتمد على اللمس)
        if (false /*Conditions fot all inputs*/) { 
            if (!wakeUpScreen()) AnyKeyPress = true;
            else goto END;
        }
        if (false /*Conditions for previous btn*/) { PrevPress = true; }
        if (false /*Conditions for Next btn*/) { NextPress = true; }
        if (false /*Conditions for Esc btn*/) { EscPress = true; }
        if (false /*Conditions for Select btn*/) { SelPress = true; }
    }
END:
    // جزء التحكم في فترة تصفير المدخلات
    if (AnyKeyPress) {
        long tmp = millis();
        while ((millis() - tmp) < 200 && false /*Conditions fot all inputs*/);
    }
}

/*********************************************************************
** Function: keyboard
** location: mykeyboard.cpp
** Starts keyboard to type data
**********************************************************************/
String keyboard(String mytext, int maxSize, String msg) {
    // هذه الدالة تتعلق بمنطق واجهة المستخدم (UI) وليست خاصة بالدبابيس
    // لا نغيرها ونترك منطقها في ملف آخر (عادةً ما يرجع قيمة فارغة أو نص مُدخل)
    return ""; 
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {
    // يمكن تفعيل النوم العميق هنا
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, LOW);
    // esp_deep_sleep_start();
}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() {}