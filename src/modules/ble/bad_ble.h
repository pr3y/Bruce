#include <SPI.h>
#include <SD.h>
#include <time.h>

// For some reason, StickC and P and P2 dont recognize the following library... may be it need to use EspTinyUSB lib.... need studies
#include <BleKeyboard.h>

void key_input_ble(FS fs, String bad_script = "/badpayload.txt");

void ble_setup();

void ble_keyboard();