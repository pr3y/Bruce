#ifndef _PCA9555_H
#define _PCA9555_H

#include <Arduino.h>
#include <Wire.h>

#define PCA9555_DEFAULT_ADDR 0x20

class PCA9555 {
public:
    PCA9555();
    ~PCA9555();

    bool begin(uint8_t address = PCA9555_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool outputGPIO(uint16_t pins);
    uint16_t inputGPIO(void);
    bool configureDirection(uint16_t pins);

    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, bool val);
    bool digitalRead(uint8_t pin);

private:
    uint8_t _i2caddr;
    TwoWire *_wire;

    uint8_t output_state[2];    // espelha saída
    uint8_t direction_state[2]; // espelha direção

    bool writeRegister(uint8_t reg, uint8_t value);
    bool readRegister(uint8_t reg, uint8_t &value);
};

#endif
