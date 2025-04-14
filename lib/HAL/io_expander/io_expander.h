#ifndef _IO_EXPANDER_H
#define _IO_EXPANDER_H
#include <Arduino.h>
#include <Wire.h>
#ifdef IO_EXPANDER_AW9523
#include "Adafruit_AW9523_2.h"
#define IO_EXP_CLASS Adafruit_AW9523
#endif

#if defined(IO_EXPANDER_AW9523) // || defined(IO_EXPANDER_xxxx)

#ifndef IO_EXP_GPS
#warning "define IO_EXP_GPS in your board file"
#define IO_EXP_GPS -1
#endif
#ifndef IO_EXP_MIC
#warning "define IO_EXP_MIC in your board file"
#define IO_EXP_MIC -1
#endif
#ifndef IO_EXP_VIBRO
#warning "define IO_EXP_VIBRO in your board file"
#define IO_EXP_VIBRO -1
#endif
#ifndef IO_EXP_CC_RX
#warning "define IO_EXP_CC_RX in your board file"
#define IO_EXP_CC_RX -1
#endif
#ifndef IO_EXP_CC_TX
#warning "define IO_EXP_CC_TX in your board file"
#define IO_EXP_CC_TX -1
#endif

#define IO_EXPANDER_ADDRESS AW9523_DEFAULT_ADDR // 0x58

class io_expander : public IO_EXP_CLASS {
private:
    /* data */
public:
    io_expander() : IO_EXP_CLASS(/* args */) {};
    //~io_expander() { IO_EXP_CLASS::~IO_EXP_CLASS(); };
    void turnPinOnOff(int8_t pin, bool val) {
        return pin >= 0 ? Adafruit_AW9523::digitalWrite(pin, val) : delay(0);
    };
};
#else
// dummy class
class io_expander {
private:
    /* data */
public:
    io_expander() {};
    ~io_expander() {};
    bool begin(uint8_t address, TwoWire *wire = &Wire) { return false; };
    bool reset(void) { return false; };
    bool openDrainPort0(bool od) { return false; };

    // All 16 pins at once
    bool outputGPIO(uint16_t pins) { return false; };
    uint16_t inputGPIO(void) { return 0; };
    bool configureDirection(uint16_t pins) { return false; };
    bool configureLEDMode(uint16_t pins) { return false; };
    bool interruptEnableGPIO(uint16_t pins) { return false; };

    // Individual pin control
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, bool val);
    bool digitalRead(uint8_t pin) { return false; };
    void analogWrite(uint8_t pin, uint8_t val);
    void enableInterrupt(uint8_t pin, bool en);
};

#endif // #ifdef IO_EXPANDER_AW9523
#endif // _IO_EXPANDER_H
