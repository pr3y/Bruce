#include "BUS/I2C/I2C_Device.tpp"
#include "PMIC.hpp"

#include <Wire.h>
#include <stdint.h>
#define PMICLIB_I2C_MASTER_SPEED 400000

#define ATTR_NOT_IMPLEMENTED     __attribute__((error("Not implemented")))
#define IS_BIT_SET(val, mask)    (((val) & (mask)) == (mask))

namespace HAL::PMIC
{
    using namespace HAL::BUS;
    template <class Driver>
    class Base : public I2C_Device<Driver>
    {
        friend class I2C_Device<Driver>;

    public:
        Base() : myModel(UNDEFINED) {};
        virtual uint8_t  getChipID() = 0;
        virtual bool     init()      = 0;
        virtual void     deinit()    = 0;
        virtual void     shutdown()  = 0;
        virtual uint8_t  getBatteryPercent() { return -1; }
        virtual uint16_t getBattVoltage() { return 0; }
        virtual bool     isBatteryConnect() { return false; }
        virtual bool     isVbusIn() { return false; }
        virtual bool     isCharging() { return false; }
        virtual uint16_t getSystemVoltage()                         = 0;
        virtual uint16_t getVbusVoltage()                           = 0;
        virtual bool     setSysPowerDownVoltage(uint16_t millivolt) = 0;
        virtual uint16_t getSysPowerDownVoltage()                   = 0;

        bool begin(TwoWire &w, uint8_t addr, int sda, int scl)
        {
            if (started)
                return thisChip().initImpl();
            started      = true;
            this->mySDA  = sda;
            this->mySCL  = scl;
            this->myWire = &w;
            this->myWire->begin(this->mySDA, this->mySCL);
            this->myADDR = addr;
            return thisChip().initImpl();
        }

        PMICChipModel getChipModel() { return myModel; }

        /*
         * CRTP Helper
         */
    protected:
        bool begin()
        {
            if (started)
                return thisChip().initImpl();
            started = true;
            if (this->myWire)
            {
                log_i("SDA:%d SCL:%d", this->mySDA, this->mySCL);
                this->myWire->begin(this->mySDA, this->mySCL);
            }
            return thisChip().initImpl();
        }

        void end()
        {
            this->myWire->end();
        }

        inline const Driver &thisChip() const
        {
            return static_cast<const Driver &>(*this);
        }

        inline Driver &thisChip()
        {
            return static_cast<Driver &>(*this);
        }

        void setChipModel(PMICChipModel m) { setChipModel(m); }

    protected:
        bool          started = false;
        PMICChipModel myModel = UNDEFINED;
    };
} // namespace HAL::PMIC