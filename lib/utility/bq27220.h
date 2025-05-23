#ifndef __BQ27220_H__
#define __BQ27220_H__

#define BQ27220_I2C_ADDRESS 0x55 // device addr
#ifndef BQ27220_I2C_SDA
#define BQ27220_I2C_SDA
#endif
#ifndef BQ27220_I2C_SCL
#define BQ27220_I2C_SCL
#endif
#define BQ27220_DEVICE_ID 0x0220 // device id

// commands
#define BQ27220_COMMAND_CONTROL         0X00
#define BQ27220_COMMAND_TEMP            0X06
#define BQ27220_COMMAND_BATTERY_ST      0X0A
#define BQ27220_COMMAND_VOLT            0X08
#define BQ27220_COMMAND_BAT_STA         0X0A
#define BQ27220_COMMAND_CURR            0X0C
#define BQ27220_COMMAND_REMAIN_CAPACITY 0X10
#define BQ27220_COMMAND_FCHG_CAPACITY   0X12
#define BQ27220_COMMAND_AVG_CURR        0x14
#define BQ27220_COMMAND_TTE             0X16
#define BQ27220_COMMAND_TTF             0X18
#define BQ27220_COMMAND_STANDBY_CURR    0X1A
#define BQ27220_COMMAND_STTE            0X1C
#define BQ27220_COMMAND_MAX_LOAD_CURR   0x1E
#define BQ27220_COMMAND_MAX_LOAD_TTE    0x20
#define BQ27220_COMMAND_RAW_COULOMB_CNT 0x22
#define BQ27220_COMMAND_AVG_PWR         0x24
#define BQ27220_COMMAND_INT_TEMP        0x28
#define BQ27220_COMMAND_STATE_CHARGE    0X2C
#define BQ27220_COMMAND_STATE_HEALTH    0X2E
#define BQ27220_COMMAND_CHARGING_VOLT   0X30
#define BQ27220_COMMAND_CHARGING_CURR   0X32
#define BQ27220_COMMAND_OPERATION_STATUS 0x3A
#define BQ27220_COMMAND_DESIGN_CAPACITY 0x3C
#define BQ27220_COMMAND_CHANGE_RAM      0x3E
#define BQ27220_COMMAND_MAC_DATA        0x40
#define BQ27220_COMMAND_MAC_DATA_SUM_LEN 0x60
#define BQ27220_COMMAND_ANALOG_CURR     0x79
#define BQ27220_COMMAND_RAW_CURR        0X7A
#define BQ27220_COMMAND_RAW_VOLT        0X7C
#define BQ27220_COMMAND_RAW_TEMP        0x7E

#define BQ27220_CONTROL_CONTROL_STATUS         0x0000
#define BQ27220_CONTROL_DEVICE_NUMBER          0x0001
#define BQ27220_CONTROL_FW_VERSION             0x0002
#define BQ27220_CONTROL_HW_VERSION             0x0003
#define BQ27220_CONTROL_BOARD_OFFSET           0x0009
#define BQ27220_CONTROL_CC_OFFSET              0x000A
#define BQ27220_CONTROL_CC_OFFSET_SAVE         0x000B
#define BQ27220_CONTROL_OCV_CMD                0x000C
#define BQ27220_CONTROL_BAT_INSERT             0x000D
#define BQ27220_CONTROL_BAT_REMOVE             0x000E
#define BQ27220_CONTROL_SET_SNOOZE             0x0013
#define BQ27220_CONTROL_CLEAR_SNOOZE           0x0014
#define BQ27220_CONTROL_SET_PROFILE_1          0x0015
#define BQ27220_CONTROL_SET_PROFILE_2          0x0016
#define BQ27220_CONTROL_SET_PROFILE_3          0x0017
#define BQ27220_CONTROL_SET_PROFILE_4          0x0018
#define BQ27220_CONTROL_SET_PROFILE_5          0x0019
#define BQ27220_CONTROL_SET_PROFILE_6          0x001A
#define BQ27220_CONTROL_CAL_TOGGLE             0x002D
#define BQ27220_CONTROL_SEALED                 0x0030
#define BQ27220_CONTROL_RESET                  0x0041
#define BQ27220_CONTROL_OERATION_STATUS        0x0054
#define BQ27220_CONTROL_GAUGING_STATUS         0x0056
#define BQ27220_CONTROL_EXIT_CAL               0x0080
#define BQ27220_CONTROL_ENTER_CAL              0x0081
#define BQ27220_CONTROL_ENTER_CFG_UPDATE       0x0090
#define BQ27220_CONTROL_EXIT_CFG_UPDATE_REINIT 0x0091
#define BQ27220_CONTROL_EXIT_CFG_UPDATE        0x0092
#define BQ27220_CONTROL_RETURN_TO_ROM          0x0F00

#define BQ27220_ADDRESS_CEDV1_FULL_CAPACITY     0x929D
#define BQ27220_ADDRESS_CEDV1_DESIGN_CAPACITY   0x929F

#define BQ27220_UNSEAL_KEY1 0x0414
#define BQ27220_UNSEAL_KEY2 0x3672
#define BQ27220_SUPER_KEY   0xffff

#include <Arduino.h>
#include <Wire.h>

enum OP_STATUS{
    SEALED = 0b11,
    UNSEALED = 0b10,
    FULL = 0b01,
};

enum CURR_MODE
{
    CURR_RAW,
    CURR_INSTANT,
    CURR_STANDBY,
    CURR_CHARGING,
    CURR_AVERAGE,
};

enum VOLT_MODE
{
    VOLT,
    VOLT_CHARGING,
    VOLT_RWA
};

union battery_state
{
    struct __st
    {
        uint16_t DSG : 1;
        uint16_t SYSDWN : 1;
        uint16_t TDA : 1;
        uint16_t BATTPRES : 1;
        uint16_t AUTH_GD : 1;
        uint16_t OCVGD : 1;
        uint16_t TCA : 1;
        uint16_t RSVD : 1;
        uint16_t CHGING : 1;
        uint16_t FC : 1;
        uint16_t OTD : 1;
        uint16_t OTC : 1;
        uint16_t SLEEP : 1;
        uint16_t OCVFALL : 1;
        uint16_t OCVCOMP : 1;
        uint16_t FD : 1;
    } st;
    uint16_t full;
};

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
    uint16_t getDesignCap(void);
    bool setDesignCap(uint16_t capval);
    uint16_t getOperationStatus(void);

private:
    uint8_t addr;
    TwoWire *wire;
    int scl;
    int sda;
    union battery_state bat_st;
    bool i2cReadBytes(uint8_t subAddress, uint8_t *dest, uint8_t count);
    bool i2cWriteBytes(uint8_t subAddress, uint8_t *src, uint8_t count);
    uint16_t readWord(uint16_t subAddress);
    uint16_t readCtrlWord(uint16_t fun);
    uint16_t writeCtrlWord(uint16_t fun);
    uint16_t writeWord(uint8_t addr, uint16_t fun);
    uint8_t calculateCheckSum(uint8_t OldChkSum, uint16_t OldVal, uint16_t NewVal);
};

#endif
