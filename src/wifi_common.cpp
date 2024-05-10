#include "globals.h"
#include "wifi_common.h"
#include "mykeyboard.h"   // usinf keyboard when calling rename
#include "display.h"      // using displayRedStripe  and loop options


/***************************************************************************************
** Function name: wifiConnect
** Description:   Connects to wifiNetwork
***************************************************************************************/
bool wifiConnect(String ssid, int encryptation, bool isAP) {
  sprite.deleteSprite();
  sprite.createSprite(WIDTH-12,HEIGHT-12);

  if(!isAP) {

    EEPROM.begin(EEPROMSIZE);
    pwd = EEPROM.readString(10); //43

    delay(200);
    if(encryptation>0) pwd = keyboard(pwd,63, "Network Password:");

    if (pwd!=EEPROM.readString(10)) { //43
      EEPROM.writeString(10, pwd); //43
      EEPROM.commit(); // Store data to EEPROM
      EEPROM.end(); // Free EEPROM memory
    }

    WiFi.begin(ssid, pwd);

    resetSpriteDisplay(0, 0, FGCOLOR,FP);
    sprite.fillRect(0,0,sprite.width(),sprite.height(),BGCOLOR);

    sprite.print("Connecting to: " + ssid + ".");
    while (WiFi.status() != WL_CONNECTED && !checkSelPress()) {
      delay(500);
      sprite.print(".");
      sprite.pushSprite(6,6);
      tft.drawSmoothRoundRect(5,5,5,5,WIDTH-10,HEIGHT-10,FGCOLOR,BGCOLOR);
    }

    if(WiFi.status() == WL_CONNECTED) { 
      wifiConnected=true;
      return true;
    }

    else return false;

  } else { //Running in Access point mode
    IPAddress AP_GATEWAY(172, 0, 0, 1);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(AP_GATEWAY, AP_GATEWAY, IPAddress(255, 255, 255, 0));
    WiFi.softAP("BruceNet", "",6,0,1,false);
    Serial.print("IP: "); Serial.println(WiFi.softAPIP());
    wifiConnected=true;
    return true;
  } 
  delay(200);
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
}


/***************************************************************************************
** Function name: wifiConnectMenu
** Description:   Opens a menu to connect to a wifi
***************************************************************************************/
void wifiConnectMenu(bool isAP) {
  if(isAP) {
    wifiConnect("",0,true);
  } 
  else if (WiFi.status() != WL_CONNECTED) {
    int nets;
    WiFi.mode(WIFI_MODE_STA);
    displayScanning();
    nets=WiFi.scanNetworks();
    options = { };
    for(int i=0; i<nets; i++){
      options.push_back({WiFi.SSID(i).c_str(), [=]() { wifiConnect(WiFi.SSID(i).c_str(),int(WiFi.encryptionType(i))); }});
    }
    delay(200);
    loopOptions(options);
    delay(200);
  } 
  sprite.createSprite(WIDTH-20,HEIGHT-20);
  // On fail installing will run the following line

}

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3){
    if (arg == 31337)
      return 1;
    else
      return 0;
}