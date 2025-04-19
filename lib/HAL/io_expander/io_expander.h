#ifndef _IO_EXPANDER_H
#define _IO_EXPANDER_H
#include <Arduino.h>
#include <Wire.h>
#ifdef IO_EXPANDER_AW9523
#include "Adafruit_AW9523.h"
#define IO_EXP_CLASS Adafruit_AW9523
#endif

#ifndef IO_EXP_GPS
#define IO_EXP_GPS -1
#endif
#ifndef IO_EXP_MIC
#define IO_EXP_MIC -1
#endif
#ifndef IO_EXP_VIBRO
#define IO_EXP_VIBRO -1
#endif
#ifndef IO_EXP_CC_RX
#define IO_EXP_CC_RX -1
#endif
#ifndef IO_EXP_CC_TX
#define IO_EXP_CC_TX -1
#endif

#if defined(IO_EXPANDER_AW9523) // || defined(IO_EXPANDER_xxxx)

#define IO_EXPANDER_ADDRESS AW9523_DEFAULT_ADDR // 0x58

class io_expander : public IO_EXP_CLASS {
private:
    bool _started = false;
    /* data */
public:
    io_expander() : IO_EXP_CLASS(/* args */) {};
    //~io_expander() { IO_EXP_CLASS::~IO_EXP_CLASS(); };
    void turnPinOnOff(int8_t pin, bool val) {
        if (!_started) return;
        return pin >= 0 ? Adafruit_AW9523::digitalWrite(pin, val) : delay(0);
    };
    bool init(uint8_t a, TwoWire *_w) {
        _started = begin(a, _w);
        configureDirection(0xFFFF); // All outputs
        turnPinOnOff(IO_EXP_GPS, LOW);
        turnPinOnOff(IO_EXP_MIC, LOW);
        turnPinOnOff(IO_EXP_VIBRO, LOW);
        turnPinOnOff(IO_EXP_CC_RX, LOW);
        turnPinOnOff(IO_EXP_CC_TX, LOW);
        return _started;
    };
    void setPinDirection(uint8_t pin, uint8_t mode) { pinMode(pin, mode); }
    bool readPin(int8_t pin) { return digitalRead(pin); }
};
#else
#define IO_EXPANDER_ADDRESS 0
// dummy class
class io_expander {
private:
    /* data */
public:
    io_expander() {};
    ~io_expander() {};
    void turnPinOnOff(int8_t pin, bool val) {};
    bool init(uint8_t a, TwoWire *_w) { return false; };
    void setPinDirection(uint8_t pin, uint8_t mode) {}
    bool readPin(int8_t pin) { return false; }
};

#endif // #ifdef IO_EXPANDER_AW9523
#endif // _IO_EXPANDER_H
