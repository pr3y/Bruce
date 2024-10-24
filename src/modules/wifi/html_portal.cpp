#include "html_portal.h" 

WebServer* htmlPortal_ep = nullptr; 
DNSServer htmlPortal_dnsServer;  

String htmlPortal_html_file;
String htmlPortal_AP_name = "Html Wifi";
String htmlPortal_output_file = "creds.csv"; 
String htmlPortal_selectedIP = "192.168.4.1"; 

void startHtmlPortal() {
    htmlPortal_AP_name = keyboard("Html Wifi", 30, "Enter SSID for your WiFi:");

    bool use_sd = setupSdCard();
    FS* fs;
    if (use_sd) {
        fs = &SD;
    } else {
        fs = &LittleFS;
    }

    htmlPortal_html_file = loopSD(*fs, true);
    if (htmlPortal_html_file.isEmpty()) {
        Serial.println("No file selected, returning to the main menu.");
        return; 
    }

    if (!htmlPortal_html_file.endsWith(".html")) {
        htmlPortal_html_file = getHtmlTemplate(); 
    } else {
        File file = (*fs).open(htmlPortal_html_file, FILE_READ);
        htmlPortal_html_file = file.readString();
        file.close();
    }

    options = {
        {"172.0.0.1", [&]() { htmlPortal_selectedIP = "172.0.0.1"; }},
        {"192.168.4.1", [&]() { htmlPortal_selectedIP = "192.168.4.1"; }},
    };
    delay(200);
    loopOptions(options);

    WiFi.mode(WIFI_MODE_AP);
    IPAddress AP_GATEWAY;
    if (htmlPortal_selectedIP == "172.0.0.1") {
        AP_GATEWAY = IPAddress(172, 0, 0, 1);
    } else {
        AP_GATEWAY = IPAddress(192, 168, 4, 1);
    }

    WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
    WiFi.softAP(htmlPortal_AP_name);

    htmlPortal_ep = new WebServer(80); 
    htmlPortal_ep->on("/", []() {
        htmlPortal_ep->send(200, "text/html", htmlPortal_html_file);  
    });
    htmlPortal_ep->begin();

    bool redraw = true;
    bool exitPortal = false;

    while (!exitPortal) {
        if (redraw) {
            drawMainBorder();
            tft.setTextSize(2);
            tft.setTextColor(TFT_WHITE);
            tft.drawCentreString("HTML Portal", tft.width() / 2, 29, SMOOTH_FONT);
            tft.setCursor(8, 46);
            tft.setTextColor(FGCOLOR);
            tft.println("AP: " + htmlPortal_AP_name.substring(0, 15));
            tft.setCursor(8, tft.getCursorY());
            tft.println("-> " + htmlPortal_selectedIP + "/");

            redraw = false; 
        }

        htmlPortal_ep->handleClient();  

        if (checkEscPress()) {
            exitPortal = true;  
        }

        delay(100); 
    }

    htmlPortal_ep->close();
    delete htmlPortal_ep;
    htmlPortal_dnsServer.stop();
    WiFi.softAPdisconnect();
}

String getHtmlTemplate() {
    return "<!DOCTYPE html><html><head><title>Custom Portal</title></head><body><h1>Welcome to Custom Portal!</h1></body></html>";
}
