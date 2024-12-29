#include "bq27220.h"

BQ27220::BQ27220() : addr{BQ27220_I2C_ADDRESS}, wire(&Wire), scl(BQ27220_I2C_SCL), sda(BQ27220_I2C_SDA) {}

bool BQ27220::unseal()
{
    OP_STATUS status;

    writeCtrlWord(BQ27220_UNSEAL_KEY1);
    delayMicroseconds(5000);
    writeCtrlWord(BQ27220_UNSEAL_KEY2);
    delayMicroseconds(5000);
    status = OP_STATUS(readWord(BQ27220_CONTROL_CONTROL_STATUS));
    if(status = OP_STATUS::UNSEALED)
    {
        return true;
    }
    return false;
}

bool BQ27220::seal()
{
    OP_STATUS status;

    writeCtrlWord(BQ27220_CONTROL_SEALED);
    delayMicroseconds(5000);
    status = OP_STATUS(readWord(BQ27220_CONTROL_CONTROL_STATUS));
    if(status = OP_STATUS::SEALED)
    {
        return true;
    }
    return false;
}

uint16_t BQ27220::getTemp()
{
    return readWord(BQ27220_COMMAND_TEMP);
}

uint16_t BQ27220::getBatterySt(void)
{
    return readWord(BQ27220_COMMAND_BATTERY_ST);
}

bool BQ27220::getIsCharging(void)
{
    uint16_t ret = readWord(BQ27220_COMMAND_BATTERY_ST);
    bat_st.full = ret;
    return !bat_st.st.DSG;
}

uint16_t BQ27220::getTimeToEmpty()
{
    return readWord(BQ27220_COMMAND_TTE);
}

uint16_t BQ27220::getRemainCap()
{
    return readWord(BQ27220_COMMAND_REMAIN_CAPACITY);
}

uint16_t BQ27220::getFullChargeCap(void)
{
    return readWord(BQ27220_COMMAND_FCHG_CAPATICY);
}

uint16_t BQ27220::getChargePcnt(void)
{
    return readWord(BQ27220_COMMAND_STATE_CHARGE);
}

uint16_t BQ27220::getAvgPower(void)
{
    return readWord(BQ27220_COMMAND_AVG_PWR);
}

uint16_t BQ27220::getStandbyCur(void)
{
    return readWord(BQ27220_COMMAND_STANDBY_CURR);
}

uint16_t BQ27220::getVolt(VOLT_MODE type)
{
    switch (type)
    {
    case VOLT:
        return readWord(BQ27220_COMMAND_VOLT);
        break;
    case VOLT_CHARGING:
        return readWord(BQ27220_COMMAND_CHARGING_VOLT);
        break;
    case VOLT_RWA:
        return readWord(BQ27220_COMMAND_RAW_VOLT);
        break;
    default:
        break;
    }
    return 0;
}

int16_t BQ27220::getCurr(CURR_MODE type)
{
    switch (type)
    {
    case CURR_RAW:
        return (int16_t)readWord(BQ27220_COMMAND_RAW_CURR);
        break;
    case CURR_INSTANT:
        return (int16_t)readWord(BQ27220_COMMAND_CURR);
        break;
    case CURR_STANDBY:
        return (int16_t)readWord(BQ27220_COMMAND_STANDBY_CURR);
        break;
    case CURR_CHARGING:
        return (int16_t)readWord(BQ27220_COMMAND_CHARGING_CURR);
        break;
    case CURR_AVERAGE:
        return (int16_t)readWord(BQ27220_COMMAND_AVG_CURR);
        break;
    default:
        break;
    }
    return -1;
}

uint16_t BQ27220::readWord(uint16_t subAddress)
{
    uint8_t data[2];
    i2cReadBytes(subAddress, data, 2);
    return ((uint16_t)data[1] << 8) | data[0];
}

uint16_t BQ27220::getId()
{
    return 0x0220;
}

uint16_t BQ27220::readCtrlWord(uint16_t fun)
{
    uint8_t msb = (fun >> 8);
    uint8_t lsb = (fun & 0x00FF);
    uint8_t cmd[2] = {lsb, msb};
    uint8_t data[2] = {0};

    i2cWriteBytes((uint8_t)BQ27220_COMMAND_CONTROL, cmd, 2);

    if (i2cReadBytes((uint8_t)0, data, 2))
    {
        return ((uint16_t)data[1] << 8) | data[0];
    }
    return 0;
}


uint16_t BQ27220::writeCtrlWord(uint16_t fun)
{
    uint8_t msb = (fun >> 8);
    uint8_t lsb = (fun & 0x00FF);
    uint8_t cmd[2] = {lsb, msb};
    uint8_t data[2] = {0};

    i2cWriteBytes((uint8_t)BQ27220_COMMAND_CONTROL, cmd, 2);
    return 0;
}

bool BQ27220::i2cReadBytes(uint8_t subAddress, uint8_t *dest, uint8_t count)
{
    wire->beginTransmission(addr);
    wire->write(subAddress);
    wire->endTransmission(true);

    wire->requestFrom(addr, count);
    for (int i = 0; i < count; i++)
    {
        dest[i] = wire->read();
    }
    return true;
}

bool BQ27220::i2cWriteBytes(uint8_t subAddress, uint8_t *src, uint8_t count)
{
    wire->beginTransmission(addr);
    wire->write(subAddress);
    for (int i = 0; i < count; i++)
    {
        wire->write(src[i]);
    }
    wire->endTransmission(true);
    return true;
}