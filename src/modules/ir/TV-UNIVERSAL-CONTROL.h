#ifndef IR_REMOTE_MENU_H
#define IR_REMOTE_MENU_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <FS.h>
#include <SD.h>
#include "core/globals.h"


struct Command {
    String name;
    String protocol;
    String address;
    String command;
};

extern IRsend irsend;
extern Command commands[];
extern int command_count;
extern int current_selection;

void setupIRRemote();
void loopIRRemote();
void drawMenu();
void executeCommand();
void sendNECCommand(String address, String command);
void sendRawCommand(uint16_t frequency, String rawData);
bool sendDecodedCommand(String protocol, String value, String bits="32");

#endif
