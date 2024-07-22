#include "globals.h"
#include "wifi_common.h"
#include "mykeyboard.h"   // usinf keyboard when calling rename
#include "display.h"      // using displayRedStripe  and loop options
#include "settings.h"


/***************************************************************************************
** Function name: wifiConnect
** Description:   Connects to wifiNetwork
***************************************************************************************/
bool wifiConnect(String ssid, int encryptation, bool isAP) {
  if(!isAP) {
    int tmz;
    EEPROM.begin(EEPROMSIZE);
    tmz = EEPROM.read(10);        // read timezone
    if(tmz>8) tmz=0;
    bool found = false;
    bool wrongPass = false;
    getConfigs();
    JsonObject setting = settings[0];
    JsonArray WifiList = setting["wifi"].as<JsonArray>();

    pwd = EEPROM.readString(20); //password

    if (sdcardMounted) {
      for (JsonObject wifiEntry : WifiList) {
        String name = wifiEntry["ssid"].as<String>();
        String pass = wifiEntry["pwd"].as<String>();
        log_i("SSID: %s, Pass: %s", name, pass);
        if (name == ssid) {
          pwd = pass;
          found = true;
          log_i("Found SSID: %s", name);
          break;
        }
      }
    }

  Retry:
    if (!found || wrongPass) {
      delay(200);
      if (encryptation > 0) pwd = keyboard(pwd, 63, "Network Password:");

      EEPROM.begin(EEPROMSIZE);
      if (pwd != EEPROM.readString(20)) {
        EEPROM.writeString(20, pwd);
        EEPROM.commit(); // Store data to EEPROM
      }
      EEPROM.end(); // Free EEPROM memory
      if (sdcardMounted && !found) {
        // Cria um novo objeto JSON para adicionar ao array "wifi"
        JsonObject newWifi = WifiList.add<JsonObject>();
        newWifi["ssid"] = ssid;
        newWifi["pwd"] = pwd;
        found=true;
        saveConfigs();
      } else if (sdcardMounted && found && wrongPass) {
        for (JsonObject wifiEntry : WifiList) {
          if (wifiEntry["ssid"].as<String>() == ssid) {
            wifiEntry["pwd"] = pwd;
            log_i("Mudou pwd de SSID: %s", ssid);
            break;
          }
        }
        saveConfigs();
      }

    }

    drawMainBorder();
    tft.setCursor(7,27);
    tft.print("Connecting to: " + ssid + ".");
    int i=0;
    WiFi.begin(ssid, pwd);

    while (WiFi.status() != WL_CONNECTED) {
      if(tft.getCursorY()!=27 && tft.getCursorX()==0) tft.setCursor(7,tft.getCursorY());
      tft.print(".");
      i++;
      if(i>20) {
        displayError("Wifi Offline");
        wifiDisconnect();
        delay(500);
        return false;
      }
      delay(500);
    }

    if(WiFi.status() == WL_CONNECTED) {
      wifiConnected=true;
      timeClient.begin();
      timeClient.update();
      if(tmz==0) timeClient.setTimeOffset(-3 * 3600);
      if(tmz==1) timeClient.setTimeOffset(-2 * 3600);
      if(tmz==2) timeClient.setTimeOffset(-4 * 3600);
      if(tmz==3) timeClient.setTimeOffset(1 * 3600);
      if(tmz==4) timeClient.setTimeOffset(8 * 3600);
      if(tmz==5) timeClient.setTimeOffset(10 * 3600);
      if(tmz==6) timeClient.setTimeOffset(9 * 3600);
      if(tmz==7) timeClient.setTimeOffset(3 * 3600);
      if(tmz==8) timeClient.setTimeOffset(2 * 3600);
      localTime = myTZ.toLocal(timeClient.getEpochTime());
      rtc.setTime(timeClient.getEpochTime());
      updateTimeStr(rtc.getTimeStruct());
      clock_set=true;
      return true;
    }

    else return false;

  } else { //Running in Access point mode
    IPAddress AP_GATEWAY(172, 0, 0, 1);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
    WiFi.softAP("BruceNet", "",6,0,4,false);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
    wifiConnected=true;
    return true;
  }
  delay(200);
  sprite.deleteSprite();
  returnToMenu=true;
}


/***************************************************************************************
** Function name: wifiDisconnect
** Description:   disconnects and turn off the WIFI module
***************************************************************************************/
void wifiDisconnect() {
  WiFi.softAPdisconnect(true); // turn off AP mode
  WiFi.disconnect(true,true);  // turn off STA mode
  WiFi.mode(WIFI_OFF);         // enforces WIFI_OFF mode
  wifiConnected=false;
  returnToMenu=true;
}


/***************************************************************************************
** Function name: wifiConnectMenu
** Description:   Opens a menu to connect to a wifi
***************************************************************************************/
bool wifiConnectMenu(bool isAP) {
  if(isAP) {
    wifiConnect("",0,true);
  }
  else if (WiFi.status() != WL_CONNECTED) {
    int nets;
    WiFi.mode(WIFI_MODE_STA);
    displayRedStripe("Scanning..",TFT_WHITE,FGCOLOR);
    nets=WiFi.scanNetworks();
    options = { };
    for(int i=0; i<nets; i++){
      options.push_back({WiFi.SSID(i).c_str(), [=]() { wifiConnect(WiFi.SSID(i).c_str(),int(WiFi.encryptionType(i)), false); }});
    }
    delay(200);
    loopOptions(options);
    delay(200);
  }

  return wifiConnected;
}

