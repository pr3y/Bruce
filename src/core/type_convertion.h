#ifndef __TYPE_CONVERTION_H__
#define __TYPE_CONVERTION_H__

#include <Arduino.h>

String hexStrToBinStr(const String &hexStr);
void decimalToHexString(uint64_t decimal, char *output);
uint8_t hexCharToDecimal(char c);
uint32_t hexStringToDecimal(const char *hexString);
char *dec2binWzerofill(uint64_t Dec, unsigned int bitLength);

#endif
