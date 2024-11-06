#include <regex>
#include "globals.h"
#include "sd_functions.h"
#include "mykeyboard.h"   // using keyboard when calling rename
#include "display.h"      // using displayRedStripe as error msg
#include "passwords.h"
#include "modules/others/audio.h"
#include "modules/rf/rf.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/wifi/wigle.h"
#include "modules/others/bad_usb.h"
#include "modules/others/qrcode_menu.h"
#include "modules/bjs_interpreter/interpreter.h"

#include <MD5Builder.h>
#include <esp32/rom/crc.h>  // for CRC32
#include <algorithm> // for std::sort

struct FilePage {
  int pageIndex;
  int startIdx;
  int endIdx;
};


//SPIClass sdcardSPI;
String fileToCopy;
//String fileList[MAXFILES][3];
//String fileList[1][3];
std::vector<FileList> fileList;

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

#if defined(CORES3)
  if (!SD.begin(SDCARD_CS))
#elif TFT_MOSI == SDCARD_MOSI && TFT_MOSI>0
  if (!SD.begin(SDCARD_CS, tft.getSPIinstance()))
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
  #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
  sdcardSPI.end(); // Closes SPI connections and release pins.
  #endif
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
  // Using Global Buffer
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
    displayError("Not enought space", true);
    return false;
  }
  //tft.drawRect(5,HEIGHT-12, (WIDTH-10), 9, bruceConfig.priColor);
  while ((bytesRead = source.read(buff, bufSize)) > 0) {
    if (dest.write(buff, bytesRead) != bytesRead) {
      //Serial.println("Falha ao escrever no arquivo de destino");
      source.close();
      dest.close();
      result = false;
      Serial.println("Error 5");
    } else {
      prog+=bytesRead;
      float rad = 360*prog/tot;
      tft.drawArc(WIDTH/2,HEIGHT/2,HEIGHT/4,HEIGHT/5,0,int(rad),ALCOLOR,bruceConfig.bgColor,true);
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
  //Using Global Buffer

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
  //tft.drawRect(5,HEIGHT-12, (WIDTH-10), 9, bruceConfig.priColor);
  while ((bytesRead = sourceFile.read(buff, bufSize)) > 0) {
    if (destFile.write(buff, bytesRead) != bytesRead) {
      //Serial.println("Falha ao escrever no arquivo de destino");
      sourceFile.close();
      destFile.close();
      return false;
    } else {
      prog+=bytesRead;
      float rad = 360*prog/tot;
      tft.drawArc(WIDTH/2,HEIGHT/2,HEIGHT/4,HEIGHT/5,0,int(rad),ALCOLOR,bruceConfig.bgColor,true);
      //tft.fillRect(7,HEIGHT-10, (WIDTH-14)*prog/tot, 5, bruceConfig.priColor);
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
** Function name: readSmallFile
** Description:   read a small (<3KB) file and return its contents as a single string
**                on any error returns an empty string
***************************************************************************************/
String readSmallFile(FS &fs, String filepath) {
  String fileContent = "";
  File file;

  file = fs.open(filepath, FILE_READ);
  if (!file) return "";

  size_t fileSize = file.size();
  if(fileSize > SAFE_STACK_BUFFER_SIZE || fileSize > ESP.getFreeHeap()) {
      displayError("File is too big", true);
      return "";
  }
  // TODO: if(psramFound()) -> use PSRAM instead

  fileContent = file.readString();

  file.close();
  return fileContent;
}

/***************************************************************************************
** Function name: getFileSize
** Description:   get a file size without opening
***************************************************************************************/
size_t getFileSize(FS &fs, String filepath) {
  /*
  #if !defined(M5STACK)
    if(&fs == &SD) filepath = "/sd" + filepath;
    else if(&fs == &LittleFS) filepath = "/littlefs" + filepath;
    else return 0;  // not found
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (stat(filepath.c_str(), &st) != 0) return 0;  // stat error
    // else
    return st.st_size;
  #else
  */
  File file = fs.open(filepath, FILE_READ);
  if (!file) return 0;
  size_t fileSize = file.size();
  file.close();
  return fileSize;
}

String md5File(FS &fs, String filepath) {
  if(!fs.exists(filepath)) return "";
  String txt = readSmallFile(fs, filepath);
  MD5Builder md5;
  md5.begin();
  md5.add(txt);
  md5.calculate();
  return(md5.toString());
}

String crc32File(FS &fs, String filepath) {
  if(!fs.exists(filepath)) return "";
  String txt = readSmallFile(fs, filepath);
  // derived from https://techoverflow.net/2022/08/05/how-to-compute-crc32-with-ethernet-polynomial-0x04c11db7-on-esp32-crc-h/
  uint32_t romCRC = (~crc32_le((uint32_t)~(0xffffffff), (const uint8_t*)txt.c_str(), txt.length()))^0xffffffff;
  char s[18] = {0};
  char crcBytes[4] = {0};
  memcpy(crcBytes, &romCRC, sizeof(uint32_t));
  snprintf(s, sizeof(s), "%02X%02X%02X%02X\n", crcBytes[3], crcBytes[2], crcBytes[1], crcBytes[0]);
  return(String(s));
}

/***************************************************************************************
** Function name: sortList
** Description:   sort files for name
***************************************************************************************/
bool sortList(const FileList& a, const FileList& b) {
    // Order items alfabetically
    String fa=a.filename.c_str();
    fa.toUpperCase();
    String fb=b.filename.c_str();
    fb.toUpperCase();
    return fa < fb;
}

/***************************************************************************************
** Function name: checkExt
** Description:   check file extension
***************************************************************************************/
bool checkExt(String ext, String pattern) {
    ext.toUpperCase();
    pattern.toUpperCase();
    if (ext == pattern) return true;

    pattern = "^(" + pattern + ")$";

    char charArray[pattern.length() + 1];
    pattern.toCharArray(charArray, pattern.length() + 1);
    std::regex ext_regex(charArray);
    return std::regex_search(ext.c_str(), ext_regex);
}

/***************************************************************************************
** Function name: sortList
** Description:   sort files for name
***************************************************************************************/
void readFs(FS fs, String folder, String allowed_ext) {
    int allFilesCount = 0;
    fileList.clear();
    FileList object;

    File root = fs.open(folder);
    if (!root || !root.isDirectory()) {
        //Serial.println("Não foi possível abrir o diretório");
        return; // Retornar imediatamente se não for possível abrir o diretório
    }

    //Add Folders to the list
    File file = root.openNextFile();
    while (file && ESP.getFreeHeap()>1024) {
        String fileName = file.name();
        if (file.isDirectory()) {
            object.filename = fileName.substring(fileName.lastIndexOf("/") + 1);
            object.folder = true;
            object.operation=false;
            fileList.push_back(object);
        }
        file = root.openNextFile();
    }
    file.close();
    root.close();
    // Sort folders
    std::sort(fileList.begin(), fileList.end(), sortList);
    int new_sort_start=fileList.size();

    //Add files to the list
    root = fs.open(folder);
    File file2 = root.openNextFile();
    while (file2) {
        String fileName = file2.name();
        if (!file2.isDirectory()) {
            String ext = fileName.substring(fileName.lastIndexOf(".") + 1);
            if (allowed_ext=="*" || checkExt(ext, allowed_ext)) {
              object.filename = fileName.substring(fileName.lastIndexOf("/") + 1);
              object.folder = false;
              object.operation=false;
              fileList.push_back(object);
            }
        }
        file2 = root.openNextFile();
    }
    file2.close();
    root.close();

    //
    Serial.println("Files listed with: " + String(fileList.size()) + " files/folders found");

    // Order file list
    std::sort(fileList.begin()+new_sort_start, fileList.end(), sortList);

    // Adds Operational btn at the botton
    object.filename = "> Back";
    object.folder=false;
    object.operation=true;

    fileList.push_back(object);

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
  tft.fillScreen(bruceConfig.bgColor);
  tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,bruceConfig.priColor);
  if(&fs==&SD) {
    closeSdCard();
    if(!setupSdCard()){
      displayError("Fail Mounting SD", true);
      return "";
    }
  }
  bool exit = false;
  //returnToMenu=true;  // make sure menu is redrawn when quitting in any point

  readFs(fs, Folder, allowed_ext);

  maxFiles = fileList.size() - 1; //discount the >back operator
  while(1){
    //if(returnToMenu) break; // stop this loop and retur to the previous loop
    if(exit) break; // stop this loop and retur to the previous loop

    if(redraw) {
      if(strcmp(PreFolder.c_str(),Folder.c_str()) != 0 || reload){
        tft.fillScreen(bruceConfig.bgColor);
        tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,bruceConfig.priColor);
        index=0;
        Serial.println("reload to read: " + Folder);
        readFs(fs, Folder, allowed_ext);
        PreFolder = Folder;
        maxFiles = fileList.size()-1;
        reload=false;
      }
      if(fileList.size()<2) readFs(fs, Folder,allowed_ext);

      listFiles(index, fileList);
      #if defined(HAS_TOUCH)
        TouchFooter();
      #endif
      delay(REDRAW_DELAY);
      redraw = false;
    }

    #ifdef CARDPUTER
      if(checkEscPress()) break;  // quit

      /* TODO: go back 1 level instead of quitting
      if(Keyboard.isKeyPressed(KEY_BACKSPACE)) {
        // go back 1 level
          if(Folder == "/") break;
          Folder = fileList[index][1];
          index = 0;
          redraw=true;
          continue;
      }*/

      const short PAGE_JUMP_SIZE = 5;
      if(checkNextPagePress()) {
        index += PAGE_JUMP_SIZE;
        if(index>maxFiles) index=maxFiles-1; // check bounds
        redraw = true;
        continue;
      }
      if(checkPrevPagePress()) {
        index -= PAGE_JUMP_SIZE;
        if(index<0) index = 0;  // check bounds
        redraw = true;
        continue;
      }

      // check letter shortcuts

      char pressed_letter = checkLetterShortcutPress();
      if(pressed_letter>0) {
        //Serial.println(pressed_letter);
        if(tolower(fileList[index].filename.c_str()[0]) == pressed_letter) {
          // already selected, go to the next
          index += 1;
          // check if index is still valid
          if(index<=maxFiles && tolower(fileList[index].filename.c_str()[0]) == pressed_letter)
          {
            redraw = true;
            continue;
          }
        }
        // else look again from the start
        for(int i=0; i<maxFiles; i++) {
          if(tolower(fileList[i].filename.c_str()[0]) == pressed_letter) {  // check if 1st char matches
            index = i;
            redraw = true;
            break;  // quit on 1st match
          }
        }
      }
    #elif defined (T_EMBED)
      if(checkEscPress()) break;  // quit
    #endif

    if(checkPrevPress()) {
      if(index==0) index = maxFiles;
      else if(index>0) index--;
      redraw = true;
    }
    /* DW Btn to next item */
    if(checkNextPress()) {
      if(index==maxFiles) index = 0;
      else index++;
      redraw = true;
    }

    /* Select to install */
    if(checkSelPress()) {
      delay(200);

      if(checkSelPress())
      {
        // Definição da matriz "Options"
        if(fileList[index].folder==true && fileList[index].operation==false) {
          options = {
            {"New Folder", [=]() { createFolder(fs, Folder); }},
            {"Rename",     [=]() { renameFile(fs, Folder + fileList[index].filename, fileList[index].filename); }}, // Folder=="/"? "":"/" +  Attention to Folder + filename, Need +"/"+ beetween them?
            {"Delete",     [=]() { deleteFromSd(fs, Folder + fileList[index].filename); }},                         // Folder=="/"? "":"/" +  Attention to Folder + filename, Need +"/"+ beetween them?
            {"Main Menu",  [&]() { exit = true; }},
          };
          delay(200);
          loopOptions(options);
          tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,bruceConfig.priColor);
          reload = true;
          redraw = true;
        } else if(fileList[index].folder==false && fileList[index].operation==false){
          goto Files;
        } else {
          options = {
            {"New Folder", [=]() { createFolder(fs, Folder); }},
          };
          if(fileToCopy!="") options.push_back({"Paste", [=]() { pasteFile(fs, Folder); }});
          options.push_back({"Main Menu", [&]() { exit = true; }});
          delay(200);
          loopOptions(options);
          tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,bruceConfig.priColor);
          reload = true;
          redraw = true;
        }
      } else {
        Files:
        if(fileList[index].folder==true && fileList[index].operation==false) {
          Folder = Folder + (Folder=="/"? "":"/") +  fileList[index].filename; //Folder=="/"? "":"/" +
          //Debug viewer
          Serial.println(Folder);
          redraw=true;
        } else if (fileList[index].folder==false && fileList[index].operation==false) {
          //Save the file/folder info to Clear memory to allow other functions to work better
          String filepath=Folder + (Folder=="/"? "":"/") +  fileList[index].filename; //
          String filename=fileList[index].filename;
          //Debug viewer
          Serial.println(filepath + " --> " + filename);
          fileList.clear(); // Clear memory to allow other functions to work better

          options = {
            {"View File",  [=]() { viewFile(fs, filepath); }},
            {"File Info",  [=]() { fileInfo(fs, filepath); }},
            {"Rename",     [=]() { renameFile(fs, filepath, filename); }},
            {"Copy",       [=]() { copyFile(fs, filepath); }},
            {"Delete",     [=]() { deleteFromSd(fs, filepath); }},
            {"New Folder", [=]() { createFolder(fs, Folder); }},
          };
          if(fileToCopy!="") options.push_back({"Paste",  [=]() { pasteFile(fs, Folder); }});
          if(&fs == &SD) options.push_back({"Copy->LittleFS", [=]() { copyToFs(SD,LittleFS, filepath); }});
          if(&fs == &LittleFS && sdcardMounted) options.push_back({"Copy->SD", [=]() { copyToFs(LittleFS, SD, filepath); }});

          // custom file formats commands added in front
          if(filepath.endsWith(".jpg")) options.insert(options.begin(), {"View Image",  [&]() {
              showJpeg(fs, filepath);
              delay(750);
              while(!checkAnyKeyPress()) yield();
            }});
          if(filepath.endsWith(".gif")) options.insert(options.begin(), {"View Image",  [&]() {
              showGIF(fs, filepath);
              delay(750);
              while(!checkAnyKeyPress()) yield();
            }});
          if(filepath.endsWith(".ir")) options.insert(options.begin(), {"IR Tx SpamAll",  [&]() {
              delay(200);
              txIrFile(&fs, filepath);
            }});
          if(filepath.endsWith(".sub")) options.insert(options.begin(), {"Subghz Tx",  [&]() {
              delay(200);
              txSubFile(&fs, filepath);
            }});
          if(filepath.endsWith(".csv")) {
            options.insert(options.begin(), {"Wigle Upload",  [&]() {
              delay(200);
              Wigle wigle;
              wigle.upload(&fs, filepath);
            }});
            options.insert(options.begin(), {"Wigle Up All",  [&]() {
              delay(200);
              Wigle wigle;
              wigle.upload_all(&fs, Folder);
            }});
          }
          if(filepath.endsWith(".bjs") || filepath.endsWith(".js")) {
            options.insert(options.begin(), {"JS Script Run",  [&]() {
              delay(200);
              run_bjs_script_headless(fs, filepath);
              exit=true;
            }});
          }
          #if defined(USB_as_HID)
          if(filepath.endsWith(".txt")) {
            options.push_back({"BadUSB Run",  [&]() {
              Kb.begin(); USB.begin();
              key_input(fs, filepath);
              // TODO: reinit serial port
            }});
            options.push_back({"USB HID Type",  [&]() {
               String t = readSmallFile(fs, filepath);
               displayRedStripe("Typing");
               key_input_from_string(t);
            }});
          }
          if(filepath.endsWith(".enc")) {  // encrypted files
              options.insert(options.begin(), {"Decrypt+Type",  [&]() {
                  String plaintext = readDecryptedFile(fs, filepath);
                  if(plaintext.length()==0) return displayError("Decryption failed", true);  // file is too big or cannot read, or cancelled
                  // else
                  plaintext.trim();  // remove newlines
                  key_input_from_string(plaintext);
              }});
          }
          #endif
          if(filepath.endsWith(".enc")) {  // encrypted files
              options.insert(options.begin(), {"Decrypt+Show",  [&]() {
                String plaintext = readDecryptedFile(fs, filepath);
                delay(200);
                if(plaintext.length()==0) return displayError("Decryption failed", true);
                plaintext.trim();  // remove newlines
                //if(plaintext.length()<..)
                  displaySuccess(plaintext, true);
                // else
                // TODO: show in the text viewer
              }});
          }
          #if defined(HAS_NS4168_SPKR)
          if(isAudioFile(filepath)) options.insert(options.begin(), {"Play Audio",  [&]() {
            delay(200);
            playAudioFile(&fs, filepath);
            setup_gpio(); //TODO: remove after fix select loop
          }});
          #endif
          // generate qr codes from small files (<3K)
          size_t filesize = getFileSize(fs, filepath);
          //Serial.println(filesize);
          if(filesize < SAFE_STACK_BUFFER_SIZE && filesize>0) {
            options.push_back({"QR code",  [&]() {
              delay(200);
              qrcode_display(readSmallFile(fs, filepath));
            }});
            options.push_back({"CRC32",  [&]() {
              delay(200);
              displaySuccess(crc32File(fs, filepath), true);
            }});
            options.push_back({"MD5",  [&]() {
              delay(200);
              displaySuccess(md5File(fs, filepath), true);
            }});
          }

          options.push_back({"Main Menu", [&]() { exit = true; }});
          delay(200);
          if(!filePicker) loopOptions(options);
          else {
            result = filepath;
            break;
          }
          tft.drawRoundRect(5,5,WIDTH-10,HEIGHT-10,5,bruceConfig.priColor);
          reload = true;
          redraw = true;
        } else {
          if(Folder == "/") break;
          Folder = Folder.substring(0,Folder.lastIndexOf('/'));
          if(Folder=="") Folder = "/";
          Serial.println("Going to folder: " + Folder);
          index = 0;
          redraw=true;
        }
        redraw = true;
      }
    }
  }
  fileList.clear();
  return result;

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
  tft.fillScreen(bruceConfig.bgColor);
  String fileContent = "";
  File file;
  String displayText;
  int totalPages;
  int currentPage = 0;
  bool updateContent = true;

  file = fs.open(filepath, FILE_READ);
  if (!file) return;

  // TODO: detect binary file, switch to hex view
  // String header=file.read(100); file.rewind();
  // if(isValidAscii(header)) ...

  while (file.available()) {
    fileContent = file.readString();
  }
  file.close();
  delay(100);

  totalPages = createFilePages(fileContent);

  while(1) {
    if(updateContent) {
      tft.fillScreen(bruceConfig.bgColor);
      tft.setCursor(0,4);
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
    displayError("LittleFS is Full", true);
    return false;
  } else return true;
}
/*********************************************************************
**  Function: checkLittleFsSize
**  Check if there are more then 4096 bytes available for storage
**********************************************************************/
bool checkLittleFsSizeNM() {
  return (LittleFS.totalBytes() - LittleFS.usedBytes()) >= 4096;
}

/*********************************************************************
**  Function: getFsStorage
**  Function will return true and FS will point to SDFS if available
**  and LittleFS otherwise. If LittleFS is full it wil return false.
**********************************************************************/
bool getFsStorage(FS *&fs) {
  if(setupSdCard()) fs=&SD;
  else if(checkLittleFsSize()) fs=&LittleFS;
  else return false;

  return true;
}

/*********************************************************************
**  Function: fileInfo
**  Display file info
**********************************************************************/
void fileInfo(FS fs, String filepath) {
  tft.fillScreen(bruceConfig.bgColor);
  tft.setCursor(0,0);
  tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
  tft.setTextSize(FP);

  File file = fs.open(filepath, FILE_READ);
  if (!file) return;

  int bytesize = file.size();
  float filesize = bytesize;
  String unit = "B";

  time_t modifiedTime = file.getLastWrite();

  if (filesize >= 1000000) {
    filesize /= 1000000.0;
    unit = "MB";
  } else if (filesize >= 1000) {
    filesize /= 1000.0;
    unit = "kB";
  }

  padprintln("");
  tft.drawCentreString("-"+String(file.name()), WIDTH/2, tft.getCursorY(), 1);
  padprintln("\n");
  padprintln("Path: " + filepath);
  padprintln("");
  padprintf("Bytes: %d\n", bytesize);
  padprintln("");
  padprintf("Size: %.02f %s\n", filesize, unit.c_str());
  padprintln("");
  padprintf("Modified: %s\n", ctime(&modifiedTime));

  file.close();
  delay(100);

  while(1) {
    if(checkEscPress() || checkSelPress()) break;
    delay(100);
  }

  return;
}
