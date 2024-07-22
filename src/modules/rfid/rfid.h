#include "mfrc522_i2c.h" // RFID2 M5Stack - IncursioHack
#include <Wire.h> // RFID2 and RF433 M5Stack - IncursioHack

void displayReadMode();
void displayWriteMode();
void cls();
void rfid_setup();
void rfid_loop();
void readCard();
void displayUID();
void writeCard();