
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <FS.h>
#include <SD.h>
#include <globals.h>



// custom Ir
void sendRawCommand(uint16_t frequency, String rawData);
void sendNECCommand(String address, String command);
void sendNECextCommand(String address, String command);
void sendRC5Command(String address, String command);
void sendRC6Command(String address, String command);
void sendSamsungCommand(String address, String command);
void sendSonyCommand(String address, String command);
void sendPanasonicCommand(String address, String data);
bool sendDecodedCommand(String protocol, String value, uint8_t bits=32);
void otherIRcodes();
bool txIrFile(FS *fs, String filepath);