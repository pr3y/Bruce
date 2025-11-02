#include "bq27220.h"

BQ27220::BQ27220() : addr{BQ27220_I2C_ADDRESS}, wire(&Wire), scl(BQ27220_I2C_SCL), sda(BQ27220_I2C_SDA) {}

bool BQ27220::unseal()
{
    OP_STATUS status;

    writeCtrlWord(BQ27220_UNSEAL_KEY1);
    delayMicroseconds(5000);
    writeCtrlWord(BQ27220_UNSEAL_KEY2);
    delayMicroseconds(5000);
    status = OP_STATUS(getOperationStatus() >> 1 & 0b11);
    if (status == OP_STATUS::SEALED) { return false; }
    return true;
}

bool BQ27220::seal()
{
    OP_STATUS status;

    writeCtrlWord(BQ27220_CONTROL_SEALED);
    delayMicroseconds(5000);
    status = OP_STATUS(getOperationStatus() >> 1 & 0b11);
    if (status == OP_STATUS::SEALED) { return true; }
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
    return readWord(BQ27220_COMMAND_FCHG_CAPACITY);
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

uint16_t BQ27220::getDesignCap(void){ return readWord(BQ27220_COMMAND_DESIGN_CAPACITY); }

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

uint16_t BQ27220::writeWord(uint8_t addr, uint16_t fun) {
    uint8_t msb = (fun >> 8);
    uint8_t lsb = (fun & 0x00FF);
    uint8_t cmd[2] = {lsb, msb};
    uint8_t data[2] = {0};

    i2cWriteBytes((uint8_t)addr, cmd, 2);
    return 0;
}

uint16_t BQ27220::getOperationStatus() { return readWord(BQ27220_COMMAND_OPERATION_STATUS); }

// refer https://www.ti.com/lit/ug/sluubd4a/sluubd4a.pdf
bool BQ27220::setDesignCap(uint16_t capval) {
    bool isSeal = false;
    if (OP_STATUS(getOperationStatus() >> 1 & 0b11) == OP_STATUS::SEALED) {
        isSeal = true;
        if (!unseal()) return false;
    }
    writeCtrlWord(BQ27220_SUPER_KEY);
    delayMicroseconds(2000);
    writeCtrlWord(BQ27220_SUPER_KEY);
    delayMicroseconds(2000);
    writeCtrlWord(BQ27220_CONTROL_ENTER_CFG_UPDATE);
    delayMicroseconds(2000);
    uint8_t num = 0;
    while ((getOperationStatus() >> 10 & 1) == 0) {
        if (num++ > 10) return false;
        delay(100);
    }
    // design capacity
    writeWord((uint8_t)BQ27220_COMMAND_CHANGE_RAM, BQ27220_ADDRESS_CEDV1_DESIGN_CAPACITY);
    delayMicroseconds(2000);
    uint16_t chksum_len = readWord(BQ27220_COMMAND_MAC_DATA_SUM_LEN);
    // re select
    writeWord((uint8_t)BQ27220_COMMAND_CHANGE_RAM, BQ27220_ADDRESS_CEDV1_DESIGN_CAPACITY);
    delayMicroseconds(2000);
    uint16_t oldVal = readWord(BQ27220_COMMAND_MAC_DATA);
    writeWord(BQ27220_COMMAND_MAC_DATA, capval >> 8 | capval << 8);
    delayMicroseconds(2000);
    uint8_t newChkSum = calculateCheckSum(chksum_len & 0x00FF, oldVal, capval);
    writeWord(BQ27220_COMMAND_MAC_DATA_SUM_LEN, chksum_len & 0xFF00 | newChkSum);
    delayMicroseconds(2000);
    // full capacity
    writeWord((uint8_t)BQ27220_COMMAND_CHANGE_RAM, BQ27220_ADDRESS_CEDV1_FULL_CAPACITY);
    delayMicroseconds(2000);
    chksum_len = readWord(BQ27220_COMMAND_MAC_DATA_SUM_LEN);
    writeWord((uint8_t)BQ27220_COMMAND_CHANGE_RAM, BQ27220_ADDRESS_CEDV1_FULL_CAPACITY);
    delayMicroseconds(2000);
    oldVal = readWord(BQ27220_COMMAND_MAC_DATA);
    writeWord(BQ27220_COMMAND_MAC_DATA, capval >> 8 | capval << 8);
    delayMicroseconds(2000);
    newChkSum = calculateCheckSum(chksum_len & 0x00FF, oldVal, capval);
    writeWord(BQ27220_COMMAND_MAC_DATA_SUM_LEN, chksum_len & 0xFF00 | newChkSum);
    delayMicroseconds(2000);
    writeCtrlWord(BQ27220_CONTROL_EXIT_CFG_UPDATE_REINIT);
    delayMicroseconds(2000);
    num = 0;
    while ((getOperationStatus() >> 10 & 1) != 0) {
        if (num++ > 10) return false;
        delay(100);
    }
    if (isSeal && !seal()) return false;
    return true;
}

uint8_t BQ27220::calculateCheckSum(uint8_t OldChkSum, uint16_t OldVal, uint16_t NewVal) {
    return 255 - (uint8_t)(((uint16_t)(255 - OldChkSum - (OldVal >> 8) - (OldVal & 0x00FF)) % 256 +
                            (NewVal >> 8) + (NewVal & 0x00FF)) %
                           256);
}
