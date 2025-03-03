#include "../REG/BQ7220.hpp"
#include <Wire.h>
namespace HAL::FUEL
{
    class BQ27220
    {
    public:
        BQ27220();
        bool unseal();
        bool seal();
        uint16_t getTemp();
        uint16_t getBatterySt(void);
        bool getIsCharging(void);
        uint16_t getRemainCap();
        uint16_t getTimeToEmpty();
        uint16_t getFullChargeCap(void);
        uint16_t getChargePcnt(void);
        uint16_t getAvgPower(void);
        uint16_t getStandbyCur(void);
        uint16_t getVolt(VOLT_MODE type);
        int16_t getCurr(CURR_MODE type);
        uint16_t getId();

    private:
        TwoWire *wire;
        uint8_t addr;
        int scl;
        int sda;
        union battery_state bat_st;
        bool i2cReadBytes(uint8_t subAddress, uint8_t *dest, uint8_t count);
        bool i2cWriteBytes(uint8_t subAddress, uint8_t *src, uint8_t count);
        uint16_t readWord(uint16_t subAddress);
        uint16_t readCtrlWord(uint16_t fun);
        uint16_t writeCtrlWord(uint16_t fun);
    };
}