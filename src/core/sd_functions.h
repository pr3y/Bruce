#include <FS.h>
#include <LittleFS.h>
#include <SD.h>
#include <SPI.h>



//extern SPIClass sdcardSPI;

bool setupSdCard();

void closeSdCard();

bool ToggleSDCard();

bool deleteFromSd(FS fs, String path);

bool renameFile(FS fs, String path, String filename);

bool copyFile(FS fs, String path);

bool copyToFs(FS from, FS to, String path);

bool pasteFile(FS fs, String path);

bool createFolder(FS fs, String path);

String readLineFromFile(File myFile);

String readSmallFile(FS &fs, String filepath);

String readDecryptedAesFile(FS &fs, String filepath);

void readFs(FS fs, String folder, String result[][3], String allowed_ext = "*");

void sortList(String fileList[][3], int fileListCount);

String loopSD(FS &fs, bool filePicker = false, String allowed_ext = "*");

void viewFile(FS fs, String filepath);

int createFilePages(String fileContent);

bool checkLittleFsSize();

bool checkLittleFsSizeNM(); //Don't display msg
