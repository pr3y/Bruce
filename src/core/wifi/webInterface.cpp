#include "webInterface.h"
#include "core/display.h"    // using displayRedStripe as error msg
#include "core/mykeyboard.h" // using keyboard when calling rename
#include "core/passwords.h"
#include "core/sd_functions.h" // using sd functions called to rename and manage sd files
#include "core/serialcmds.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h" // using common wifisetup
#include "esp_task_wdt.h"
#include "webFiles.h"
#include <globals.h>

File uploadFile;
FS _webFS = LittleFS;
// WiFi as a Client
const int default_webserverporthttp = 80;

// WiFi as an Access Point
IPAddress AP_GATEWAY(172, 0, 0, 1); // Gateway

AsyncWebServer *server = nullptr; // initialise webserver
const char *host = "bruce";
String uploadFolder = "";

/**********************************************************************
**  Function: stopWebUi
**  Turn off the WebUI
**********************************************************************/
void stopWebUi() {
    tft.setLogging(false);
    isWebUIActive = false;
    server->end();
    server->~AsyncWebServer();
    free(server);
    server = nullptr;
    MDNS.end();
}
/**********************************************************************
**  Function: loopOptionsWebUi
**  Display options to launch the WebUI
**********************************************************************/
void loopOptionsWebUi() {
    if (isWebUIActive) {
        bool opt = WiFi.getMode() - 1;
        options = {
            {"Stop WebUI", stopWebUi},
            {"WebUi screen", lambdaHelper(startWebUi, opt)}
        };
        addOptionToMainMenu();
        loopOptions(options);
        return;
    }
    options = {
        {"my Network", lambdaHelper(startWebUi, false)},
        {"AP mode",    lambdaHelper(startWebUi, true) },
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
String listFiles(FS fs, String folder) {
    // log_i("Listfiles Start");
    String returnText = "pa:" + folder + ":0\n";
    Serial.println("Listing files stored on SD");

    _webFS = fs;

    File root = fs.open(folder);
    File foundfile = root.openNextFile();
    if (folder == "//") folder = "/";
    uploadFolder = folder;
    String PreFolder = folder;
    PreFolder = PreFolder.substring(0, PreFolder.lastIndexOf("/"));
    if (PreFolder == "") PreFolder = "/";

    if (folder == "/") folder = "";
    while (foundfile) {
        if (esp_get_free_heap_size() > (String("Fo:" + String(foundfile.name()) + ":0\n").length()) + 1024) {
            if (foundfile.isDirectory()) returnText += "Fo:" + String(foundfile.name()) + ":0\n";
        } else break;
        foundfile = root.openNextFile();
        esp_task_wdt_reset();
    }
    root.close();
    foundfile.close();

    if (folder == "") folder = "/";
    root = fs.open(folder);
    foundfile = root.openNextFile();
    while (foundfile) {
        if (esp_get_free_heap_size() > (String("Fo:" + String(foundfile.name()) + ":0\n").length()) + 1024) {
            if (!(foundfile.isDirectory()))
                returnText +=
                    "Fi:" + String(foundfile.name()) + ":" + humanReadableSize(foundfile.size()) + "\n";
        } else break;
        foundfile = root.openNextFile();
        esp_task_wdt_reset();
    }
    root.close();
    foundfile.close();

    // log_i("ListFiles End");
    return returnText;
}

/**********************************************************************
**  Function: checkUserWebAuth
** used by server->on functions to discern whether a user has the correct
** httpapitoken OR is authenticated by username and password
**********************************************************************/
bool checkUserWebAuth(AsyncWebServerRequest *request) {
    bool isAuthenticated = false;
    if (request->authenticate(bruceConfig.webUI.user.c_str(), bruceConfig.webUI.pwd.c_str())) {
        isAuthenticated = true;
    }
    return isAuthenticated;
}

/**********************************************************************
**  Function: createDirRecursive
** Create folders recursivelly
**********************************************************************/
void createDirRecursive(String path, FS fs) {
    String currentPath = "";
    int startIndex = 0;
    Serial.print("Verifying folder: ");
    Serial.println(path);

    while (startIndex < path.length()) {
        int endIndex = path.indexOf("/", startIndex);
        if (endIndex == -1) endIndex = path.length();

        currentPath += path.substring(startIndex, endIndex);
        if (currentPath.length() > 0) {
            if (!fs.exists(currentPath)) {
                fs.mkdir(currentPath);
                Serial.print("Creating folder: ");
                Serial.println(currentPath);
            }
        }

        if (endIndex < path.length()) { currentPath += "/"; }
        startIndex = endIndex + 1;
    }
}
/**********************************************************************
**  Function: handleUpload
** handles uploads to the filserver
**********************************************************************/
void handleUpload(
    AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final
) {
    // make sure authenticated before allowing upload
    // Serial.println("Folder: " + uploadFolder);
    if (uploadFolder == "/") uploadFolder = "";

    if (checkUserWebAuth(request)) {
        if (!index) {
            if (request->hasArg("password")) filename = filename + ".enc";
            Serial.println("File: " + uploadFolder + "/" + filename);
            String relativePath = filename;
            String fullPath = uploadFolder + "/" + relativePath;
            String dirPath = fullPath.substring(0, fullPath.lastIndexOf("/"));
            if (dirPath.length() > 0) { createDirRecursive(dirPath, _webFS); }
        RETRY:
            request->_tempFile = _webFS.open(uploadFolder + "/" + filename, "w");
            if (!request->_tempFile) {
                Serial.println("Failed to open file for writing: " + uploadFolder + "/" + filename);
                vTaskDelay(pdMS_TO_TICKS(5));
                goto RETRY;
            }
        }

        if (len) {
            if (request->hasArg("password")) {
                // encryption requested
                static int chunck_no = 0;
                if (chunck_no != 0) {
                    // TODO: handle multiple chunks
                    request->send(404, "text/html", "file is too big");
                    return;
                } else chunck_no += 1;
                String enc_password = request->arg("password");
                String plaintext = String((char *)data).substring(0, len);
                String cyphertxt = encryptString(plaintext, enc_password);
                if (cyphertxt == "") { return; }
                if (request->_tempFile)
                    request->_tempFile.write((const uint8_t *)cyphertxt.c_str(), cyphertxt.length());
            } else {
                if (request->_tempFile) request->_tempFile.write(data, len);
            }
        }
        if (final) {
            // close the file handle as the upload is now done
            if (request->_tempFile) request->_tempFile.close();
        }
    }
}

void notFound(AsyncWebServerRequest *request) { request->send(404, "text/plain", "Nothing in here Sharky"); }

/**********************************************************************
**  Function: drawWebUiScreen
**  Draw information on screen of WebUI.
**********************************************************************/
void drawWebUiScreen(bool mode_ap) {
    tft.fillScreen(bruceConfig.bgColor);
    tft.fillScreen(bruceConfig.bgColor);
    tft.drawRoundRect(5, 5, tftWidth - 10, tftHeight - 10, 5, ALCOLOR);
    if (mode_ap) {
        setTftDisplay(0, 0, bruceConfig.bgColor, FM);
        tft.drawCentreString("BruceNet/brucenet", tftWidth / 2, 7, 1);
    }
    setTftDisplay(0, 0, ALCOLOR, FM);
    tft.drawCentreString("BRUCE WebUI", tftWidth / 2, 27, 1);
    String txt;
    if (!mode_ap) txt = WiFi.localIP().toString();
    else txt = WiFi.softAPIP().toString();
    tft.setTextColor(bruceConfig.priColor);

    tft.drawCentreString("http://bruce.local", tftWidth / 2, 45, 1);
    setTftDisplay(7, 67);

    tft.setTextSize(FM);
    tft.print("IP: ");
    tft.println(txt);
    tft.setCursor(7, tft.getCursorY());
    tft.println("Usr: " + String(bruceConfig.webUI.user));
    tft.setCursor(7, tft.getCursorY());
    tft.println("Pwd: " + String(bruceConfig.webUI.pwd));
    tft.setCursor(7, tft.getCursorY());
    tft.setTextColor(TFT_RED);
    tft.setTextSize(FP);

#if defined(HAS_TOUCH)
    TouchFooter();
#endif

    tft.drawCentreString("press Esc to stop", tftWidth / 2, tftHeight - 15, 1);
}

/**********************************************************************
**  Function: color565ToWebHex
**  convert 565 color to web hex format for theme purposes
**********************************************************************/
String color565ToWebHex(uint16_t color565) {
    // Extract RGB components from 565
    uint8_t r = (color565 >> 11) & 0x1F;
    uint8_t g = (color565 >> 5) & 0x3F;
    uint8_t b = color565 & 0x1F;

    // Scale up to 8 bits
    r = (r << 3) | (r >> 2);
    g = (g << 2) | (g >> 4);
    b = (b << 3) | (b >> 2);

    char hex[8];
    snprintf(hex, sizeof(hex), "#%02X%02X%02X", r, g, b);
    return String(hex);
}

/**********************************************************************
**  Function: configureWebServer
**  configure web server
**********************************************************************/
void configureWebServer() {
    MDNS.begin(host);
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server->onNotFound(notFound);

    // server->onFileUpload(handleUpload);

    server->on(
        "/upload",
        HTTP_POST,
        [](AsyncWebServerRequest *request) { request->send(200, "text/plain", "File upload completed"); },
        handleUpload
    );

    server->on("/logout", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(401, "text/html", "");
        response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        response->addHeader("Location", "/logged-out");
        request->send(response);
    });

    server->on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Client disconnected.");
        AsyncWebServerResponse *response =
            request->beginResponse(200, "text/html", logout_html, logout_html_size);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
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
            AsyncWebServerResponse *response =
                request->beginResponse(200, "text/html", index_html, index_html_size);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
        } else {
            request->requestAuthentication();
        }
    });
    server->on("/theme.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            String css = ":root{--color:" + color565ToWebHex(bruceConfig.priColor) +
                         ";--sec-color:" + color565ToWebHex(bruceConfig.secColor) +
                         ";--background:" + color565ToWebHex(bruceConfig.bgColor) + ";}";
            request->send(200, "text/css", css);
        } else {
            request->requestAuthentication();
        }
    });
    server->on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            AsyncWebServerResponse *response =
                request->beginResponse(200, "text/css", index_css, index_css_size);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
        } else {
            return request->requestAuthentication();
        }
    });
    server->on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            AsyncWebServerResponse *response =
                request->beginResponse(200, "application/javascript", index_js, index_js_size);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
        } else {
            return request->requestAuthentication();
        }
    });
    server->on("/systeminfo", HTTP_GET, [](AsyncWebServerRequest *request) {
        char response_body[300];
        uint64_t LittleFSTotalBytes = LittleFS.totalBytes();
        uint64_t LittleFSUsedBytes = LittleFS.usedBytes();
        uint64_t SDTotalBytes = SD.totalBytes();
        uint64_t SDUsedBytes = SD.usedBytes();
        sprintf(
            response_body,
            "{\"%s\":\"%s\",\"SD\":{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"},"
            "\"LittleFS\":{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}}",
            "BRUCE_VERSION",
            BRUCE_VERSION,
            "free",
            humanReadableSize(SDTotalBytes - SDUsedBytes).c_str(),
            "used",
            humanReadableSize(SDUsedBytes).c_str(),
            "total",
            humanReadableSize(SDTotalBytes).c_str(),
            "free",
            humanReadableSize(LittleFSTotalBytes - LittleFSUsedBytes).c_str(),
            "used",
            humanReadableSize(LittleFSUsedBytes).c_str(),
            "total",
            humanReadableSize(LittleFSTotalBytes).c_str()
        );
        request->send(200, "application/json", response_body);
    });

    server->on("/getscreen", HTTP_GET, [](AsyncWebServerRequest *request) {
        uint8_t binData[MAX_LOG_ENTRIES * MAX_LOG_SIZE];
        size_t binSize = 0;

        tft.getBinLog(binData, binSize);
        request->send(200, "application/octet-stream", (const uint8_t *)binData, binSize);
    });

    // WIP: Serve a folder to a custom WEBUI..
    // if (bruceConfig.webUI_folder != "") {
    //      //Chech for what fs it is using, to survey to proper folder
    //     server->serveStatic("/www", LittleFS, webUI_folder).setFilter([](AsyncWebServerRequest *request) {
    //         return checkUserWebAuth(request);
    //     });
    //     if (sdcardMounted) {
    //         server->serveStatic("/www", SD, webUI_folder).setFilter([](AsyncWebServerRequest *request) {
    //             return checkUserWebAuth(request);
    //         });
    //     }
    // }

    // Index page
    server->on("/Oc34N", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response =
            request->beginResponse(200, "text/html", not_found_html, not_found_html_size);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    });

    // Route to rename a file
    server->on("/rename", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasArg("fileName") && request->hasArg("filePath")) {
            String fs = request->arg("fs").c_str();
            String fileName = request->arg("fileName").c_str();
            String filePath = request->arg("filePath").c_str();
            String filePath2 = filePath.substring(0, filePath.lastIndexOf('/') + 1) + fileName;
            // Rename the file of folder
            if (fs == "SD") {
                if (SD.rename(filePath, filePath2))
                    request->send(200, "text/plain", filePath + " renamed to " + filePath2);
                else request->send(200, "text/plain", "Fail renaming file.");
            } else {
                if (LittleFS.rename(filePath, filePath2))
                    request->send(200, "text/plain", filePath + " renamed to " + filePath2);
                else request->send(200, "text/plain", "Fail renaming file.");
            }
        }
    });

    // Route to send a generic command (Tasmota compatible API)
    // https://tasmota.github.io/docs/Commands/#with-web-requests
    server->on("/cm", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasArg("cmnd")) {
            String cmnd = request->arg("cmnd");
            if (serialCli.parse(cmnd)) {
                // drawWebUiScreen(WiFi.getMode() == WIFI_MODE_AP ? true : false);
                int sep = cmnd.indexOf(" ");
                String firstParam = (sep >= 0) ? cmnd.substring(0, sep) : cmnd;
                if (firstParam == "nav") {
                    String response = getOptionsJSON();
                    request->send(200, "application/json", response.c_str());
                } else {
                    request->send(200, "text/plain", "command " + cmnd + " success");
                }
            } else {
                request->send(400, "text/plain", "command failed, check the serial log for details");
            }
        } else {
            request->send(400, "text/plain", "http request missing required arg: cmnd");
        }
    });

    // Reinicia o ESP
    server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            ESP.restart();
        } else {
            request->requestAuthentication();
        }
    });

    // List files of the LittleFS
    server->on("/listfiles", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            String folder = "/";
            if (request->hasArg("folder")) { folder = request->arg("folder"); }
            bool useSD = false;
            if (strcmp(request->arg("fs").c_str(), "SD") == 0) {
                request->send(200, "text/plain", listFiles(SD, folder));
            } else {
                request->send(200, "text/plain", listFiles(LittleFS, folder));
            }

        } else {
            request->requestAuthentication();
        }
    });

    // define route to handle download, create folder and delete
    server->on("/file", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            if (request->hasArg("name") && request->hasArg("action")) {
                String fileName = request->arg("name").c_str();
                String fileAction = request->arg("action").c_str();
                String fileSys = request->arg("fs").c_str();
                bool useSD = false;
                if (fileSys == "SD") useSD = true;

                FS *fs;
                if (useSD) fs = &SD;
                else fs = &LittleFS;

                log_i("filename: %s", fileName.c_str());
                log_i("fileAction: %s", fileAction);

                if (!(*fs).exists(fileName)) {
                    if (strcmp(fileAction.c_str(), "create") == 0) {
                        if ((*fs).mkdir(fileName)) {
                            request->send(200, "text/plain", "Created new folder: " + String(fileName));
                        } else {
                            request->send(200, "text/plain", "FAIL creating folder: " + String(fileName));
                        }
                    } else if (strcmp(fileAction.c_str(), "createfile") == 0) {
                        File newFile = (*fs).open(fileName, FILE_WRITE, true);
                        if (newFile) {
                            newFile.close();
                            request->send(200, "text/plain", "Created new file: " + String(fileName));
                        } else {
                            request->send(200, "text/plain", "FAIL creating file: " + String(fileName));
                        }
                    } else request->send(400, "text/plain", "ERROR: file does not exist");

                } else {
                    if (strcmp(fileAction.c_str(), "download") == 0) {
                        request->send(*fs, fileName, "application/octet-stream", true);
                    } else if (strcmp(fileAction.c_str(), "image") == 0) {
                        String extension = fileName.substring(fileName.lastIndexOf('.') + 1);
                        // https://www.iana.org/assignments/media-types/media-types.xhtml#image
                        if (extension == "jpg") extension = "jpeg"; // www.rfc-editor.org/rfc/rfc2046.html
                        request->send(*fs, fileName, "image/" + extension);
                    } else if (strcmp(fileAction.c_str(), "delete") == 0) {
                        if (deleteFromSd(*fs, fileName)) {
                            request->send(200, "text/plain", "Deleted : " + String(fileName));
                        } else {
                            request->send(200, "text/plain", "FAIL deleting: " + String(fileName));
                        }
                    } else if (strcmp(fileAction.c_str(), "create") == 0) {
                        if (SD.mkdir(fileName)) {
                            request->send(200, "text/plain", "Created new folder: " + String(fileName));
                        } else {
                            request->send(200, "text/plain", "FAIL creating folder: " + String(fileName));
                        }
                    } else if (strcmp(fileAction.c_str(), "createfile") == 0) {
                        File newFile = SD.open(fileName, FILE_WRITE, true);
                        if (newFile) {
                            newFile.close();
                            request->send(200, "text/plain", "Created new file: " + String(fileName));
                        } else {
                            request->send(200, "text/plain", "FAIL creating file: " + String(fileName));
                        }

                    } else if (strcmp(fileAction.c_str(), "edit") == 0) {
                        File editFile = (*fs).open(fileName, FILE_READ);
                        if (editFile) {
                            String fileContent = editFile.readString();
                            request->send(200, "text/plain", fileContent);
                            editFile.close();
                        } else {
                            request->send(500, "text/plain", "Failed to open file for reading");
                        }

                    } else {
                        request->send(400, "text/plain", "ERROR: invalid action param supplied");
                    }
                }
            } else {
                request->send(400, "text/plain", "ERROR: name and action params required");
            }
        } else {
            request->requestAuthentication();
        }
    });

    server->on("/edit", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            if (request->hasArg("name") && request->hasArg("content") && request->hasArg("fs")) {
                String fileName = request->arg("name");
                String fileContent = request->arg("content");
                bool useSD = false;

                if (strcmp(request->arg("fs").c_str(), "SD") == 0) { useSD = true; }

                fs::FS *fs = useSD ? (fs::FS *)&SD : (fs::FS *)&LittleFS;
                String fsType = useSD ? "SD" : "LittleFS";

                if ((useSD && !setupSdCard()) || (!useSD && !LittleFS.begin())) {
                    request->send(500, "text/plain", "Failed to initialize file system: " + fsType);
                    return;
                }

                File editFile = fs->open(fileName, FILE_WRITE);
                if (editFile) {
                    if (editFile.write((const uint8_t *)fileContent.c_str(), fileContent.length())) {
                        request->send(200, "text/plain", "File edited: " + fileName);
                    } else {
                        request->send(500, "text/plain", "Failed to write to file: " + fileName);
                    }
                    editFile.close();
                } else {
                    request->send(500, "text/plain", "Failed to open file for writing: " + fileName);
                }
            } else {
                request->send(400, "text/plain", "ERROR: name, content, and fs parameters required");
            }
        } else {
            request->requestAuthentication();
        }
    });

    // Wi-Fi configuration on web page
    server->on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkUserWebAuth(request)) {
            if (request->hasArg("usr") && request->hasArg("pwd")) {
                const char *usr = request->arg("usr").c_str();
                const char *pwd = request->arg("pwd").c_str();
                bruceConfig.setWebUICreds(usr, pwd);
                request->send(
                    200, "text/plain", "User: " + String(usr) + " configured with password: " + String(pwd)
                );
            }
        } else {
            request->requestAuthentication();
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
        if (mode_ap) wifiConnectMenu(WIFI_AP);
        else wifiConnectMenu(WIFI_STA);
    } else {
        keepWifiConnected = true;
    }

    // configure web server
    if (!server) {
        // Clear this vector to free stack memory
        options.clear();

        Serial.println("Configuring Webserver ...");
        if (psramFound()) server = (AsyncWebServer *)ps_malloc(sizeof(AsyncWebServer));
        else server = (AsyncWebServer *)malloc(sizeof(AsyncWebServer));

        new (server) AsyncWebServer(default_webserverporthttp);

        configureWebServer();

        isWebUIActive = true;
    }
    tft.setLogging();
    drawWebUiScreen(mode_ap);
#ifdef HAS_SCREEN // Headless always run in the background!
    while (!check(EscPress)) {
        // nothing here, just to hold the screen until the server is on.
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    bool closeServer = false;

    options.clear();
    options.emplace_back("Exit", [&closeServer]() { closeServer = true; });
    options.emplace_back("Run in background", []() {});

    loopOptions(options);

    if (closeServer) {
        stopWebUi();

        delay(100);
        if (!keepWifiConnected) { wifiDisconnect(); }
    }
#endif
}
