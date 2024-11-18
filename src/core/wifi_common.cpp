#include "globals.h"
#include "wifi_common.h"
#include "mykeyboard.h" // usinf keyboard when calling rename
#include "display.h"    // using displayRedStripe  and loop options
#include "settings.h"
#include "powerSave.h"
#include "utils.h"

bool _wifiConnect(const String& ssid, int encryption)
{
  String password = bruceConfig.getWifiPassword(ssid);
  if (password == "" && encryption > 0) {
    password = keyboard(password, 63, "Network Password:");
  }
  bool connected = _connectToWifiNetwork(ssid, password);
  bool retry = false;;

  while (!connected) {
    wakeUpScreen();

    options = {
      {"Retry",  [&]() { retry = true;  }},
      {"Cancel", [&]() { retry = false; }},
    };
    loopOptions(options);
    delay(200);

    if (!retry) {
      wifiDisconnect();
      return false;
    }

    password = keyboard(password, 63, "Network Password:");
    connected = _connectToWifiNetwork(ssid, password);
  }

  if (connected) {
    wifiConnected = true;
    wifiIP = WiFi.localIP().toString();
    bruceConfig.addWifiCredential(ssid, password);
    updateClockTimezone();
  }

  delay(200);
  return connected;
}

bool _connectToWifiNetwork(const String& ssid, const String& pwd) {
  drawMainBorderWithTitle("WiFi Connect");
  padprintln("");
  padprint("Connecting to: " + ssid + ".");

  WiFi.begin(ssid, pwd);

  int i = 1;
  while (WiFi.status() != WL_CONNECTED) {
    if (tft.getCursorX() >= WIDTH-12) {
      padprintln("");
      padprint("");
    }
    tft.print(".");

    if (i > 20) {
      displayError("Wifi Offline");
      delay(500);
      break;
    }

    delay(500);
    i++;
  }

  return WiFi.status() == WL_CONNECTED;
}

bool _setupAP()
{
  IPAddress AP_GATEWAY(172, 0, 0, 1);
  WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
  WiFi.softAP(bruceConfig.wifiAp.ssid, bruceConfig.wifiAp.pwd, 6, 0, 4, false);
  wifiIP = WiFi.softAPIP().toString(); // update global var
  Serial.println("IP: " + wifiIP);
  wifiConnected = true;
  return true;
}

void wifiDisconnect()
{
  WiFi.softAPdisconnect(true); // turn off AP mode
  WiFi.disconnect(true, true); // turn off STA mode
  WiFi.mode(WIFI_OFF);         // enforces WIFI_OFF mode
  wifiConnected = false;
  returnToMenu = true;
}

bool wifiConnectMenu(wifi_mode_t mode)
{
  if( WiFi.isConnected() ) return false; // safeguard

  switch (mode)
  {
  case WIFI_AP: // access point
    WiFi.mode(WIFI_AP);
    return _setupAP();
  break;
  
  case WIFI_STA: // station mode
    int nets;
    WiFi.mode(WIFI_MODE_STA);
    displayRedStripe("Scanning..", TFT_WHITE, bruceConfig.priColor);
    nets = WiFi.scanNetworks();
    options = {};
    for (int i = 0; i < nets; i++) {
      options.emplace_back(
        WiFi.SSID(i).c_str(), [=]() { _wifiConnect(WiFi.SSID(i), int(WiFi.encryptionType(i))); }
      );
    }
    options.emplace_back( "Main Menu", [=](){ backToMenu(); });
    delay(200);
    loopOptions(options);
    delay(200);
  break;

  case WIFI_AP_STA: // repeater mode
    // _setupRepeater();
  break;

  default: // error handling
    Serial.println("Unknown wifi mode: " + String(mode));
  break;
  }

  if (returnToMenu)
    return false;
  return wifiConnected;
}

void wifiConnectTask(int maxSearch)
{
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_MODE_STA);
  int nets = WiFi.scanNetworks();
  String ssid;
  String pwd;

  for (int i = 0; i < min(nets, maxSearch); i++) {
    ssid = WiFi.SSID(i).c_str();
    pwd = bruceConfig.getWifiPassword(ssid);
    if (pwd == "") continue;

    WiFi.begin(ssid, pwd);
    for (int i = 0; i<20; i++) {
      if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        wifiIP = WiFi.localIP().toString();
        updateClockTimezone();
        return;
      }
      delay(300);
    }
  }

  return;
}

void checkMAC()
{
  drawMainBorderWithTitle("MAC ADDRESS");
  padprintln("\n");
  padprintln(WiFi.macAddress());

  delay(200);
  while (!checkAnyKeyPress())
  {
    delay(80);
  }
}
