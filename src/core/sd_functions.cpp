#include <regex>
#include "globals.h"
#include "sd_functions.h"
#include "mykeyboard.h"   // usinf keyboard when calling rename
#include "display.h"      // using displayRedStripe as error msg
#include "modules/others/audio.h"
#include "modules/rf/rf.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/others/bad_usb.h"

struct FilePage {
  int pageIndex;
  int startIdx;
  int endIdx;
};


SPIClass sdcardSPI;
String fileToCopy;
String fileList[MAXFILES][3];
FilePage filePages[100];  // Maximum of 100 pages



/***************************************************************************************
** Function name: setupSdCard
** Description:   Start SD Card
***************************************************************************************/
bool setupSdCard() {
  if(SDCARD_SCK==-1) {
    sdcardMounted = false;
    return false;
  }

  // avoid unnecessary remounting
  if(sdcardMounted) return true;

#if TFT_MOSI == SDCARD_MOSI
  if (!SD.begin(SDCARD_CS))
#else
  sdcardSPI.begin(SDCARD_SCK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS); // start SPI communications
  if (!SD.begin(SDCARD_CS, sdcardSPI))
#endif
  {
    #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
      sdcardSPI.end(); // Closes SPI connections and release pin header.
    #endif
    sdcardMounted = false;
    return false;
  }
  else {
    //Serial.println("SDCARD mounted successfully");
    sdcardMounted = true;
    return true;
  }
}

/***************************************************************************************
** Function name: closeSdCard
** Description:   Turn Off SDCard, set sdcardMounted state to false
***************************************************************************************/
void closeSdCard() {
  SD.end();
  sdcardSPI.end(); // Closes SPI connections and release pins.
  //Serial.println("SD Card Unmounted...");
  sdcardMounted = false;
}

/***************************************************************************************
** Function name: ToggleSDCard
** Description:   Turn Off or On the SDCard, return sdcardMounted state
***************************************************************************************/
bool ToggleSDCard() {
  if (sdcardMounted == true) {
    closeSdCard();
    sdcardMounted = false;
    return false;
  } else {
      sdcardMounted = setupSdCard();
      return sdcardMounted;
  }
}
/***************************************************************************************
** Function name: deleteFromSd
** Description:   delete file or folder
***************************************************************************************/
bool deleteFromSd(FS fs, String path) {
  File dir = fs.open(path);
  if (!dir.isDirectory()) {
    dir.close();
    return fs.remove(path);
  }

  dir.rewindDirectory();
  bool success = true;

  File file = dir.openNextFile();
  while(file) {
    if (file.isDirectory()) {
      success &= deleteFromSd(fs, file.path());
    } else {
      String path2 = file.path();
      file.close();
      success &= fs.remove(path2);
    }
    file = dir.openNextFile();
  }
  file.close();

  dir.close();
  // Apaga a própria pasta depois de apagar seu conteúdo
  success &= fs.rmdir(path.c_str());
  return success;
}

/***************************************************************************************
** Function name: renameFile
** Description:   rename file or folder
***************************************************************************************/
bool renameFile(FS fs, String path, String filename) {
  String newName = keyboard(filename,76,"Type the new Name:");
    // Rename the file of folder
    if (fs.rename(path, path.substring(0,path.lastIndexOf('/')) + "/" + newName)) {
        //Serial.println("Renamed from " + filename + " to " + newName);
        return true;
    } else {
        //Serial.println("Fail on rename.");
        return false;
    }
}
/***************************************************************************************
** Function name: copyToFs
** Description:   copy file from SD or LittleFS to LittleFS or SD
***************************************************************************************/
bool copyToFs(FS from, FS to, String path) {
  // Tamanho do buffer para leitura/escrita
  const size_t bufferSize = 2048*2; // Ajuste conforme necessário para otimizar a performance
  uint8_t buffer[bufferSize];
  bool result;

  if (!SD.begin()) { result = false; Serial.println("Error 1"); }
  if(!LittleFS.begin()) { result = false; Serial.println("Error 2"); }

  File source = from.open(path, FILE_READ);
  if (!source) {
    Serial.println("Error 3");
    result = false;
  }
  path = path.substring(path.lastIndexOf('/'));
  if(!path.startsWith("/")) path = "/" + path;
  File dest = to.open(path, FILE_WRITE);
  if (!dest) {
    Serial.println("Error 4");
    result = false;
  }
  size_t bytesRead;
  int tot=source.size();
  int prog=0;

  if(&to==&LittleFS && (LittleFS.totalBytes() - LittleFS.usedBytes()) < tot) {
    Serial.println("Not enaugh space on LittleFS for this file");
    displayError("Not enaugh space");
    delay(3000);
    return false;
  }
  //tft.drawRect(5,HEIGHT-12, (WIDTH-10), 9, FGCOLOR);
  while ((bytesRead = source.read(buffer, bufferSize)) > 0) {
    if (dest.write(buffer, bytesRead) != bytesRead) {
      //Serial.println("Falha ao escrever no arquivo de destino");
      source.close();
      dest.close();
      result = false;
      Serial.println("Error 5");
    } else {
      prog+=bytesRead;
      float rad = 360*prog/tot;
      tft.drawArc(WIDTH/2,HEIGHT/2,HEIGHT/4,HEIGHT/5,0,int(rad),ALCOLOR,BGCOLOR,true);
    }
  }
  if(prog==tot) result = true;
  else { result = false; displayError("Error 6"); }

  if(!result) delay(5000);
  return result;
}

