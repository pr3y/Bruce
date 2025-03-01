#ifndef __DS3231__
#define __DS3231__

#include <Arduino.h>

// I2C Slave Address
const uint8_t DS3231_ADDRESS = 0x68;

// DS3231 Register Addresses
const uint8_t DS3231_REG_TIMEDATE = 0x00;
const uint8_t DS3231_REG_ALARMONE = 0x07;
const uint8_t DS3231_REG_ALARMTWO = 0x0B;

const uint8_t DS3231_REG_CONTROL = 0x0E;
const uint8_t DS3231_REG_STATUS  = 0x0F;
const uint8_t DS3231_REG_AGING   = 0x10;

const uint8_t DS3231_REG_TEMP = 0x11;

// DS3231 Register Data Size if not just 1
const size_t DS3231_REG_TIMEDATE_SIZE = 7;
const size_t DS3231_REG_ALARMONE_SIZE = 4;
const size_t DS3231_REG_ALARMTWO_SIZE = 3;

const size_t DS3231_REG_TEMP_SIZE = 2;

// DS3231 Control Register Bits
const uint8_t DS3231_A1IE    = 0;
const uint8_t DS3231_A2IE    = 1;
const uint8_t DS3231_INTCN   = 2;
const uint8_t DS3231_RS1     = 3;
const uint8_t DS3231_RS2     = 4;
const uint8_t DS3231_CONV    = 5;
const uint8_t DS3231_BBSQW   = 6;
const uint8_t DS3231_EOSC    = 7;
const uint8_t DS3231_AIEMASK = (_BV(DS3231_A1IE) | _BV(DS3231_A2IE));
const uint8_t DS3231_RSMASK  = (_BV(DS3231_RS1) | _BV(DS3231_RS2));

// DS3231 Status Register Bits
const uint8_t DS3231_A1F     = 0;
const uint8_t DS3231_A2F     = 1;
const uint8_t DS3231_BSY     = 2;
const uint8_t DS3231_EN32KHZ = 3;
const uint8_t DS3231_OSF     = 7;
const uint8_t DS3231_AIFMASK = (_BV(DS3231_A1F) | _BV(DS3231_A2F));

#endif /* __DS3231__ */
