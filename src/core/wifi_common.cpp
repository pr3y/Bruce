#include "globals.h"
#include "wifi_common.h"
#include "mykeyboard.h" // usinf keyboard when calling rename
#include "display.h"    // using displayRedStripe  and loop options
#include "settings.h"
#include "eeprom.h"
#include "powerSave.h"

/***************************************************************************************
** Function name: wifiConnect
** Description:   Connects to wifiNetwork
***************************************************************************************/
bool wifiConnect(String ssid, int encryption, bool isAP)
{
  if (isAP) return wifiApConnect();

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
    _updateClockTimezone();
  }

  delay(200);
  return connected;
}

bool _connectToWifiNetwork(String ssid, String pwd) {
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

void _updateClockTimezone() {
  timeClient.begin();
  timeClient.update();

  timeClient.setTimeOffset(bruceConfig.tmz * 3600);

  localTime = myTZ.toLocal(timeClient.getEpochTime());

#if !defined(HAS_RTC)
  rtc.setTime(timeClient.getEpochTime());
  updateTimeStr(rtc.getTimeStruct());
  clock_set = true;
#endif
}

/***************************************************************************************
** Function name: wifiApConnect
** Description:   Create wifi Access Point
***************************************************************************************/
bool wifiApConnect()
{
  IPAddress AP_GATEWAY(172, 0, 0, 1);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
  WiFi.softAP(bruceConfig.wifiAp.ssid, bruceConfig.wifiAp.pwd, 6, 0, 4, false);
  wifiIP = WiFi.softAPIP().toString(); // update global var
  Serial.println("IP: " + wifiIP);
  wifiConnected = true;
  return true;
}

/***************************************************************************************
** Function name: wifiDisconnect
** Description:   disconnects and turn off the WIFI module
***************************************************************************************/
void wifiDisconnect()
{
  WiFi.softAPdisconnect(true); // turn off AP mode
  WiFi.disconnect(true, true); // turn off STA mode
  WiFi.mode(WIFI_OFF);         // enforces WIFI_OFF mode
  wifiConnected = false;
  returnToMenu = true;
}

/***************************************************************************************
** Function name: wifiConnectMenu
** Description:   Opens a menu to connect to a wifi
***************************************************************************************/
bool wifiConnectMenu(bool isAP)
{
  if (isAP) return wifiApConnect();

  if (WiFi.status() != WL_CONNECTED) {
    int nets;
    WiFi.mode(WIFI_MODE_STA);
    displayRedStripe("Scanning..", TFT_WHITE, bruceConfig.priColor);
    nets = WiFi.scanNetworks();
    options = {};
    for (int i = 0; i < nets; i++) {
      options.push_back(
        {WiFi.SSID(i).c_str(), [=]() { wifiConnect(WiFi.SSID(i).c_str(), int(WiFi.encryptionType(i))); }}
      );
    }
    options.push_back({"Main Menu", [=]()
                       { backToMenu(); }});
    delay(200);
    loopOptions(options);
    delay(200);
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
        _updateClockTimezone();
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
