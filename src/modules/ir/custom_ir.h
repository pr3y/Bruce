
#include <Arduino.h>
#include <FS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <SD.h>
#include <globals.h>

struct IRCode {
    IRCode(
        String protocol = "", String address = "", String command = "", String data = "", uint8_t bits = 32
    )
        : protocol(protocol), address(address), command(command), data(data), bits(bits) {}

    IRCode(IRCode *code) {
        name = String(code->name);
        type = String(code->type);
        protocol = String(code->protocol);
        address = String(code->address);
        command = String(code->command);
        frequency = code->frequency;
        bits = code->bits;
        // duty_cycle = code->duty_cycle;
        data = String(code->data);
        filepath = String(code->filepath);
    }

    String protocol = "";
    String address = "";
    String command = "";
    String data = "";
    uint8_t bits = 32;
    String name = "";
    String type = "";
    uint16_t frequency = 0;
    // float duty_cycle;
    String filepath = "";
};

// Custom IR
void sendIRCommand(IRCode *code);
void sendRawCommand(uint16_t frequency, String rawData);
void sendNECCommand(String address, String command);
void sendNECextCommand(String address, String command);
void sendRC5Command(String address, String command);
void sendRC6Command(String address, String command);
void sendSamsungCommand(String address, String command);
void sendSonyCommand(String address, String command, uint8_t nbits);
void sendKaseikyoCommand(String address, String command);
bool sendDecodedCommand(String protocol, String value, uint8_t bits = 32);
void otherIRcodes();
bool txIrFile(FS *fs, String filepath);
