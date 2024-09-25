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

void setup() {

    tft.init();
    tft.setRotation(3);

    qrcode.init();
    // create qrcode
    qrcode.create("Hello world.");

}

void loop() { }
