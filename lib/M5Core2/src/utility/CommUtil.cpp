#if defined (CORE2)
/*----------------------------------------------------------------------*
 * M5Stack I2C Common Library v1.0                                      *
 *                                                                      *
 * This work is licensed under the GNU Lesser General Public            *
 * License v2.1                                                         *
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html           *
 *----------------------------------------------------------------------*/
#include "CommUtil.h"
#include "../M5Core2.h"

extern M5Core2 M5;

// debug for message of I2C ( bypass message to serial)
// #define  I2C_DEBUG_TO_SERIAL

CommUtil::CommUtil() {
}

// Wire.h read and write protocols
bool CommUtil::writeCommand(uint8_t address, uint8_t subAddress) {
    bool function_result = false;
    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    function_result = (Wire.endTransmission() == 0);  // Send the Tx buffer

#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("writeCommand:send to 0x%02x [0x%02x] result:%s\n", address,
                  subAddress, function_result ? "OK" : "NG");
#endif

    return (function_result);
}

// Wire.h read and write protocols
bool CommUtil::writeByte(uint8_t address, uint8_t subAddress, uint8_t data) {
    bool function_result = false;
    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    Wire.write(data);                 // Put data in Tx buffer
    function_result = (Wire.endTransmission() == 0);  // Send the Tx buffer

#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("writeByte:send to 0x%02x [0x%2x] data=0x%02x result:%s\n",
                  address, subAddress, data, function_result ? "OK" : "NG");
#endif

    return (function_result);
}

// Wire.h read and write protocols
bool CommUtil::writeBytes(uint8_t address, uint8_t subAddress, uint8_t *data,
                          uint8_t length) {
    bool function_result = false;

#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("writeBytes:send to 0x%02x [0x%02x] data=", address,
                  subAddress);
#endif

    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    for (int i = 0; i < length; i++) {
        Wire.write(*(data + i));  // Put data in Tx buffer
#ifdef I2C_DEBUG_TO_SERIAL
        Serial.printf("%02x ", *(data + i));
#endif
    }
    function_result = (Wire.endTransmission() == 0);  // Send the Tx buffer

#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("result:%s\n", function_result ? "OK" : "NG");
#endif

    return function_result;  // Send the Tx buffer
}

bool CommUtil::readByte(uint8_t address, uint8_t *result) {
#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("readByte :read from 0x%02x requestByte=1 receive=", address);
#endif

    if (Wire.requestFrom(address, (uint8_t)1)) {
        *result = Wire.read();  // Fill Rx buffer with result
#ifdef I2C_DEBUG_TO_SERIAL
        Serial.printf("%02x\n", result);
#endif
        return true;
    }
#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("none\n");
#endif
    return false;
}

bool CommUtil::readByte(uint8_t address, uint8_t subAddress, uint8_t *result) {
#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("readByte :read from 0x%02x [0x%02x] requestByte=1 receive=",
                  address, subAddress);
#endif

    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    if (Wire.endTransmission(false) == 0 &&
        Wire.requestFrom(address, (uint8_t)1)) {
        *result = Wire.read();  // Fill Rx buffer with result
#ifdef I2C_DEBUG_TO_SERIAL
        Serial.printf("%02x\n", *result);
#endif
        return true;
    }
#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("none\n");
#endif
    return false;
}

bool CommUtil::readBytes(uint8_t address, uint8_t subAddress, uint8_t count,
                         uint8_t *dest) {
#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("readBytes:read from 0x%02x [0x%02x] requestByte=%d receive=",
                  address, subAddress, count);
#endif

    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    uint8_t i = 0;
    if (Wire.endTransmission(false) == 0 &&
        Wire.requestFrom(address, (uint8_t)count)) {
        while (Wire.available()) {
            dest[i++] = Wire.read();  // Put read results in the Rx buffer
#ifdef I2C_DEBUG_TO_SERIAL
            Serial.printf("%02x ", dest[i - 1]);
#endif
        }
#ifdef I2C_DEBUG_TO_SERIAL
        Serial.printf(" (len:%d)\n", i);
#endif
        return true;
    }
#ifdef I2C_DEBUG_TO_SERIAL
    Serial.printf("none\n");
#endif
    return false;
}

bool CommUtil::readBytes(uint8_t address, uint8_t count, uint8_t *dest) {
    uint8_t i = 0;
    if (Wire.requestFrom(address, (uint8_t)count)) {
        while (Wire.available()) {
            // Put read results in the Rx buffer
            dest[i++] = Wire.read();
        }
        return true;
    }
    return false;
}

void CommUtil::scanID(bool *result) {
    for (int i = 0x00; i <= 0x7f; i++) {
        *(result + i) = writeCommand(i, 0x00);
    }
}
#endif