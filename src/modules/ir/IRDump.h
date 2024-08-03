#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <FS.h>
#include <SD.h>
#include "core/globals.h"

String generateUniqueFilename(FS* fs, String baseName);

void recvReceive();
void processRawData();
void saveController();
void displayWaitingMessage();
void displayRecordedMessage();
void selectFileSystem(FS* &fs);
void initializeIRReceiver(IRrecv &irrecv);
void saveSignalToFile(FS* fs, String filename);