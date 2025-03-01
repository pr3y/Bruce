#ifndef __PCF8563__
#define __PCF8563__

#include <Arduino.h>

// I2C Slave Address
const uint8_t PCF8563_ADDRESS = 0x51;

// PCF8563 Register Addresses
const uint8_t PCF8563_REG_CONTROL        = 0x00;
const uint8_t PCF8563_REG_STATUS         = 0x01;
const uint8_t PCF8563_REG_TIMEDATE       = 0x02;
const uint8_t PCF8563_REG_INTEGRITY      = 0x02;
const uint8_t PCF8563_REG_ALARM          = 0x09;
const uint8_t PCF8563_REG_CLKOUT_CONTROL = 0x0D;
const uint8_t PCF8563_REG_TIMER_CONTROL  = 0x0E;
const uint8_t PCF8563_REG_TIMER          = 0x0F;

// PCF8563 Register Data Size if not just 1
const size_t PCF8563_REG_TIMEDATE_SIZE = 7;
const size_t PCF8563_REG_ALARM_SIZE    = 4;

// PCF8563 Valid Register Bits (PCF8563_REG_INTEGRITY)
const uint8_t PCF8563_INTEGRITY_VALID = 7;

// PCF8563 Control Register Bits (PCF8563_REG_CONTROL)
const uint8_t PCF8563_CONTROL_CLOCK = 5;

// PCF8563 Status Register Bits (PCF8563_REG_STATUS)
const uint8_t PCF8563_STATUS_TIE  = 0; // timer interrupt enable
const uint8_t PCF8563_STATUS_AIE  = 1; // alarm interrupt enable
const uint8_t PCF8563_STATUS_TF   = 2; // timer flag
const uint8_t PCF8563_STATUS_AF   = 3; // alarm flag
const uint8_t PCF8563_STATUS_TITP = 4; // timer interrupt period config

#endif /* __PCF8563__ */
