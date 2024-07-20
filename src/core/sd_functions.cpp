#include "globals.h"    
#include "sd_functions.h"
#include "mykeyboard.h"   // usinf keyboard when calling rename
#include "display.h"      // using displayRedStripe as error msg

SPIClass sdcardSPI;
String fileToCopy;
String fileList[MAXFILES][3];


/***************************************************************************************
** Function name: setupSdCard
** Description:   Start SD Card
***************************************************************************************/
bool setupSdCard() {
  sdcardSPI.begin(SDCARD_SCK, SDCARD_MISO, SDCARD_MOSI, SDCARD_CS); // start SPI communications
  delay(10);
  if (!SD.begin(SDCARD_CS, sdcardSPI)) { 
    sdcardSPI.end(); // Closes SPI connections and release pin header.
    //Serial.println("Failed to mount SDCARD");
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

  if (!SD.begin()) { result = false; displayError("Error 1"); } 
  if(!LittleFS.begin()) { result = false; displayError("Error 2"); } 
  
  File source = from.open(path, FILE_READ);
  if (!source) {
    displayError("Error 3");
    result = false;
  }
  path = path.substring(path.lastIndexOf('/'));
  if(!path.startsWith("/")) path = "/" + path;
  File dest = to.open(path, FILE_WRITE);
  if (!dest) {
    displayError("Error 4");
    result = false;
  }
  size_t bytesRead;
  int tot=source.size();
  int prog=0;
  //tft.drawRect(5,HEIGHT-12, (WIDTH-10), 9, FGCOLOR);
  while ((bytesRead = source.read(buffer, bufferSize)) > 0) {
    if (dest.write(buffer, bytesRead) != bytesRead) {
      //Serial.println("Falha ao escrever no arquivo de destino");
      source.close();
      dest.close();
      result = false;
      displayError("Error 5");
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
            name1.toUpperCase();
            name2 = fileList[i + 1][0];
            name2.toUpperCase();

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
** Function name: sortList
** Description:   sort files for name
***************************************************************************************/
void readFs(FS fs, String folder, String result[][3]) {

    int allFilesCount = 0;
    while(allFilesCount<MAXFILES) {
      result[allFilesCount][0]="";
      result[allFilesCount][1]="";
      result[allFilesCount][2]="";
      allFilesCount++;
    }
    allFilesCount=0;

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
            else {
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
String loopSD(FS &fs, bool filePicker) {
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

  readFs(fs, Folder, fileList);

  for(int i=0; i<MAXFILES; i++) if(fileList[i][2]!="") maxFiles++; else break;
  while(1){
    if(returnToMenu) break; // stop this loop and retur to the previous loop

    if(redraw) { 
      if(strcmp(PreFolder.c_str(),Folder.c_str()) != 0 || reload){
        index=0;
        readFs(fs, Folder, fileList);
        PreFolder = Folder;
        maxFiles=0;
        for(int i=0; i<MAXFILES; i++) if(fileList[i][2]!="") maxFiles++; else break;
        reload=false;
      }
      listFiles(index, fileList);

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
      
      #if defined(CARDPUTER)
      Keyboard.update();
      if(Keyboard.isKeyPressed(KEY_ENTER)) 
      #else
      if(digitalRead(SEL_BTN)==LOW) 
      #endif
      {
        // Definição da matriz "Options" 
        if(fileList[index][2]=="folder") {
          options = {
            {"New Folder", [=]() { createFolder(fs, Folder); }},
            {"Rename", [=]() { renameFile(fs, fileList[index][1], fileList[index][0]); }},
            {"Delete", [=]() { deleteFromSd(fs, fileList[index][1]); }},
            {"Main Menu", [=]() { backToMenu(); }},
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
          options.push_back({"Main Menu", [=]() { backToMenu(); }});
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
          options = {
            {"New Folder", [=]() { createFolder(fs, Folder); }},
            {"Rename", [=]() { renameFile(fs, fileList[index][1], fileList[index][0]); }},
            {"Copy", [=]() { copyFile(fs, fileList[index][1]); }},
          };
          if(fileToCopy!="") options.push_back({"Paste",  [=]() { pasteFile(fs, Folder); }});
          options.push_back({"Delete", [=]() { deleteFromSd(fs, fileList[index][1]); }});
          if(&fs == &SD) options.push_back({"Copy->LittleFS", [=]() { copyToFs(SD,LittleFS, fileList[index][1]); }});
          if(&fs == &LittleFS && sdcardMounted) options.push_back({"Copy->SD", [=]() { copyToFs(LittleFS, SD, fileList[index][1]); }});

          options.push_back({"Main Menu", [=]() { backToMenu(); }});
          delay(200);
          if(!filePicker) loopOptions(options);
          else { 
            result = fileList[index][1];
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
      if(checkEscPress()) break;
    #endif
  }
  return result;
  //closeSdCard();
  //setupSdCard();  
}


