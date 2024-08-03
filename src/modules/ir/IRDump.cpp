/**
 * @file IRDump.cpp
 * @author @im.nix (https://github.com/Niximkk)
 * @version 0.2
 * @date 2024-08-03
 * 
 * This code is ass, feel free to delete everthing and redo it from scratch <3
 * Code from Fuck-Nemo...
 * No wait, fuck that firmware, the code is originally from https://github.com/FlasherTwelve/IR_Replay ^^
 */

#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "IRDump.h"

/* Dont touch this */
#define MAX_RAWBUF_SIZE 300
#define IR_FREQUENCY 38000
#define DUTY_CYCLE 0.330000

decode_results results;
uint16_t* rawcode;
int rawDataLen;
int signalCount;
String signalName;
String signalCode;

void initializeIRReceiver(IRrecv &irrecv) {
  irrecv.enableIRIn();
  pinMode(IrRx, INPUT);
}

void displayWaitingMessage() {
  drawMainBorder();
  tft.setCursor(10, 28);
  tft.setTextSize(FP);
  tft.println("Waiting for signal.");
}

void displayRecordedMessage() {
  drawMainBorder();
  tft.setCursor(10, 28);
  tft.setTextSize(FP);
  tft.println("Signal found!");
  tft.setCursor(10, tft.getCursorY());
  tft.print("HEX: 0x");
  tft.println(results.value, HEX);
  tft.setCursor(10, tft.getCursorY());
  tft.println("");
  tft.setCursor(10, tft.getCursorY());
  tft.println("M5 To save");
  tft.setCursor(10, tft.getCursorY());
  tft.println("Nxt To discard");
  tft.setCursor(10, tft.getCursorY());
  tft.println("Esc To exit");
}

// TODO: Remove bloat
void processRawData() {
  rawcode = new uint16_t[MAX_RAWBUF_SIZE];
  memset(rawcode, 0, MAX_RAWBUF_SIZE * sizeof(uint16_t));
  rawDataLen = results.rawlen;
  signalCode = "";
  Serial.print("Raw Data ");
  Serial.print(rawDataLen);
  Serial.print(" : ");
  
  /* I HAVE NO FUCKING IDEA WHY WE NEED TO MULTIPLY BY 2, BUT WE DO. */
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
  delete[] rawcode;
  rawcode = nullptr;
}

// TODO: Make this less shitty, implement a better way of selecting options.
void recvReceive() {
  IRrecv irrecv(IrRx);
  while (true) {
    initializeIRReceiver(irrecv);
    displayWaitingMessage();
    while (true) {
      if (irrecv.decode(&results)) {
        displayRecordedMessage();
        processRawData();
        while(true){
          if (checkSelPress()) { saveController(); break; }
          if (checkNextPress()) break;
          if (checkEscPress()) return;
        }
        irrecv.resume();
        break;
      }
    }
    if (checkEscPress()) return;
  }
}

void selectFileSystem(FS* &fs) {
  options = {};
  if (checkLittleFsSize()) options.push_back({"LittleFS",  [&]() { fs=&LittleFS; }});
  if (setupSdCard()) options.push_back({"SD Card",  [&]() { fs=&SD; }});
  if (options.size() == 0) {
    displayRedStripe("No space, womp womp", TFT_WHITE, FGCOLOR);
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

// TODO: Add a way to save multiple signals before closing the file.
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
    file.println("name: CoolSignal");
    file.println("type: raw");
    file.println("frequency: " + String(IR_FREQUENCY));
    file.println("duty_cycle: " + String(DUTY_CYCLE));
    file.println("data: " + signalCode);
    file.println("#");
    file.close();
    displayRedStripe("Saved!", TFT_WHITE, FGCOLOR);
    delay(1000);
  }
}

// TODO: Only call this to close the file and save the controller.
void saveController() {
  FS *fs = NULL;
  String filename = keyboard("MyController", 30, "File name:");
  drawMainBorder();
  selectFileSystem(fs);
  
  if (fs) {
    saveSignalToFile(fs, filename);
    delay(100);
  }
  recvReceive();
}

// TODO: Make a saveSignal(); function to only store the signal without closing the file.