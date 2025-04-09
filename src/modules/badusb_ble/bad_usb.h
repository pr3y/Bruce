
#include <SD.h>
#include <SPI.h>
#include <USB.h>
#include <time.h>

// For some reason, StickC and P and P2 dont recognize the following library... may be it need to use
// EspTinyUSB lib.... need studies
#ifdef USB_as_HID
#include <USBHIDKeyboard.h>
extern USBHIDKeyboard Kb;
#else
#include <CH9329_Keyboard.h>
// #include <SoftwareSerial.h>
extern CH9329_Keyboard_ Kb;
#endif

void key_input(FS fs, String bad_script = "/badpayload.txt");
void key_input_from_string(String text);

void usb_setup();

void usb_keyboard();
