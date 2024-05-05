#include <FS.h>
#include <SD.h>
#include <SPI.h>


extern SPIClass sdcardSPI;
  
bool setupSdCard();

void closeSdCard();

bool ToggleSDCard();

bool deleteFromSd(String path);

bool renameFile(String path, String filename);

bool copyFile(String path);

bool pasteFile(String path);

bool createFolder(String path);

String readLineFromFile(File myFile);

void readFs(String folder, String result[][3]);

void sortList(String fileList[][3], int fileListCount);

String loopSD(bool filePicker = false);