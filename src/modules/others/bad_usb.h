#ifdef CARDPUTER

#include <USB.h>
#include <SPI.h>
#include <SD.h>
#include <time.h>

// For some reason, StickC and P and P2 dont recognize the following library... may be it need to use EspTinyUSB lib.... need studies
#include <USBHIDKeyboard.h>

void key_input(FS fs, String bad_script = "/badpayload.txt");

void usb_setup();



#endif