/***************************************************************************************
** Function name: copyFile
** Description:   copy file address to memory
***************************************************************************************/
bool copyFile(FS fs, String path) {
  File file = fs.open(path, FILE_READ);
  if(!file.isDirectory()) {
    fileToCopy = path;
    file.close();
    return true;
  }
  else {
    displayRedStripe("Cannot copy Folder");
    file.close();
    return false;
  }

}

/***************************************************************************************
** Function name: pasteFile
** Description:   paste file to new folder
***************************************************************************************/
bool pasteFile(FS fs, String path) {
  // Tamanho do buffer para leitura/escrita
  const size_t bufferSize = 2048*2; // Ajuste conforme necessário para otimizar a performance
  uint8_t buffer[bufferSize];

  // Abrir o arquivo original
  File sourceFile = fs.open(fileToCopy, FILE_READ);
  if (!sourceFile) {
    //Serial.println("Falha ao abrir o arquivo original para leitura");
    return false;
  }

  // Criar o arquivo de destino
  File destFile = fs.open(path + "/" + fileToCopy.substring(fileToCopy.lastIndexOf('/') + 1), FILE_WRITE);
  if (!destFile) {
    //Serial.println("Falha ao criar o arquivo de destino");
    sourceFile.close();
    return false;
  }

  // Ler dados do arquivo original e escrever no arquivo de destino
  size_t bytesRead;
  int tot=sourceFile.size();
  int prog=0;
  //tft.drawRect(5,HEIGHT-12, (WIDTH-10), 9, FGCOLOR);
  while ((bytesRead = sourceFile.read(buffer, bufferSize)) > 0) {
    if (destFile.write(buffer, bytesRead) != bytesRead) {
      //Serial.println("Falha ao escrever no arquivo de destino");
      sourceFile.close();
      destFile.close();
      return false;
    } else {
      prog+=bytesRead;
      float rad = 360*prog/tot;
      tft.drawArc(WIDTH/2,HEIGHT/2,HEIGHT/4,HEIGHT/5,0,int(rad),ALCOLOR,BGCOLOR,true);
      //tft.fillRect(7,HEIGHT-10, (WIDTH-14)*prog/tot, 5, FGCOLOR);
    }
  }

  // Fechar ambos os arquivos
  sourceFile.close();
  destFile.close();
  return true;
}


/***************************************************************************************
** Function name: createFolder
** Description:   create new folder
***************************************************************************************/
bool createFolder(FS fs, String path) {
  String foldername=keyboard("",76,"Folder Name: ");
  if(!fs.mkdir(path + "/" + foldername)) {
    displayRedStripe("Couldn't create folder");
    return false;
  }
  return true;
}

/**********************************************************************
**  Function: readLineFromFile
**  Read the line of the config file until the ';'
**********************************************************************/
String readLineFromFile(File myFile) {
  String line = "";
  char character;

  while (myFile.available()) {
    character = myFile.read();
    if (character == ';') {
      break;
    }
    line += character;
  }
  return line;
}

