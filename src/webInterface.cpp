#include "globals.h"
#include "webInterface.h"
#include "sd_functions.h" // using sd functions called to rename and manage sd files
#include "wifi_common.h"  // using common wifisetup
#include "mykeyboard.h"   // using keyboard when calling rename
#include "display.h"      // using displayRedStripe as error msg


struct Config {
  String httpuser;
  String httppassword;       // password to access web admin
  int webserverporthttp;     // http port number for web admin
};

File uploadFile;
  // WiFi as a Client
String default_httpuser = "admin";  
String default_httppassword = "bruce";
const int default_webserverporthttp = 80;

//WiFi as an Access Point
IPAddress AP_GATEWAY(172, 0, 0, 1);  // Gateway

Config config;                        // configuration

WebServer* server=nullptr;               // initialise webserver
const char* host = "bruce";
const String fileconf = "/bruce.txt";
String uploadFolder="";



/**********************************************************************
**  Function: webUIMyNet
**  Display options to launch the WebUI
**********************************************************************/
void webUIMyNet() {
  if (WiFi.status() != WL_CONNECTED) {
    if(wifiConnectMenu()) startWebUi(false); 
    else {
      displayError("Wifi Offline");
    }
  } else {
    //If it is already connected, just start the network
    startWebUi(false); 
  }
  // On fail installing will run the following line
}


/**********************************************************************
**  Function: loopOptionsWebUi
**  Display options to launch the WebUI
**********************************************************************/
void loopOptionsWebUi() {
  // Definição da matriz "Options"
  std::vector<std::pair<std::string, std::function<void()>>> options = {
      {"my Network", [=]() { webUIMyNet(); }},
      {"AP mode", [=]()    { startWebUi(true); }},
  };
  delay(200);

  loopOptions(options);
  // On fail installing will run the following line
}


