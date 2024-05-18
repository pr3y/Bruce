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

// variables
// command = U_SPIFFS = 100
// command = U_FLASH = 0
int command = 0;
bool updateFromSd_var = false;
bool update;

size_t file_size;
  // WiFi as a Client
String default_httpuser = "admin";  
String default_httppassword = "bruce";
const int default_webserverporthttp = 80;

//WiFi as an Access Point
IPAddress AP_GATEWAY(172, 0, 0, 1);  // Gateway

Config config;                        // configuration

AsyncWebServer *server = nullptr;               // initialise webserver
const char* host = "bruce";
const String fileconf = "/bruce.txt";
bool shouldReboot = false;            // schedule a reboot
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
  sprite.createSprite(WIDTH-20,HEIGHT-20);
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
  sprite.createSprite(WIDTH-20,HEIGHT-20);
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
String listFiles(bool ishtml, String folder) {
  String returnText = "";
  Serial.println("Listing files stored on SD");

  if (ishtml) {
    returnText += "<table><tr><th align='left'>Name</th><th style=\"text-align=center;\">Size</th><th></th></tr>\n";
  }
  File root = SD.open(folder);
  File foundfile = root.openNextFile();
  if (folder=="//") folder = "/";
  uploadFolder = folder;
  String PreFolder = folder;
  PreFolder = PreFolder.substring(0, PreFolder.lastIndexOf("/"));
  if(PreFolder=="") PreFolder= "/";
  returnText += "<tr><th align='left'><a onclick=\"listFilesButton('"+ PreFolder + "')\" href='javascript:void(0);'>... </a></th><th align='left'></th><th></th></tr>\n";

  if (folder=="/") folder = "";
  while (foundfile) {
    if(foundfile.isDirectory()) {
      if (ishtml) {
        returnText += "<tr align='left'><td><a onclick=\"listFilesButton('"+ String(foundfile.path()) + "')\" href='javascript:void(0);'>\n" + String(foundfile.name()) + "</a></td>";
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
  root = SD.open(folder);
  foundfile = root.openNextFile();
  while (foundfile) {
    if(!(foundfile.isDirectory())) {
      if (ishtml) {
        returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td>\n";
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
  if (var == "FIRMWARE") return String(BRUCE_VERSION);
  else if (var == "FREESD") return humanReadableSize(SD.totalBytes() - SD.usedBytes());
  else if (var == "USEDSD") return humanReadableSize(SD.usedBytes());
  else if (var == "TOTALSD") return humanReadableSize(SD.totalBytes());
  else return "Attribute not configured"; 
}


/**********************************************************************
**  Function: checkUserWebAuth
** used by server.on functions to discern whether a user has the correct
** httpapitoken OR is authenticated by username and password
**********************************************************************/
bool checkUserWebAuth(AsyncWebServerRequest * request) {
  bool isAuthenticated = false;
  if (request->authenticate(config.httpuser.c_str(), config.httppassword.c_str())) {
    isAuthenticated = true;
  }
  return isAuthenticated;
}

/**********************************************************************
**  Function: handleUpload
** handles uploads to the filserver
**********************************************************************/
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // make sure authenticated before allowing upload
  Serial.println("Folder: " + uploadFolder);  
  if (uploadFolder=="/") uploadFolder = "";

  if (checkUserWebAuth(request)) {
    if (!index) {
        // open the file on first call and store the file handle in the request object
        request->_tempFile = SD.open(uploadFolder + "/" + filename, "w");
    }

    if (len) {
      // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);
    }

    if (final) {
        // close the file handle as the upload is now done
        request->_tempFile.close();
        request->redirect("/");
      enableCore0WDT();
    }
  } else {
    return request->requestAuthentication();
  }
}
/**********************************************************************
**  Function: notFound
** handles not found requests
**********************************************************************/
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}



/**********************************************************************
**  Function: configureWebServer
**  configure web server
**********************************************************************/
void configureWebServer() {

  MDNS.begin(host);
  
  // if url isn't found
  server->onNotFound([](AsyncWebServerRequest * request) {
    request->redirect("/");
  });

  // visiting this page will cause you to be logged out
  server->on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->requestAuthentication();
    request->send(401);
  });

  // presents a "you are now logged out webpage
  server->on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);
    request->send_P(401, "text/html", logout_html, processor);
  });

  server->on("/rename", HTTP_POST, [](AsyncWebServerRequest * request) {
      if (request->hasParam("fileName", true) && request->hasParam("filePath", true))  { 
        String fileName = request->getParam("fileName", true)->value().c_str();
        String filePath = request->getParam("filePath", true)->value().c_str();
        String filePath2 = filePath.substring(0,filePath.lastIndexOf('/')+1) + fileName;
        if(!SD.begin()) {
            request->send(200, "text/plain", "Fail starting SD Card.");
        } else {
          // Rename the file of folder
          if (SD.rename(filePath, filePath2)) {
              request->send(200, "text/plain", filePath + " renamed to " + filePath2);
          } else {
              request->send(200, "text/plain", "Fail renaming file.");
          }
        }
      }
  });

  // run handleUpload function when any file is uploaded
  server->onFileUpload(handleUpload);


  server->on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (checkUserWebAuth(request)) {
      request->send_P(200, "text/html", index_html, processor);
    } else {
      return request->requestAuthentication();
    }
  });

  server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (checkUserWebAuth(request)) {
      ESP.restart();
    } else {
      return request->requestAuthentication();
    }
  });

  server->on("/listfiles", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (checkUserWebAuth(request)) {
      update = false;
      String folder = "";
      if (request->hasParam("folder")) {
        folder = request->getParam("folder")->value().c_str();
      } else {
        String folder = "/";
      }
      request->send(200, "text/plain", listFiles(true, folder));

    } else {
      return request->requestAuthentication();
    }
  });

  server->on("/file", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (checkUserWebAuth(request)) {
      if (request->hasParam("name") && request->hasParam("action")) {
        const char *fileName = request->getParam("name")->value().c_str();
        const char *fileAction = request->getParam("action")->value().c_str();

        if (!SD.exists(fileName)) {
          request->send(400, "text/plain", "ERROR: file does not exist");
        } else {
          if (strcmp(fileAction, "download") == 0) {
            request->send(SD, fileName, "application/octet-stream");
          } else if (strcmp(fileAction, "delete") == 0) {
            if(deleteFromSd(fileName)) { request->send(200, "text/plain", "Deleted : " + String(fileName)); }
            else { request->send(200, "text/plain", "FAIL delating: " + String(fileName));}
            
          } else if (strcmp(fileAction, "create") == 0) {
            if(SD.mkdir(fileName)) {
            } else { request->send(200, "text/plain", "FAIL creating folder: " + String(fileName));}
            request->send(200, "text/plain", "Created new folder: " + String(fileName));
          } else {
            request->send(400, "text/plain", "ERROR: invalid action param supplied");
          }
        }
      } else {
        request->send(400, "text/plain", "ERROR: name and action params required");
      }
    } else {
      return request->requestAuthentication();
    }
  });

  server->on("/wifi", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (checkUserWebAuth(request)) {
      if (request->hasParam("usr") && request->hasParam("pwd")) {
        const char *ssid = request->getParam("usr")->value().c_str();
        const char *pwd = request->getParam("pwd")->value().c_str();
        SD.remove(fileconf);
        File file = SD.open(fileconf, FILE_WRITE);        
        file.print(String(ssid) + ";" + String(pwd) + ";\n");
        config.httpuser = ssid;
        config.httppassword = pwd;
        file.print("#ManagerUser;ManagerPassword;");
        file.close();
        request->send(200, "text/plain", "User: " + String(ssid) + " configured with password: " + String(pwd));
      }
      } else {
        return request->requestAuthentication();
    }
  });

  server->on("/Oc34N", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(404, "text/html", page_404);
  });

}

/**********************************************************************
**  Function: startWebUi
**  Start the WebUI
**********************************************************************/
void startWebUi(bool mode_ap) {

config.httpuser     = default_httpuser;
config.httppassword = default_httppassword;
config.webserverporthttp = default_webserverporthttp;
file_size = 0;

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
  #if defined(CARDPUTER) || defined(STICK_C_PLUS2)
  server = (AsyncWebServer*)malloc(sizeof(AsyncWebServer));
  #else
  server = (AsyncWebServer*)malloc(sizeof(AsyncWebServer));
  #endif
  new (server) AsyncWebServer(config.webserverporthttp);
  configureWebServer();
  server->begin();

  drawMainBorder();
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
  while (!checkSelPress()) {
      // nothing here, just to hold the screen until the server is on.
  }
  server->reset();
  server->end();
  server->~AsyncWebServer();
  free(server);

  server = nullptr;


  delay(100);
  wifiDisconnect();

}