/***************************************************************************************
** Function name: sortList
** Description:   sort files for name
***************************************************************************************/
void sortList(String fileList[][3], int fileListCount) {
    bool swapped;
    String temp[3];
    String name1, name2;

    do {
        swapped = false;
        for (int i = 0; i < fileListCount - 1; i++) {
            name1 = fileList[i][0];
            name1.toLowerCase();  // Use lowercase so special chars like '_' can come first
            name2 = fileList[i + 1][0];
            name2.toLowerCase();

            // Verificar se ambos são pastas ou arquivos
            bool isFolder1 = fileList[i][2] == "folder";
            bool isFolder2 = fileList[i + 1][2] == "folder";

            // Primeiro, ordenar pastas
            if (isFolder1 && !isFolder2) {
                continue; // Se o primeiro for uma pasta e o segundo não, não troque
            } else if (!isFolder1 && isFolder2) {
                // Se o primeiro for um arquivo e o segundo uma pasta, troque
                for (int j = 0; j < 3; j++) {
                    temp[j] = fileList[i][j];
                    fileList[i][j] = fileList[i + 1][j];
                    fileList[i + 1][j] = temp[j];
                }
                swapped = true;
            } else {
                // Ambos são pastas ou arquivos, então ordenar alfabeticamente
                if (name1.compareTo(name2) > 0) {
                    for (int j = 0; j < 2; j++) {
                        temp[j] = fileList[i][j];
                        fileList[i][j] = fileList[i + 1][j];
                        fileList[i + 1][j] = temp[j];
                    }
                    swapped = true;
                }
            }
        }
    } while (swapped);
}
/***************************************************************************************
** Function name: clearFileList
** Description:   clear File List to clear memory to other functions
***************************************************************************************/
void clearFileList(String list[][3]) {
  int i = 0;
    while(i<MAXFILES) {
      list[i][0]="";
      list[i][1]="";
      list[i][2]="";
      i++;
    }
}
bool checkExt(String ext, String pattern) {
    if (ext == pattern) return true;

    char charArray[pattern.length() + 1];
    pattern.toCharArray(charArray, pattern.length() + 1);
    std::regex ext_regex(charArray);
    return std::regex_search(ext.c_str(), ext_regex);
}

/***************************************************************************************
** Function name: sortList
** Description:   sort files for name
***************************************************************************************/
void readFs(FS fs, String folder, String result[][3], String allowed_ext) {

    int allFilesCount = 0;
    clearFileList(result);

    File root = fs.open(folder);
    if (!root || !root.isDirectory()) {
        //Serial.println("Não foi possível abrir o diretório");
        return; // Retornar imediatamente se não for possível abrir o diretório
    }

    File file2 = root.openNextFile();
    while (file2 && allFilesCount < (MAXFILES-1)) {
        String fileName = file2.name();
        if (!file2.isDirectory()) {
            String ext = fileName.substring(fileName.lastIndexOf(".") + 1);
            ext.toUpperCase();
            if (ext.equals("BIN")) {
              result[allFilesCount][0] = fileName.substring(fileName.lastIndexOf("/") + 1);
              result[allFilesCount][1] = file2.path();
              result[allFilesCount][2] = "file";
              allFilesCount++;
            }
            else if(allowed_ext=="*" || checkExt(ext, allowed_ext)) {
              result[allFilesCount][0] = fileName.substring(fileName.lastIndexOf("/") + 1);
              result[allFilesCount][1] = file2.path();
              result[allFilesCount][2] = "file";
              allFilesCount++;
            }
        }

        file2 = root.openNextFile();
    }
    file2.close();
    root.close();

    root = fs.open(folder);
    File file = root.openNextFile();
    while (file && allFilesCount < (MAXFILES-1)) {
        String fileName = file.name();
        if (file.isDirectory()) {
            result[allFilesCount][0] = fileName.substring(fileName.lastIndexOf("/") + 1);
            result[allFilesCount][1] = file.path();
            result[allFilesCount][2] = "folder";
            allFilesCount++;
        }
        file = root.openNextFile();
    }
    file.close();
    root.close();

    // Ordenar os arquivos e pastas
    sortList(result, allFilesCount);
    //allFilesCount++;
    result[allFilesCount][0] = "> Back";
    folder = folder.substring(0,folder.lastIndexOf('/'));
    if(folder=="") folder = "/";
    result[allFilesCount][1] = folder;
    result[allFilesCount][2] = "operator";
}

