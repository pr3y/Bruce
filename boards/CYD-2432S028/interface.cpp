#include "interface.h"
#include "core/powerSave.h"
#include "core/utils.h"

#if XPT2046_SPI_BUS_MOSI_IO_NUM==TFT_MOSI
    #include <XPT2046_Touchscreen.h>
    #include <SPI.h>
    #include <globals.h>
    XPT2046_Touchscreen touch(XPT2046_SPI_CONFIG_CS_GPIO_NUM, XPT2046_TOUCH_CONFIG_INT_GPIO_NUM);
    #define XPT2046_CS 33
#elif defined(HAS_CAPACITIVE_TOUCH)
    #include "CYD28_TouchscreenC.h"
    #define CYD28_DISPLAY_HOR_RES_MAX 240
    #define CYD28_DISPLAY_VER_RES_MAX 320
    CYD28_TouchC touch(CYD28_DISPLAY_HOR_RES_MAX, CYD28_DISPLAY_VER_RES_MAX);
#else
    #include "CYD28_TouchscreenR.h"
    #define CYD28_DISPLAY_HOR_RES_MAX 320
    #define CYD28_DISPLAY_VER_RES_MAX 240  
    CYD28_TouchR touch(CYD28_DISPLAY_HOR_RES_MAX, CYD28_DISPLAY_VER_RES_MAX);
    #if defined(TOUCH_XPT2046_SPI)
        #define XPT2046_CS XPT2046_SPI_CONFIG_CS_GPIO_NUM
    #else
        #define XPT2046_CS 33
    #endif
#endif

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() { 
    #ifndef HAS_CAPACITIVE_TOUCH // Capacitive Touchscreen uses I2C to communicate
        pinMode(XPT2046_CS, OUTPUT);
    #endif

    #if XPT2046_SPI_BUS_MOSI_IO_NUM!=TFT_MOSI // Devices that doesn't share SPI bus
    if(!touch.begin()) {
        Serial.println("Touch IC not Started");
        log_i("Touch IC not Started");
    } else log_i("Touch IC Started");
    #endif

    #ifndef HAS_CAPACITIVE_TOUCH // Capacitive Touchscreen uses I2C to communicate
        digitalWrite(XPT2046_CS, LOW);
    #endif

}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() { 
    // Brightness control must be initialized after tft in this case @Pirata
    pinMode(TFT_BL,OUTPUT);
    ledcSetup(TFT_BRIGHT_CHANNEL,TFT_BRIGHT_FREQ, TFT_BRIGHT_Bits); //Channel 0, 10khz, 8bits
    ledcAttachPin(TFT_BL, TFT_BRIGHT_CHANNEL);
    ledcWrite(TFT_BRIGHT_CHANNEL,255);

    #if XPT2046_SPI_BUS_MOSI_IO_NUM==TFT_MOSI // Devices that DO share SPI bus
    if(!touch.begin(tft.getSPIinstance())) {
        Serial.println("Touch IC not Started");
        log_i("Touch IC not Started");
    } else log_i("Touch IC Started");
    #endif
}

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) { 
    int dutyCycle;
    if (brightval==100) dutyCycle=255;
    else if (brightval==75) dutyCycle=130;
    else if (brightval==50) dutyCycle=70;
    else if (brightval==25) dutyCycle=20;
    else if (brightval==0) dutyCycle=0;
    else dutyCycle = ((brightval*255)/100);

    log_i("dutyCycle for bright 0-255: %d",dutyCycle);
    ledcWrite(TFT_BRIGHT_CHANNEL,dutyCycle); // Channel 0
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    if (touch.touched()) { //touch.tirqTouched() &&
        #if XPT2046_SPI_BUS_MOSI_IO_NUM==TFT_MOSI // Devices that DO share SPI bus
            auto t = touch.getPoint();
            t = touch.getPoint();
        #else // Devices that don't share SPI bus
            auto t = touch.getPointScaled();
            t = touch.getPointScaled();
        #endif
        if(bruceConfig.rotation==3) {
            t.y = (tftHeight+20)-t.y;
            t.x = tftWidth-t.x;
        }
        if(bruceConfig.rotation==0) {
            int tmp=t.x;
            t.x = tftWidth-t.y;
            t.y = tmp;
        }
        if(bruceConfig.rotation==2) {
            int tmp=t.x;
            t.x = t.y;
            t.y = (tftHeight+20)-tmp;
        }

        if(!wakeUpScreen()) AnyKeyPress = true;
        else goto END;

        // Touch point global variable
        touchPoint.x = t.x;
        touchPoint.y = t.y;
        touchPoint.pressed=true;
        touchHeatMap(touchPoint);

    }
    END:
    if(AnyKeyPress) {
      long tmp=millis();
      while((millis()-tmp)<200 && (touch.touched())) delay(50);
    }
}

/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() { 
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0,LOW); 
    esp_deep_sleep_start();
}


/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to tornoff the device (name is odd btw)
**********************************************************************/
void checkReboot() { }