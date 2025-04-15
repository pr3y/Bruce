#ifndef _ADAFRUIT_AW9523_2H
#define _ADAFRUIT_AW9523_2H

#include "Arduino.h"
#include "Wire.h"

#define AW9523_DEFAULT_ADDR 0x58 ///< Default I2C address for AW9523

#define AW9523_LED_MODE 0x99 ///< Special pinMode() macro for constant current mode

#define AW9523_REG_CHIPID 0x10
#define AW9523_REG_SOFTRESET 0x7F
#define AW9523_REG_INPUT0 0x00
#define AW9523_REG_OUTPUT0 0x02
#define AW9523_REG_CONFIG0 0x04
#define AW9523_REG_INTENABLE0 0x06
#define AW9523_REG_GCR 0x11
#define AW9523_REG_LEDMODE 0x12

class Adafruit_AW9523 {
public:
    Adafruit_AW9523();
    ~Adafruit_AW9523();

    bool begin(uint8_t address = AW9523_DEFAULT_ADDR, TwoWire *wire = &Wire);
    bool reset(void);
    bool openDrainPort0(bool od);

    bool outputGPIO(uint16_t pins);
    uint16_t inputGPIO(void);
    bool configureDirection(uint16_t pins);
    bool configureLEDMode(uint16_t pins);
    bool interruptEnableGPIO(uint16_t pins);

    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, bool val);
    bool digitalRead(uint8_t pin);
    void analogWrite(uint8_t pin, uint8_t val);
    void enableInterrupt(uint8_t pin, bool en);

private:
    uint8_t _i2caddr;
    TwoWire *_wire;

    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t &value);
};

#endif