/*********************************************************************
**  Function: loopSD
**  Where you choose what to do with your SD Files
**********************************************************************/
String loopSD(FS &fs, bool filePicker, String allowed_ext) {
  String result = "";
  bool reload=false;
  bool redraw = true;
  int index = 0;
  int maxFiles = 0;
  String Folder = "/";
  String PreFolder = "/";
  tft.fillScreen(BGCOLOR);
  tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,FGCOLOR);
  closeSdCard();
  setupSdCard();
  bool exit = false;

  readFs(fs, Folder, fileList, allowed_ext);

  for(int i=0; i<MAXFILES; i++) if(fileList[i][2]!="") maxFiles++; else break;
  while(1){
    //if(returnToMenu) break; // stop this loop and retur to the previous loop
    if(exit){ 
      returnToMenu=true;
      break; // stop this loop and retur to the previous loop
    } 

    if(redraw) {
      if(strcmp(PreFolder.c_str(),Folder.c_str()) != 0 || reload){
        tft.fillScreen(BGCOLOR);
        tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,FGCOLOR);
        index=0;
        readFs(fs, Folder, fileList, allowed_ext);
        PreFolder = Folder;
        maxFiles=0;
        for(int i=0; i<MAXFILES; i++) if(fileList[i][2]!="") maxFiles++; else break;
        reload=false;
      }
      listFiles(index, fileList);
      #if defined(HAS_TOUCH)
        TouchFooter();
      #endif
      delay(150);
      redraw = false;
    }

    if(checkPrevPress()) {
      if(index==0) index = maxFiles - 1;
      else if(index>0) index--;
      redraw = true;
    }
    /* DW Btn to next item */
    if(checkNextPress()) {
      index++;
      if(index==maxFiles) index = 0;
      redraw = true;
    }

    /* Select to install */
    if(checkSelPress()) {
      delay(200);

      if(checkSelPress())
      {
        // Definição da matriz "Options"
        if(fileList[index][2]=="folder") {
          options = {
            {"New Folder", [=]() { createFolder(fs, Folder); }},
            {"Rename",     [=]() { renameFile(fs, fileList[index][1], fileList[index][0]); }},
            {"Delete",     [=]() { deleteFromSd(fs, fileList[index][1]); }},
            {"Main Menu",  [&]() { exit = true; }},
          };
          delay(200);
          loopOptions(options);
          tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,FGCOLOR);
          reload = true;
          redraw = true;
        } else if(fileList[index][2]=="file"){
          goto Files;
        } else {
          options = {
            {"New Folder", [=]() { createFolder(fs, Folder); }},
          };
          if(fileToCopy!="") options.push_back({"Paste", [=]() { pasteFile(fs, Folder); }});
          options.push_back({"Main Menu", [&]() { exit = true; }});
          delay(200);
          loopOptions(options);
          tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,FGCOLOR);
          reload = true;
          redraw = true;
        }
      } else {
        Files:
        if(fileList[index][2]=="folder") {
          Folder = fileList[index][1];
          redraw=true;
        } else if (fileList[index][2]=="file") {
          String filepath=fileList[index][1];
          String filename=fileList[index][0];
          clearFileList(fileList);
          options = {
            {"View File",  [=]() { viewFile(fs, filepath); }},
            {"Rename",     [=]() { renameFile(fs, filepath, filename); }},
            {"Copy",       [=]() { copyFile(fs, filepath); }},
            {"Delete",     [=]() { deleteFromSd(fs, filepath); }},
            {"New Folder", [=]() { createFolder(fs, Folder); }},
          };
          if(fileToCopy!="") options.push_back({"Paste",  [=]() { pasteFile(fs, Folder); }});
          if(&fs == &SD) options.push_back({"Copy->LittleFS", [=]() { copyToFs(SD,LittleFS, filepath); }});
          if(&fs == &LittleFS && sdcardMounted) options.push_back({"Copy->SD", [=]() { copyToFs(LittleFS, SD, filepath); }});

          // custom file formats commands added in front
          if(filepath.endsWith(".ir")) options.insert(options.begin(), {"IR Tx SpamAll",  [&]() { 
              delay(200);
              txIrFile(&fs, filepath);
            }});
          if(filepath.endsWith(".sub")) options.insert(options.begin(), {"Subghz Tx",  [&]() { 
              delay(200);
              txSubFile(&fs, filepath);
            }});
          #if defined(USB_as_HID)
          if(filepath.endsWith(".txt")) options.insert(options.begin(), {"BadUSB Run",  [&]() { 
              Kb.begin();
              USB.begin();
              key_input(fs, filepath);
            }});
          #endif
          #if defined(HAS_NS4168_SPKR)
          if(isAudioFile(filepath)) options.insert(options.begin(), {"Play Audio",  [&]() { 
            delay(200);
            playAudioFile(&fs, filepath);
            setup_gpio(); //TODO: remove after fix select loop
          }});
          #endif

          options.push_back({"Main Menu", [&]() { exit = true; }});
          delay(200);
          if(!filePicker) loopOptions(options);
          else {
            result = filepath;
            break;
          }
          tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,FGCOLOR);
          reload = true;
          redraw = true;
        } else {
          if(Folder == "/") break;
          Folder = fileList[index][1];
          index = 0;
          redraw=true;
        }
        redraw = true;
      }
    }

    #ifdef CARDPUTER
      /*
      if(Keyboard.isKeyPressed(KEY_BACKSPACE)) {
        // go back 1 level
          if(Folder == "/") break;
          Folder = fileList[index][1];
          index = 0;
          redraw=true;
          continue;
      }*/
      if(checkEscPress()) break;
      char pressed_letter = checkLetterShortcutPress();
      if(pressed_letter>0) {
        //Serial.println(pressed_letter);
        if(tolower(fileList[index][0].c_str()[0]) == pressed_letter) {
          // already selected, go to the next
          index += 1;
          // check if index is still valid
          if(index<=maxFiles && tolower(fileList[index][0].c_str()[0]) == pressed_letter)
          {
            redraw = true;
            continue;
          }
        }
        // else look again from the start
        for(int i=0; i<maxFiles; i++) {
          if(tolower(fileList[i][0].c_str()[0]) == pressed_letter) {  // check if 1st char matches
            index = i; 
            redraw = true;
            break;  // quit on 1st match
          }
        }
      }
    #endif
  }
  clearFileList(fileList);
  return result;
  //closeSdCard();
  //setupSdCard();
}

