#ifndef __DS3234__
#define __DS3234__
#include <Arduino.h>

const uint8_t DS3234_REG_WRITE_FLAG = 0x80;

const uint8_t DS3234_REG_TIMEDATE = 0x00;

const uint8_t DS3234_REG_ALARMONE = 0x07;
const uint8_t DS3234_REG_ALARMTWO = 0x0B;

const uint8_t DS3234_REG_CONTROL = 0x0E;
const uint8_t DS3234_REG_STATUS  = 0x0F;
const uint8_t DS3234_REG_AGING   = 0x10;

const uint8_t DS3234_REG_TEMP = 0x11;

const uint8_t DS3234_REG_RAM_ADDRESS = 0x18;
const uint8_t DS3234_REG_RAM_DATA    = 0x19;

const uint8_t DS3234_RAMSTART = 0x00;
const uint8_t DS3234_RAMEND   = 0xFF;
const uint8_t DS3234_RAMSIZE  = DS3234_RAMEND - DS3234_RAMSTART;

// DS3234 Control Register Bits
const uint8_t DS3234_A1IE    = 0;
const uint8_t DS3234_A2IE    = 1;
const uint8_t DS3234_INTCN   = 2;
const uint8_t DS3234_RS1     = 3;
const uint8_t DS3234_RS2     = 4;
const uint8_t DS3234_CONV    = 5;
const uint8_t DS3234_BBSQW   = 6;
const uint8_t DS3234_EOSC    = 7;
const uint8_t DS3234_AIEMASK = (_BV(DS3234_A1IE) | _BV(DS3234_A2IE));
const uint8_t DS3234_RSMASK  = (_BV(DS3234_RS1) | _BV(DS3234_RS2));

// DS3234 Status Register Bits
const uint8_t DS3234_A1F       = 0;
const uint8_t DS3234_A2F       = 1;
const uint8_t DS3234_BSY       = 2;
const uint8_t DS3234_EN32KHZ   = 3;
const uint8_t DS3234_CRATE0    = 4;
const uint8_t DS3234_CRATE1    = 5;
const uint8_t DS3234_BB32KHZ   = 6;
const uint8_t DS3234_OSF       = 7;
const uint8_t DS3234_AIFMASK   = (_BV(DS3234_A1F) | _BV(DS3234_A2F));
const uint8_t DS3234_CRATEMASK = (_BV(DS3234_CRATE0) | _BV(DS3234_CRATE1));

#endif /* __DS3234__ */
