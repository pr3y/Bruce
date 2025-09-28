#include "i2c_finder.h"
#include "display.h"
#include "mykeyboard.h"
#include <Wire.h>

#define FIRST_I2C_ADDRESS 0x01
#define LAST_I2C_ADDRESS 0x7F

void find_i2c_addresses() {
    drawMainBorderWithTitle("I2C Finder");
    padprintln("");
    padprintln("");

    bool first_found = true;
    Wire.begin(bruceConfigPins.i2c_bus.sda, bruceConfigPins.i2c_bus.scl);

    padprintln("Checking I2C addresses ...\n\n");
    delay(300);

    padprint("Found: ");

    for (uint8_t i = FIRST_I2C_ADDRESS; i <= LAST_I2C_ADDRESS; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) {
            if (!first_found) tft.print(", ");
            else first_found = false;
            tft.printf("0x%X", i);
        }
    }

    while (1) {
        if (check(EscPress) || check(SelPress)) {
            returnToMenu = true;
            break;
        }
    }
}

uint8_t find_first_i2c_address() {
    for (uint8_t i = FIRST_I2C_ADDRESS; i <= LAST_I2C_ADDRESS; i++) {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0) return i;
    }
    return 0;
}

bool check_i2c_address(uint8_t i2c_address) {
    Wire.begin(bruceConfigPins.i2c_bus.sda, bruceConfigPins.i2c_bus.scl);
    Wire.beginTransmission(i2c_address);
    int error = Wire.endTransmission();
    return (error == 0);
}
