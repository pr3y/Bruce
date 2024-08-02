/* This code is ass, feel free to delete everthing and redo it from scratch <3 */
/* Author: @im.nix - even tho im not proud of this code :P */

#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "IRReceiver.h"

/* Dont touch this */
#define MAX_RAWBUF_SIZE 100
#define IR_FREQUENCY 38000
#define DUTY_CYCLE 0.330000

decode_results results;
uint16_t* rawcode;

int rawDataLen;
String signalName;
String signalCode;

void initializeIRReceiver(IRrecv &irrecv) {
  irrecv.enableIRIn();
  pinMode(IrRx, INPUT);
}

void displayWaitingMessage() {
  tft.fillScreen(BGCOLOR);
  tft.setTextSize(4);
  tft.setCursor(0, 0);
  tft.println("Receiver");
  tft.setTextSize(3);
  tft.println("Waiting...");
}

void displayRecordedMessage() {
  tft.fillScreen(BGCOLOR);
  tft.setTextSize(4);
  tft.setCursor(0, 0);
  tft.println("Receiver");
  tft.setTextSize(3);
  tft.println("Recorded!");
  tft.print(results.value, HEX);
}

void processRawData() {
  rawcode = new uint16_t[MAX_RAWBUF_SIZE];
  memset(rawcode, 0, MAX_RAWBUF_SIZE * sizeof(uint16_t));
  rawDataLen = results.rawlen;
  
  Serial.print("Raw Data ");
  Serial.print(rawDataLen);
  Serial.print(" : ");
  /* I HAVE NO FUCKING IDEA WHY WE NEED TO MULTIPLY BY 2, BUT DO. */
  for (int i = 1; i < rawDataLen; i++) {
    signalCode += String(results.rawbuf[i] * 2);
    Serial.print(results.rawbuf[i], DEC);
    if (i < rawDataLen - 1) {
      Serial.print(", "); 
      signalCode += " ";
    } 
    rawcode[i - 1] = results.rawbuf[i] * 2;
  }
  Serial.println();
  Serial.print("Hex: ");
  Serial.println(results.value, HEX);
}

void recvReceive() {
  IRrecv irrecv(IrRx);
  initializeIRReceiver(irrecv);
  
  displayWaitingMessage();
  
  while (true) {
    if (irrecv.decode(&results)) {
      displayRecordedMessage();
      processRawData();
      recv_save();
      irrecv.resume();
      delay(1000);
      break;
    }
    if (checkEscPress()) break;
  }
}

void selectFileSystem(FS* &fs) {
  options = {};
  if (checkLittleFsSize()) options.push_back({"LittleFS",  [&]() { fs=&LittleFS; }});
  if (setupSdCard()) options.push_back({"SD Card",  [&]() { fs=&SD; }});
  if (options.size() == 0) {
    displayRedStripe("No space, womp womp", TFT_WHITE, BGCOLOR);
    delay(1000);
    backToMenu();
  }
  delay(200);
  loopOptions(options);
  delay(200);
}

String generateUniqueFilename(FS* fs, String baseName) {
  String filename = baseName;
  if ((*fs).exists("/BruceInfrared/" + filename + ".ir")) {
    int i = 1;
    filename += "_";
    while ((*fs).exists("/BruceInfrared/" + filename + String(i) + ".ir")) i++;
    filename += String(i);
  }
  return filename;
}

void saveSignalToFile(FS* fs, String filename) {
  if (!(*fs).exists("/BruceInfrared")) (*fs).mkdir("/BruceInfrared");
  filename = generateUniqueFilename(fs, filename);
  
  File file = (*fs).open("/BruceInfrared/" + filename + ".ir", FILE_WRITE);
  if (file) {
    file.println("Filetype: IR signals file");
    file.println("Version 1");
    file.println("#");
    file.println("# " + filename);
    file.println("#");
    file.println("name: Captured");
    file.println("type: raw");
    file.println("frequency: " + String(IR_FREQUENCY));
    file.println("duty_cycle: " + String(DUTY_CYCLE));
    file.println("data: " + signalCode);
    file.println("#");
    file.close();
  }
  signalCode = "";
}

void recv_save() {
  FS *fs = NULL;
  String filename = keyboard("", 30, "File name:");
  
  selectFileSystem(fs);
  
  if (fs) {
    saveSignalToFile(fs, filename);
    delay(100);
  }
}