#ifndef __DS1307__
#define __DS1307__

#include <Arduino.h>

// I2C Slave Address
const uint8_t DS1307_ADDRESS = 0x68;

// DS1307 Register Addresses
const uint8_t DS1307_REG_TIMEDATE = 0x00;
const uint8_t DS1307_REG_STATUS   = 0x00;
const uint8_t DS1307_REG_CONTROL  = 0x07;
const uint8_t DS1307_REG_RAMSTART = 0x08;
const uint8_t DS1307_REG_RAMEND   = 0x3f;
const uint8_t DS1307_REG_RAMSIZE  = DS1307_REG_RAMEND - DS1307_REG_RAMSTART;

// DS1307 Register Data Size if not just 1
const size_t DS1307_REG_TIMEDATE_SIZE = 7;

// DS1307 Control Register Bits
const uint8_t DS1307_RS0  = 0;
const uint8_t DS1307_RS1  = 1;
const uint8_t DS1307_SQWE = 4;
const uint8_t DS1307_OUT  = 7;

// DS1307 Status Register Bits
const uint8_t DS1307_CH = 7;

#endif /* __DS1307__ */