/**********************************************************************
**  Function: humanReadableSize
** Make size of files human readable
** source: https://github.com/CelliesProjects/minimalUploadAuthESP32
**********************************************************************/
String humanReadableSize(uint64_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " kB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}



/**********************************************************************
**  Function: listFiles
**  list all of the files, if ishtml=true, return html rather than simple text
**********************************************************************/
String listFiles(FS fs, bool ishtml, String folder, bool isLittleFS) {
  String returnText = "";
  Serial.println("Listing files stored on SD");

  if (ishtml) {
    returnText += "<table><tr><th align='left'>Name</th><th style=\"text-align=center;\">Size</th><th></th></tr>\n";
  }
  File root = fs.open(folder);
  File foundfile = root.openNextFile();
  String fileSys="SD";
  if (isLittleFS) fileSys="LittleFS";
  if (folder=="//") folder = "/";
  uploadFolder = folder;
  String PreFolder = folder;
  PreFolder = PreFolder.substring(0, PreFolder.lastIndexOf("/"));
  if(PreFolder=="") PreFolder= "/";
  returnText += "<tr><th align='left'><a onclick=\"listFilesButton('"+ PreFolder + "', '"+ fileSys +"')\" href='javascript:void(0);'>... </a></th><th align='left'></th><th></th></tr>\n";

  if (folder=="/") folder = "";
  while (foundfile) {
    if(foundfile.isDirectory()) {
      if (ishtml) {
        returnText += "<tr align='left'><td><a onclick=\"listFilesButton('"+ String(foundfile.path()) + "', '"+ fileSys +"')\" href='javascript:void(0);'>\n" + String(foundfile.name()) + "</a></td>";
        returnText += "<td></td>\n";
        returnText += "<td><i style=\"color: #ffabd7;\" class=\"gg-folder\" onclick=\"listFilesButton('" + String(foundfile.path()) + "')\"></i>&nbsp&nbsp";
        returnText += "<i style=\"color: #ffabd7;\" class=\"gg-rename\"  onclick=\"renameFile(\'" + String(foundfile.path()) + "\', \'" + String(foundfile.name()) + "\')\"></i>&nbsp&nbsp\n";
        returnText += "<i style=\"color: #ffabd7;\" class=\"gg-trash\"  onclick=\"downloadDeleteButton(\'" + String(foundfile.path()) + "\', \'delete\')\"></i></td></tr>\n\n";
      } else {
        returnText += "Folder: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
      }
    }
    foundfile = root.openNextFile();
  }
  root.close();
  foundfile.close();

  if (folder=="") folder = "/";
  root = fs.open(folder);
  foundfile = root.openNextFile();
  while (foundfile) {
    if(!(foundfile.isDirectory())) {
      if (ishtml) {
        returnText += "<tr align='left'><td>" + String(foundfile.name());
        if (String(foundfile.name()).substring(String(foundfile.name()).lastIndexOf('.') + 1).equalsIgnoreCase("bin")) returnText+= "&nbsp<i class=\"rocket\" onclick=\"startUpdate(\'" + String(foundfile.path()) + "\')\"></i>";
        returnText += "</td>\n";
        returnText += "<td style=\"font-size: 10px; text-align=center;\">" + humanReadableSize(foundfile.size()) + "</td>\n";
        returnText += "<td><i class=\"gg-arrow-down-r\" onclick=\"downloadDeleteButton(\'"+ String(foundfile.path()) + "\', \'download\')\"></i>&nbsp&nbsp\n";
        returnText += "<i class=\"gg-rename\"  onclick=\"renameFile(\'" + String(foundfile.path()) + "\', \'" + String(foundfile.name()) + "\')\"></i>&nbsp&nbsp\n";
        returnText += "<i class=\"gg-trash\"  onclick=\"downloadDeleteButton(\'" + String(foundfile.path()) + "\', \'delete\')\"></i></td></tr>\n\n";
      } else {
        returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
      }
    }
    foundfile = root.openNextFile();
  }
  root.close();
  foundfile.close();

  if (ishtml) {
    returnText += "</table>";
  }

  return returnText;
}

/**********************************************************************
**  Function: processor
** parses and processes webpages if the webpage has %SOMETHING%
** or %SOMETHINGELSE% it will replace those strings with the ones defined
**********************************************************************/

String processor(const String& var) {
  String processedHtml = var;
  processedHtml.replace("%FIRMWARE%", String(BRUCE_VERSION));
  processedHtml.replace("%FREESD%", humanReadableSize(SD.totalBytes() - SD.usedBytes()));
  processedHtml.replace("%USEDSD%", humanReadableSize(SD.usedBytes()));
  processedHtml.replace("%TOTALSD%", humanReadableSize(SD.totalBytes()));

  processedHtml.replace("%FREELittleFS%", humanReadableSize(LittleFS.totalBytes() - LittleFS.usedBytes()));
  processedHtml.replace("%USEDLittleFS%", humanReadableSize(LittleFS.usedBytes()));
  processedHtml.replace("%TOTALLittleFS%", humanReadableSize(LittleFS.totalBytes()));
  
  return processedHtml;
}


/**********************************************************************
**  Function: checkUserWebAuth
** used by server->on functions to discern whether a user has the correct
** httpapitoken OR is authenticated by username and password
**********************************************************************/
bool checkUserWebAuth() {
  bool isAuthenticated = false;
  if (server->authenticate(config.httpuser.c_str(), config.httppassword.c_str())) {
    isAuthenticated = true;
  }
  return isAuthenticated;
}



/**********************************************************************
**  Function: handleUpload
** handles uploads to the filserver
**********************************************************************/
void handleFileUpload(FS fs) {
  HTTPUpload& upload = server->upload();
  String filename = upload.filename;
  if (upload.status == UPLOAD_FILE_START) {
    if (!filename.startsWith("/")) filename = "/" + filename;
    if (uploadFolder != "/") filename = uploadFolder + filename;
    fs.remove(filename);
    uploadFile = fs.open(filename, "w");
    Serial.println("Upload Start: " + filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) uploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
      Serial.println("Upload End: " + filename);
      server->sendHeader("Location", "/"); // Redireciona para a raiz
      server->send(303);
    }
  }
}


