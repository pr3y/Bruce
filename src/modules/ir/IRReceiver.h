#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <FS.h>
#include <SD.h>
#include "core/globals.h"

void initializeIRReceiver(IRrecv &irrecv);
void displayWaitingMessage();
void displayRecordedMessage();
void processRawData();
void recvReceive();
void selectFileSystem(FS* &fs);
String generateUniqueFilename(FS* fs, String baseName);
void saveSignalToFile(FS* fs, String filename);
void recv_save();