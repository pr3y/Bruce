#include <Wire.h>
#include <Arduino.h>
namespace HAL::BUS
{
    class I2C_Device
    {
        public:
        int readRegister(uint8_t reg)
        {
            uint8_t val = 0;
            return readRegister(reg, &val, 1) == -1 ? -1 : val;
        }

        int writeRegister(uint8_t reg, uint8_t val)
        {
            return writeRegister(reg, &val, 1);
        }

        int readRegister(uint8_t reg, uint8_t *buf, uint8_t length = 1)
        {
            if (myWire)
            {
                myWire->beginTransmission(myADDR);
                myWire->write(reg);
                if (myWire->endTransmission() != 0)
                {
                    return -1;
                }
                myWire->requestFrom(myADDR, length);
                return myWire->readBytes(buf, length) == length ? 0 : -1;
            }
            return -1;
        }

        int writeRegister(uint8_t reg, uint8_t *buf, uint8_t length)
        {
            if (myWire)
            {
                myWire->beginTransmission(myADDR);
                myWire->write(reg);
                myWire->write(buf, length);
                return (myWire->endTransmission() == 0) ? 0 : -1;
            }
            return -1;
        }

        bool inline clrRegisterBit(uint8_t registers, uint8_t bit)
        {
            int val = readRegister(registers);
            if (val == -1)
            {
                return false;
            }
            return writeRegister(registers, (val & (~_BV(bit)))) == 0;
        }

        bool inline setRegisterBit(uint8_t registers, uint8_t bit)
        {
            int val = readRegister(registers);
            if (val == -1)
            {
                return false;
            }
            return writeRegister(registers, (val | (_BV(bit)))) == 0;
        }

        bool inline getRegisterBit(uint8_t registers, uint8_t bit)
        {
            int val = readRegister(registers);
            if (val == -1)
            {
                return false;
            }
            return val & _BV(bit);
        }

        protected:
        TwoWire      *myWire  = NULL;
        int           mySDA   = -1;
        int           mySCL   = -1;
        uint8_t       myADDR  = 0xFF;
    };
}