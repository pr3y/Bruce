#ifdef USB_as_HID

#include <USB.h>
#include <SPI.h>
#include <SD.h>
#include <time.h>

// For some reason, StickC and P and P2 dont recognize the following library... may be it need to use EspTinyUSB lib.... need studies
#include <USBHIDKeyboard.h>

extern USBHIDKeyboard Kb;

void key_input(FS fs, String bad_script = "/badpayload.txt");

void usb_setup();

void usb_keyboard();


#endif
