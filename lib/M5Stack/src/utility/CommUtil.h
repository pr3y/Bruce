#if defined (CORE)
/*----------------------------------------------------------------------*
 * M5Stack I2C Common Library v1.0                                      *
 *                                                                      *
 * This work is licensed under the GNU Lesser General Public            *
 * License v2.1                                                         *
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html           *
 *----------------------------------------------------------------------*/
#ifndef CommUtil_h
#define CommUtil_h

#include <Arduino.h>
#include <Wire.h>

class CommUtil {
   public:
    CommUtil();
    bool writeCommand(uint8_t address, uint8_t subAddress);
    bool writeByte(uint8_t address, uint8_t subAddress, uint8_t data);
    bool writeBytes(uint8_t address, uint8_t subAddress, uint8_t *data,
                    uint8_t length);
    bool readByte(uint8_t address, uint8_t *result);
    bool readByte(uint8_t address, uint8_t subAddress, uint8_t *result);
    bool readBytes(uint8_t address, uint8_t count, uint8_t *dest);
    bool readBytes(uint8_t address, uint8_t subAddress, uint8_t count,
                   uint8_t *dest);
    void scanID(bool *result);

   private:
};
#endif
#endif