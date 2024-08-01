/* *********************************************************************************
 * 
 * dependency library :
 *   TFT_eSPI by Bodmer https://github.com/Bodmer/TFT_eSPI
 *
***********************************************************************************/


#include <TFT_eSPI.h>
#include <qrcode.h>

TFT_eSPI tft = TFT_eSPI();
QRcode qrcode (&tft);

String ssid = "testWiFi"; //Network SSID. Required. 
String security = "WPA2"; // "WEP", "WPA", "WPA2", "WPA3" or "nopass" for open
String password = "password"; //Password, ignored if security is "nopass"

void setup() {

    tft.init();
    tft.setRotation(3);

    qrcode.init();
    // create qrcode
    qrcode.create("WIFI:S:"+ssid+";T:"+security+";P:"+password+";;");

}

void loop() { }