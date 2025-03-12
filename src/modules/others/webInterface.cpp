#include <globals.h>
#include "core/sd_functions.h" // using sd functions called to rename and manage sd files
#include "core/wifi_common.h"  // using common wifisetup
#include "core/mykeyboard.h"   // using keyboard when calling rename
#include "core/display.h"      // using displayRedStripe as error msg
#include "core/serialcmds.h"
#include "core/passwords.h"
#include "core/settings.h"
#include "webInterface.h"
#include "webFiles.h"


File uploadFile;
  // WiFi as a Client
const int default_webserverporthttp = 80;

//WiFi as an Access Point
IPAddress AP_GATEWAY(172, 0, 0, 1);  // Gateway

WebServer* server=nullptr;               // initialise webserver
const char* host = "bruce";
String uploadFolder="";

/**********************************************************************
**  Function: loopOptionsWebUi
**  Display options to launch the WebUI
**********************************************************************/
void loopOptionsWebUi() {
  options = {
      {"my Network", [=]() { startWebUi(false); }},
      {"AP mode", [=]()    { startWebUi(true); }},
  };

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
    if(ESP.getFreeHeap()<1024) break;
    if(foundfile.isDirectory()) {
      String foundfilePath = foundfile.path();
      String foundfileName = foundfile.name();
      if (ishtml) {
        returnText += "<tr align='left'><td><a onclick=\"listFilesButton('"+ foundfilePath + "', '"+ fileSys +"')\" href='javascript:void(0);'>\n" + foundfileName + "</a></td>";
        returnText += "<td></td>\n";
        returnText += "<td><i style=\"color: #ffabd7;\" class=\"gg-folder\" onclick=\"listFilesButton('" + foundfilePath + "')\"></i>&nbsp&nbsp";
        returnText += "<i style=\"color: #ffabd7;\" class=\"gg-rename\"  onclick=\"renameFile(\'" + foundfilePath + "\', \'" + foundfileName + "\')\"></i>&nbsp&nbsp\n";
        returnText += "<i style=\"color: #ffabd7;\" class=\"gg-trash\"  onclick=\"downloadDeleteButton(\'" + foundfilePath + "\', \'delete\')\"></i></td></tr>\n\n";
      } else {
        returnText += "Folder: " + foundfileName + " Size: " + humanReadableSize(foundfile.size()) + "\n";
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
    if(ESP.getFreeHeap()<1024) break;
    if(!(foundfile.isDirectory())) {
      String foundfilePath = foundfile.path();
      String foundfileName = foundfile.name();
      if (ishtml) {
        returnText += "<tr align='left'><td>" + foundfileName;
        returnText += "</td>\n";
        returnText += "<td style=\"font-size: 10px; text-align=center;\">" + humanReadableSize(foundfile.size()) + "</td>\n";
        returnText += "<td><i class=\"gg-arrow-down-r\" onclick=\"downloadDeleteButton(\'"+ foundfilePath + "\', \'download\')\"></i>&nbsp&nbsp\n";
        //if (foundfileName.endsWith("bin")) returnText+= "<i class=\"gg-arrow-up-r\" onclick=\"startUpdate(\'" + foundfilePath + "\')\"></i>&nbsp&nbsp\n";
        if (foundfileName.endsWith(".sub")) returnText+= "<i class=\"gg-data\" onclick=\"sendSubFile(\'" + foundfilePath + "\')\"></i>&nbsp&nbsp\n";
        if (foundfileName.endsWith(".ir")) returnText+= "<i class=\"gg-data\" onclick=\"sendIrFile(\'" + foundfilePath + "\')\"></i>&nbsp&nbsp\n";
        if (foundfileName.endsWith(".js")) returnText+= "<i class=\"gg-data\" onclick=\"runJsFile(\'" + foundfilePath + "\')\"></i>&nbsp&nbsp\n";
        if (foundfileName.endsWith(".bjs")) returnText+= "<i class=\"gg-data\" onclick=\"runJsFile(\'" + foundfilePath + "\')\"></i>&nbsp&nbsp\n";
        #if defined(USB_as_HID)
          if (foundfileName.endsWith(".txt")) returnText+= "<i class=\"gg-data\" onclick=\"runBadusbFile(\'" + foundfilePath + "\')\"></i>&nbsp&nbsp\n";
          if (foundfileName.endsWith(".enc")) returnText+= "<i class=\"gg-data\" onclick=\"decryptAndType(\'" + foundfilePath + "\')\"></i>&nbsp&nbsp\n";
        #endif
        returnText += "<i class=\"gg-rename\"  onclick=\"renameFile(\'" + foundfilePath + "\', \'" + foundfileName + "\')\"></i>&nbsp&nbsp\n";
        returnText += "<i class=\"gg-trash\"  onclick=\"downloadDeleteButton(\'" + foundfilePath + "\', \'delete\')\"></i>&nbsp&nbsp\n";
        returnText += "<i class=\"gg-pen\"  onclick=\"downloadDeleteButton(\'" + foundfilePath + "\', \'edit\')\"></i></td></tr>\n\n";
      } else {
        returnText += "File: " + foundfileName + " Size: " + humanReadableSize(foundfile.size()) + "\n";
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
**  Function: checkUserWebAuth
** used by server->on functions to discern whether a user has the correct
** httpapitoken OR is authenticated by username and password
**********************************************************************/
bool checkUserWebAuth() {
  bool isAuthenticated = false;
  if (server->authenticate(bruceConfig.webUI.user.c_str(), bruceConfig.webUI.pwd.c_str())) {
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
  if (server->hasArg("password")) filename = filename + ".enc";
  if (upload.status == UPLOAD_FILE_START) {
    if (!filename.startsWith("/")) filename = "/" + filename;
    if (uploadFolder != "/") filename = uploadFolder + filename;
    fs.remove(filename);
    uploadFile = fs.open(filename, "w");
    Serial.println("Upload Start: " + filename);
  } else if (upload.status == UPLOAD_FILE_WRITE && uploadFile) {
      if (server->hasArg("password")) {
        // encryption requested
        static int chunck_no = 0;
        if(chunck_no != 0) {
          // TODO: handle multiple chunks
          server->send(404, "text/html", "file is too big");
          return;
        } else chunck_no += 1;
        String enc_password = server->arg("password");
        // upload to ram, encrypt, then write cypertext
        //Serial.println(enc_password);
        String plaintext = String((char*)upload.buf).substring(0, upload.currentSize);
        //Serial.println(plaintext);
        //Serial.println(upload.currentSize);
        String cyphertxt = encryptString(plaintext, enc_password);
        if(cyphertxt=="") return;
        uploadFile.write((const uint8_t*) cyphertxt.c_str(), cyphertxt.length());
      } else {
        // write directly
        uploadFile.write(upload.buf, upload.currentSize);
      }
  } else if (upload.status == UPLOAD_FILE_END && uploadFile) {
      uploadFile.close();
      Serial.println("Upload End: " + filename);
      server->sendHeader("Location", "/"); // Redireciona para a raiz
      server->send(303);
  }
}
/**********************************************************************
**  Function: drawWebUiScreen
**  Draw information on screen of WebUI.
**********************************************************************/
void drawWebUiScreen(bool mode_ap) {
  tft.fillScreen(bruceConfig.bgColor);
  tft.fillScreen(bruceConfig.bgColor);
  tft.drawRoundRect(5,5,tftWidth-10,tftHeight-10,5,ALCOLOR);
  if(mode_ap) {
    setTftDisplay(0,0,bruceConfig.bgColor,FM);
    tft.drawCentreString("BruceNet/brucenet",tftWidth/2,7,1);
  }
  setTftDisplay(0,0,ALCOLOR,FM);
  tft.drawCentreString("BRUCE WebUI",tftWidth/2,27,1);
  String txt;
  if(!mode_ap) txt = WiFi.localIP().toString();
  else txt = WiFi.softAPIP().toString();
  tft.setTextColor(bruceConfig.priColor);

  tft.drawCentreString("http://bruce.local", tftWidth/2,45,1);
  setTftDisplay(7,67);

  tft.setTextSize(FM);
  tft.print("IP: ");   tft.println(txt);
  tft.setCursor(7,tft.getCursorY());
  tft.println("Usr: " + String(bruceConfig.webUI.user));
  tft.setCursor(7,tft.getCursorY());
  tft.println("Pwd: " + String(bruceConfig.webUI.pwd));
  tft.setCursor(7,tft.getCursorY());
  tft.setTextColor(TFT_RED);
  tft.setTextSize(FP);

  #if defined(HAS_TOUCH)
    TouchFooter();
  #endif

  tft.drawCentreString("press Esc to stop", tftWidth/2,tftHeight-15,1);

}

/**********************************************************************
**  Function: configureWebServer
**  configure web server
**********************************************************************/
void configureWebServer() {
  MDNS.begin(host);

  server->onNotFound([]() {
    server->send(404, "text/html", "Nothing in here, sharky!");
  });

  server->on("/logout", HTTP_GET, []() {
    server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server->sendHeader("Location", "/logged-out", true);
    server->requestAuthentication();
    server->send(302); // Status code for redirection
  });

  server->on("/logged-out", HTTP_GET, []() {
    String logMessage = "Client disconnected.";
    Serial.println(logMessage);
    server->sendHeader("Content-Encoding", "gzip");
    server->send_P(200, "text/html", logout_html);
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
      // WIP: custom webui page serving
      /*
      FS* fs = NULL;
      File custom_index_html_file = NONE;
      if(SD.exists("/webui.html")) fs = &SD;
      if(LittleFS.exists("/webui.html")) fs = &LittleFS;
      if(fs) {
        // try to read the custom page and serve that
        File custom_index_html_file =  fs->open("/webui.html", FILE_READ);
        if(custom_index_html_file) {
          // read the whole file
          //server->send(200, "text/html", custom_index_html);
        }
      }
      */
      // just serve the hardcoded page
      server->sendHeader("Content-Encoding", "gzip");
      server->send_P(200, "text/html", index_html, index_html_size);
    } else {
      server->requestAuthentication();
    }
  });
  server->on("/style.css", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      server->sendHeader("Content-Encoding", "gzip");
      server->send_P(200, "text/css", index_css, index_css_size);
    } else {
      server->requestAuthentication();
    }
  });
  server->on("/script.js", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      server->sendHeader("Content-Encoding", "gzip");
      server->send_P(200, "application/javascript", index_js, index_js_size);
    } else {
      server->requestAuthentication();
    }
  });
  server->on("/systeminfo", HTTP_GET, []() {
    char response_body[250];
    size_t LittleFSTotalBytes = LittleFS.totalBytes();
    size_t LittleFSUsedBytes = LittleFS.usedBytes();
    size_t SDTotalBytes = SD.totalBytes();
    size_t SDUsedBytes = SD.usedBytes();
    sprintf(response_body,
      "{\"%s\":\"%s\",\"SD\":{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"},\"LittleFS\":{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}}",
      "BRUCE_VERSION", BRUCE_VERSION,
      "free", humanReadableSize(SDTotalBytes - SDUsedBytes).c_str(),
      "used", humanReadableSize(SDUsedBytes).c_str(),
      "total", humanReadableSize(SDTotalBytes).c_str(),
      "free", humanReadableSize(LittleFSTotalBytes - LittleFSUsedBytes).c_str(),
      "used", humanReadableSize(LittleFSUsedBytes).c_str(),
      "total", humanReadableSize(LittleFSTotalBytes).c_str()
    );
    server->send(200, "application/json", response_body);
  });

  // Index page
  server->on("/Oc34N", HTTP_GET, []() {
    server->sendHeader("Content-Encoding", "gzip");
    server->send_P(200, "text/html", not_found_html, not_found_html_size);
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

  // Route to send a generic command (Tasmota compatible API) https://tasmota.github.io/docs/Commands/#with-web-requests
  server->on("/cm", HTTP_POST, []() {
    if (server->hasArg("cmnd"))  {
      String cmnd = server->arg("cmnd");
      if (serialCli.parse(cmnd)) {
        drawWebUiScreen(WiFi.getMode() == WIFI_MODE_AP ? true:false);
        server->send(200, "text/plain", "command " + cmnd + " success");
      } else {
        server->send(400, "text/plain", "command failed, check the serial log for details");
      }
    }
    server->send(400, "text/plain", "http request missing required arg: cmnd");
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
          }
          else if (strcmp(fileAction.c_str(), "createfile") == 0) {
            File newFile = (*fs).open(fileName, FILE_WRITE, true);
            if (newFile) {
              newFile.close();
              server->send(200, "text/plain", "Created new file: " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL creating file: " + String(fileName));
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
          } else if (strcmp(fileAction.c_str(), "createfile") == 0) {
            File newFile = SD.open(fileName, FILE_WRITE, true);
            if (newFile) {
              newFile.close();
              server->send(200, "text/plain", "Created new file: " + String(fileName));
            } else {
              server->send(200, "text/plain", "FAIL creating file: " + String(fileName));
            }

          } else if (strcmp(fileAction.c_str(), "edit") == 0) {
            File editFile = (*fs).open(fileName, FILE_READ);
            if (editFile) {
              String fileContent = editFile.readString();
              server->send(200, "text/plain", fileContent);
              editFile.close();
            } else {
              server->send(500, "text/plain", "Failed to open file for reading");
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

  server->on("/edit", HTTP_POST, [](){
    if (checkUserWebAuth()) {
        if (server->hasArg("name") && server->hasArg("content") && server->hasArg("fs")) {
            String fileName = server->arg("name");
            String fileContent = server->arg("content");
            bool useSD = false;

            if (strcmp(server->arg("fs").c_str(), "SD") == 0) {
                useSD = true;
            }

            fs::FS *fs = useSD ? (fs::FS*)&SD : (fs::FS*)&LittleFS;
            String fsType = useSD ? "SD" : "LittleFS";

            if ((useSD && !SD.begin()) || (!useSD && !LittleFS.begin())) {
                server->send(500, "text/plain", "Failed to initialize file system: " + fsType);
                return;
            }

            File editFile = fs->open(fileName, FILE_WRITE);
            if (editFile) {
                if (editFile.write((const uint8_t*)fileContent.c_str(), fileContent.length())) {
                    server->send(200, "text/plain", "File edited: " + fileName);
                } else {
                    server->send(500, "text/plain", "Failed to write to file: " + fileName);
                }
                editFile.close();
            } else {
                server->send(500, "text/plain", "Failed to open file for writing: " + fileName);
            }
        } else {
            server->send(400, "text/plain", "ERROR: name, content, and fs parameters required");
        }
    } else {
        server->requestAuthentication();
    } });

  // Wi-Fi configuration on web page
  server->on("/wifi", HTTP_GET, []() {
    if (checkUserWebAuth()) {
      if (server->hasArg("usr") && server->hasArg("pwd")) {
        const char *usr = server->arg("usr").c_str();
        const char *pwd = server->arg("pwd").c_str();
        bruceConfig.setWebUICreds(usr, pwd);
        server->send(200, "text/plain", "User: " + String(usr) + " configured with password: " + String(pwd));
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
  setupSdCard();

  bool keepWifiConnected = false;
  if (WiFi.status() != WL_CONNECTED) {
    if( mode_ap )
      wifiConnectMenu(WIFI_AP);
    else
      wifiConnectMenu(WIFI_STA);
  } else {
    keepWifiConnected = true;
  }

  // configure web server
  Serial.println("Configuring Webserver ...");
  if(psramFound()) server=(WebServer*)ps_malloc(sizeof(WebServer));
  else server=(WebServer*)malloc(sizeof(WebServer));

  new (server) WebServer(default_webserverporthttp);

  configureWebServer();
  drawWebUiScreen(mode_ap);

  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();
  options.clear(); // Clear this vector to free stack memory

  while (!check(EscPress)) {
      server->handleClient();
      // nothing here, just to hold the screen until the server is on.
  }
  server->close();
  server->~WebServer();
  free(server);
  server = nullptr;
  MDNS.end();

  delay(100);
  if(!keepWifiConnected) wifiDisconnect();
  enableCore0WDT();
  enableCore1WDT();
  enableLoopWDT();
  feedLoopWDT();

}
