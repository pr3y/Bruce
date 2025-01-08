#include <stdint.h>
#include <Wire.h>
#define PMICLIB_I2C_MASTER_SPEED 400000

#define ATTR_NOT_IMPLEMENTED __attribute__((error("Not implemented")))
#define IS_BIT_SET(val, mask) (((val) & (mask)) == (mask))

typedef int (*iic_fptr_t)(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
namespace HAL
{
    namespace PMIC
    {
        template <class Type, class Driver>
        class Base : public class Type<class Driver>
        {
            friend class Type<Driver>;
            friend class Driver;

        public:
            virtual uint8_t getChipID() = 0;
            virtual bool init() = 0;
            virtual void deinit() = 0;
            virtual void shutdown() = 0;
            virtual uint8_t getBatteryPercent() { return -1; }
            virtual uint16_t getBattVoltage() { return 0; }
            virtual bool isBatteryConnect() { return false; }
            virtual bool isVbusIn() { return false; }
            virtual bool isCharging() { return false; }
            virtual uint16_t getSystemVoltage() = 0;
            virtual uint16_t getVbusVoltage() = 0;
            virtual bool setSysPowerDownVoltage(uint16_t millivolt) = 0;
            virtual uint16_t getSysPowerDownVoltage() = 0;

            bool begin(TwoWire &w, uint8_t addr, int sda, int scl)
            {
                if (started)
                    return thisChip().initImpl();
                started = true;
                mySDA = sda;
                mySCL = scl;
                myWire = &w;
                myWire->begin(mySDA, mySCL);
                myADDR = addr;
                return thisChip().initImpl();
            }

            bool begin(uint8_t addr, iic_fptr_t readRegCallback, iic_fptr_t writeRegCallback)
            {
                if (started)
                    return thisChip().initImpl();
                started = true;
                thisReadRegCallback = readRegCallback;
                thisWriteRegCallback = writeRegCallback;
                myADDR = addr;
                return thisChip().initImpl();
            }

            int readRegister(uint8_t reg)
            {
                uint8_t val = 0;
                return readRegister(reg, &val, 1) == -1 ? -1 : val;
            }

            int writeRegister(uint8_t reg, uint8_t val)
            {
                return writeRegister(reg, &val, 1);
            }

            int readRegister(uint8_t reg, uint8_t *buf, uint8_t length)
            {
                if (thisReadRegCallback)
                {
                    return thisReadRegCallback(myADDR, reg, buf, length);
                }
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
                if (thisWriteRegCallback)
                {
                    return thisWriteRegCallback(myADDR, reg, buf, length);
                }
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

            uint16_t inline readRegisterH8L4(uint8_t highReg, uint8_t lowReg)
            {
                int h8 = readRegister(highReg);
                int l4 = readRegister(lowReg);
                if (h8 == -1 || l4 == -1)
                    return 0;
                return (h8 << 4) | (l4 & 0x0F);
            }

            uint16_t inline readRegisterH8L5(uint8_t highReg, uint8_t lowReg)
            {
                int h8 = readRegister(highReg);
                int l5 = readRegister(lowReg);
                if (h8 == -1 || l5 == -1)
                    return 0;
                return (h8 << 5) | (l5 & 0x1F);
            }

            uint16_t inline readRegisterH6L8(uint8_t highReg, uint8_t lowReg)
            {
                int h6 = readRegister(highReg);
                int l8 = readRegister(lowReg);
                if (h6 == -1 || l8 == -1)
                    return 0;
                return ((h6 & 0x3F) << 8) | l8;
            }

            uint16_t inline readRegisterH5L8(uint8_t highReg, uint8_t lowReg)
            {
                int h5 = readRegister(highReg);
                int l8 = readRegister(lowReg);
                if (h5 == -1 || l8 == -1)
                    return 0;
                return ((h5 & 0x1F) << 8) | l8;
            }

            /*
             * CRTP Helper
             */
        protected:
            bool begin()
            {
                if (started)
                    return thisChip().initImpl();
                started = true;
                if (myWire)
                {
                    log_i("SDA:%d SCL:%d", mySDA, mySCL);
                    myWire->begin(mySDA, mySCL);
                }
                return thisChip().initImpl();
            }

            void end()
            {
                if (myWire)
                {
                    myWire->end();
                }
            }

            inline const Driver &thisChip() const
            {
                return static_cast<const Driver &>(*this);
            }

            inline Driver &thisChip()
            {
                return static_cast<Driver &>(*this);
            }

            PMICChipModel getChipModel() { return myModel; }

        protected:
            static bool started = false;
            TwoWire *myWire = NULL;
            int mySDA = -1;
            int mySCL = -1;
            uint8_t myADDR = 0xFF;
            iic_fptr_t thisReadRegCallback = NULL;
            iic_fptr_t thisWriteRegCallback = NULL;
            PMICChipModel myModel = UNDEFINED;
        };
    }
}