/*********************************************************************
**  Function: createFilePages
**  Create a list of file pages to be displayed
**********************************************************************/
int createFilePages(String fileContent) {
  const int8_t MAX_LINES = 16;
  const int8_t MAX_LINE_CHARS = 41;

  int currentPage = 0;
  int lineStartIdx = 0;
  int pageStartIdx = 0;
  int pageEndIdx = 0;
  int totalPageLines = 0;

  while (pageEndIdx < fileContent.length()) {
    // Check end of line
    if (fileContent[pageEndIdx] == '\n' || (pageEndIdx-lineStartIdx) == MAX_LINE_CHARS) {
      totalPageLines++;
      lineStartIdx = pageEndIdx + 1;
    }

    // Check end of page
    if (totalPageLines == MAX_LINES) {
      filePages[currentPage].pageIndex = currentPage;
      filePages[currentPage].startIdx = pageStartIdx;
      filePages[currentPage].endIdx = pageEndIdx;

      currentPage++;
      pageStartIdx = pageEndIdx + 1;
      totalPageLines = 0;
    }

    pageEndIdx++;
  }

  if (totalPageLines > 0) {
    filePages[currentPage].pageIndex = currentPage;
    filePages[currentPage].startIdx = pageStartIdx;
    filePages[currentPage].endIdx = pageEndIdx;
  }

  return currentPage;
}

/*********************************************************************
**  Function: viewFile
**  Display file content
**********************************************************************/
void viewFile(FS fs, String filepath) {
  tft.fillScreen(BGCOLOR);
  String fileContent = "";
  File file;
  String displayText;
  int totalPages;
  int currentPage = 0;
  bool updateContent = true;

  file = fs.open(filepath, FILE_READ);
  if (!file) return;

  // TODO: detect binary file, switch to hex view

  while (file.available()) {
    fileContent = file.readString();
  }
  file.close();
  delay(100);

  totalPages = createFilePages(fileContent);

  while(1) {
    if(updateContent) {
      tft.fillScreen(BGCOLOR);
      tft.setCursor(0,0);
      tft.setTextSize(FP);

      displayText = fileContent.substring(
        filePages[currentPage].startIdx,
        filePages[currentPage].endIdx
      );
      tft.print(displayText);

      delay(150);
      updateContent = false;
    }

    if(checkEscPress()) break;

    if(checkPrevPress()) {
      if (currentPage > 0) {
        currentPage--;
        updateContent = true;
      }
    }

    if(checkNextPress()) {
      if (currentPage < totalPages) {
        currentPage++;
        updateContent = true;
      }
    }

    delay(100);
  }

  return;
}
/*********************************************************************
**  Function: checkLittleFsSize
**  Check if there are more then 4096 bytes available for storage
**********************************************************************/
bool checkLittleFsSize() {
  if((LittleFS.totalBytes() - LittleFS.usedBytes()) < 4096) {
    displayError("LittleFS is Full");
    delay(2000);
    return false;
  } else return true;
}
/*********************************************************************
**  Function: checkLittleFsSize
**  Check if there are more then 4096 bytes available for storage
**********************************************************************/
bool checkLittleFsSizeNM() {
  if((LittleFS.totalBytes() - LittleFS.usedBytes()) < 4096) {
    return false;
  } else return true;
}