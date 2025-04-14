#include "Adafruit_AW9523.h"

Adafruit_AW9523::Adafruit_AW9523() {}
Adafruit_AW9523::~Adafruit_AW9523() {}

bool Adafruit_AW9523::begin(uint8_t address, TwoWire *wire) {
    _i2caddr = address;
    _wire = wire;

    if (!reset()) return false;

    uint8_t chip_id = 0;
    if (!readRegister(AW9523_REG_CHIPID, chip_id)) return false;
    if (chip_id != 0x23) return false;

    configureDirection(0x0000); // all inputs
    openDrainPort0(false);
    interruptEnableGPIO(0x0000);

    return true;
}

bool Adafruit_AW9523::reset(void) { return writeRegister(AW9523_REG_SOFTRESET, 0x00); }

bool Adafruit_AW9523::outputGPIO(uint16_t pins) {
    return writeRegister(AW9523_REG_OUTPUT0, pins & 0xFF) &&
           writeRegister(AW9523_REG_OUTPUT0 + 1, (pins >> 8) & 0xFF);
}

uint16_t Adafruit_AW9523::inputGPIO(void) {
    uint8_t low = 0, high = 0;
    readRegister(AW9523_REG_INPUT0, low);
    readRegister(AW9523_REG_INPUT0 + 1, high);
    return (uint16_t)high << 8 | low;
}

bool Adafruit_AW9523::configureDirection(uint16_t pins) {
    return writeRegister(AW9523_REG_CONFIG0, ~(pins & 0xFF)) &&
           writeRegister(AW9523_REG_CONFIG0 + 1, ~(pins >> 8));
}

bool Adafruit_AW9523::configureLEDMode(uint16_t pins) {
    return writeRegister(AW9523_REG_LEDMODE, ~(pins & 0xFF)) &&
           writeRegister(AW9523_REG_LEDMODE + 1, ~(pins >> 8));
}

bool Adafruit_AW9523::interruptEnableGPIO(uint16_t pins) {
    return writeRegister(AW9523_REG_INTENABLE0, ~(pins & 0xFF)) &&
           writeRegister(AW9523_REG_INTENABLE0 + 1, ~(pins >> 8));
}

void Adafruit_AW9523::analogWrite(uint8_t pin, uint8_t val) {
    uint8_t reg;
    if (pin <= 7) {
        reg = 0x24 + pin;
    } else if (pin <= 11) {
        reg = 0x20 + (pin - 8);
    } else if (pin <= 15) {
        reg = 0x2C + (pin - 12);
    } else {
        return; // invalid pin
    }
    writeRegister(reg, val);
}

void Adafruit_AW9523::digitalWrite(uint8_t pin, bool val) {
    uint8_t reg = AW9523_REG_OUTPUT0 + (pin / 8);
    uint8_t out;
    readRegister(reg, out);
    if (val) out |= (1 << (pin % 8));
    else out &= ~(1 << (pin % 8));
    writeRegister(reg, out);
}

bool Adafruit_AW9523::digitalRead(uint8_t pin) {
    uint8_t reg = AW9523_REG_INPUT0 + (pin / 8);
    uint8_t val;
    if (!readRegister(reg, val)) return false;
    return (val >> (pin % 8)) & 0x1;
}

void Adafruit_AW9523::enableInterrupt(uint8_t pin, bool en) {
    uint8_t reg = AW9523_REG_INTENABLE0 + (pin / 8);
    uint8_t val;
    readRegister(reg, val);
    if (en) val &= ~(1 << (pin % 8));
    else val |= (1 << (pin % 8));
    writeRegister(reg, val);
}

void Adafruit_AW9523::pinMode(uint8_t pin, uint8_t mode) {
    uint8_t confreg = AW9523_REG_CONFIG0 + (pin / 8);
    uint8_t modereg = AW9523_REG_LEDMODE + (pin / 8);

    uint8_t conf, modeval;
    readRegister(confreg, conf);
    readRegister(modereg, modeval);

    if (mode == OUTPUT) {
        conf &= ~(1 << (pin % 8));
        modeval |= (1 << (pin % 8));
    } else if (mode == INPUT) {
        conf |= (1 << (pin % 8));
        modeval |= (1 << (pin % 8));
    } else if (mode == AW9523_LED_MODE) {
        conf &= ~(1 << (pin % 8));
        modeval &= ~(1 << (pin % 8));
    }

    writeRegister(confreg, conf);
    writeRegister(modereg, modeval);
}

bool Adafruit_AW9523::openDrainPort0(bool od) {
    uint8_t gcr;
    readRegister(AW9523_REG_GCR, gcr);
    if (od) gcr &= ~(1 << 4);
    else gcr |= (1 << 4);
    return writeRegister(AW9523_REG_GCR, gcr);
}

// Private methods
bool Adafruit_AW9523::writeRegister(uint8_t reg, uint8_t value) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->write(value);
    return (_wire->endTransmission() == 0);
}

bool Adafruit_AW9523::readRegister(uint8_t reg, uint8_t &value) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) return false;

    if (_wire->requestFrom((int)_i2caddr, 1) != 1) return false;

    value = _wire->read();
    return true;
}