/**********************************************************************
**  Function: configureWebServer
**  configure web server
**********************************************************************/
void configureWebServer() {
  MDNS.begin(host);

  // Configura rota padrão para arquivo não encontrado
  server->onNotFound([]() {
    server->send(404, "text/html", "Nothing in here, sharky!");
  });

  // Visitar esta página fará com que você seja solicitado a se autenticar
  server->on("/logout", HTTP_GET, []() {
    server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server->sendHeader("Location", "/logged-out", true); // Redireciona para a página de login
    server->requestAuthentication();
    server->send(302); // Código de status para redirecionamento
  });

  // Página que apresenta que você está desconectado
  server->on("/logged-out", HTTP_GET, []() {
    String logMessage = "Cliente desconectado.";
    Serial.println(logMessage);
    server->send(200, "text/html", logout_html);
  });

  // Uploadfile handler
  server->on("/uploadSD", HTTP_POST, []() {
    server->send(200, "text/plain", "Upload iniciado");
  }, []() {handleFileUpload(SD);});

  // Uploadfile handler
  server->on("/uploadLittleFS", HTTP_POST, []() {
    server->send(200, "text/plain", "Upload iniciado");
  }, []() { handleFileUpload(LittleFS); });

  // Index page
  server->on("/", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      server->send(200, "text/html", processor(index_html));
    } else {
      server->requestAuthentication();
    }
  });

  // Index page
  server->on("/Oc34N", HTTP_GET, []() {
      server->send(200, "text/html", page_404);
  });

  // Route to rename a file
  server->on("/rename", HTTP_POST, []() {
    if (server->hasArg("fileName") && server->hasArg("filePath"))  { 
      String fs = server->arg("fs").c_str();
      String fileName = server->arg("fileName").c_str();
      String filePath = server->arg("filePath").c_str();
      String filePath2 = filePath.substring(0,filePath.lastIndexOf('/')+1) + fileName;
      // Rename the file of folder
      if(fs == "SD") {
        if (SD.rename(filePath, filePath2)) server->send(200, "text/plain", filePath + " renamed to " + filePath2);
        else server->send(200, "text/plain", "Fail renaming file.");
      } else {
        if (LittleFS.rename(filePath, filePath2)) server->send(200, "text/plain", filePath + " renamed to " + filePath2);
        else server->send(200, "text/plain", "Fail renaming file.");
      }

    }
  });

  // Reinicia o ESP
  server->on("/reboot", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      ESP.restart();
    } else {
      server->requestAuthentication();
    }
  });

  // List files of the LittleFS
  server->on("/listfiles", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      String folder = "/";
      if (server->hasArg("folder")) {
        folder = server->arg("folder");
      }
      bool useSD = false;
      if (strcmp(server->arg("fs").c_str(), "SD") == 0) useSD = true;

      if (useSD) server->send(200, "text/plain", listFiles(SD, true, folder,false));
      else server->send(200, "text/plain", listFiles(LittleFS, true, folder, true));

    } else {
      server->requestAuthentication();
    }
  });

  // define route to handle download, create folder and delete
  server->on("/file", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      if (server->hasArg("name") && server->hasArg("action")) {
        String fileName = server->arg("name").c_str();
        String fileAction = server->arg("action").c_str();
        String fileSys = server->arg("fs").c_str();
        bool useSD = false;
        if (fileSys == "SD") useSD = true;

        FS *fs;
        if (useSD) fs = &SD;
        else fs = &LittleFS;

        log_i("filename: %s", fileName);
        log_i("fileAction: %s", fileAction);

        if (!(*fs).exists(fileName)) {
          if (strcmp(fileAction.c_str(), "create") == 0) {
            if ((*fs).mkdir(fileName)) {
              server->send(200, "text/plain", "Created new folder: " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL creating folder: " + String(fileName));
            } 
          } else server->send(400, "text/plain", "ERROR: file does not exist");

        } else {
          if (strcmp(fileAction.c_str(), "download") == 0) {
            File downloadFile = (*fs).open(fileName, FILE_READ);
            if (downloadFile) {
              String contentType = "application/octet-stream";
              server->setContentLength(downloadFile.size());
              server->sendHeader("Content-Type", contentType, true);
              server->sendHeader("Content-Disposition", "attachment; filename=\"" + String(downloadFile.name()) + "\"");
              server->streamFile(downloadFile, contentType);
              downloadFile.close();
            } else {
              server->send(500, "text/plain", "Failed to open file for reading");
            }
          } else if (strcmp(fileAction.c_str(), "delete") == 0) {
            if (deleteFromSd(*fs, fileName)) {
              server->send(200, "text/plain", "Deleted : " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL deleting: " + String(fileName));
            }
          } else if (strcmp(fileAction.c_str(), "create") == 0) {
            if (SD.mkdir(fileName)) {
              server->send(200, "text/plain", "Created new folder: " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL creating folder: " + String(fileName));
            }
          } else {
            server->send(400, "text/plain", "ERROR: invalid action param supplied");
          }
        }
      } else {
        server->send(400, "text/plain", "ERROR: name and action params required");
      }
    } else {
      server->requestAuthentication();
    }
  });

  // Configuração de Wi-Fi via página web
  server->on("/wifi", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      if (server->hasArg("usr") && server->hasArg("pwd")) {
        const char *ssid = server->arg("usr").c_str();
        const char *pwd = server->arg("pwd").c_str();
        SD.remove(fileconf);
        File file = SD.open(fileconf, FILE_WRITE);
        file.print(String(ssid) + ";" + String(pwd) + ";\n");
        config.httpuser = ssid;
        config.httppassword = pwd;
        file.print("#ManagerUser;ManagerPassword;");
        file.close();
        server->send(200, "text/plain", "User: " + String(ssid) + " configured with password: " + String(pwd));
      }
    } else {
      server->requestAuthentication();
    }
  });
  server->begin();
}
/**********************************************************************
**  Function: startWebUi
**  Start the WebUI
**********************************************************************/
void startWebUi(bool mode_ap) {

  config.httpuser     = default_httpuser;
  config.httppassword = default_httppassword;
  config.webserverporthttp = default_webserverporthttp;

  if(setupSdCard()) {
    if(SD.exists(fileconf)) {
      Serial.println("File Exists, reading " + fileconf);
      File file = SD.open(fileconf, FILE_READ);
      if(file) {
        default_httpuser = readLineFromFile(file);
        default_httppassword = readLineFromFile(file);
        config.httpuser     = default_httpuser;
        config.httppassword = default_httppassword;

        file.close();
      }
    }
    else {
      File file = SD.open(fileconf, FILE_WRITE);
      file.print( default_httpuser + ";" + default_httppassword + ";\n");
      file.print("#ManagerUser;ManagerPassword;");
      file.close();
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    // Choose wifi access mode
    wifiConnectMenu(mode_ap);
  }
  
  // configure web server
  Serial.println("Configuring Webserver ...");
#ifdef STICK_C_PLUS2
  server=(WebServer*)ps_malloc(sizeof(WebServer));
#else
  server=(WebServer*)malloc(sizeof(WebServer));
#endif
  new (server) WebServer(config.webserverporthttp);

  configureWebServer();

  tft.fillScreen(BGCOLOR);
  tft.drawSmoothRoundRect(5,5,5,5,WIDTH-10,HEIGHT-10,ALCOLOR,BGCOLOR);
  setTftDisplay(0,0,ALCOLOR,FM);
  tft.drawCentreString("BRUCE WebUI",tft.width()/2,7,1);
  String txt;
  if(!mode_ap) txt = WiFi.localIP().toString();
  else txt = WiFi.softAPIP().toString();
  tft.setTextColor(FGCOLOR);
  
#ifndef STICK_C
  tft.drawCentreString("http://bruce.local", tft.width()/2,25,1);
  setTftDisplay(7,47);
#else
  tft.drawCentreString("http://bruce.local", tft.width()/2,17,1);
  setTftDisplay(7,26);
#endif
  tft.setTextSize(FM);
  tft.print("IP: ");   tft.println(txt);
  tft.setCursor(7,tft.getCursorY());
  tft.println("Usr: " + String(default_httpuser));
  tft.setCursor(7,tft.getCursorY());
  tft.println("Pwd: " + String(default_httppassword));
  tft.setCursor(7,tft.getCursorY());
  tft.setTextColor(TFT_RED);
  tft.setTextSize(FP);

  #ifdef CARDPUTER
  tft.drawCentreString("press Esc to stop", tft.width()/2,tft.height()-15,1);
  #else
  tft.drawCentreString("press Pwr to stop", tft.width()/2,tft.height()-15,1);
  #endif

  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();
  while (!checkEscPress()) {
      server->handleClient();
      // nothing here, just to hold the screen until the server is on.
  }
  server->close();
  server->~WebServer();
  free(server);
  server = nullptr;
  MDNS.end();

  delay(100);
  wifiDisconnect();

}