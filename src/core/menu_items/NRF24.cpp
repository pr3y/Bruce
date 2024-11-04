#include "NRF24.h"
#include "core/display.h"
#include "modules/NRF24/nrf_common.h"
#include "modules/NRF24/nrf_jammer.h"
#include "modules/NRF24/nrf_spectrum.h"

void NRF24Menu::optionsMenu() {
    options.clear();
    options.push_back({"Information",  [=]() { nrf_info(); }});
  #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
    options.push_back({"Spectrum",     [=]() { nrf_spectrum(&CC_NRF_SPI); }});
  #elif defined(CARDPUTER) || defined(ESP32S3DEVKITC1)
    options.push_back({"Spectrum",     [=]() { nrf_spectrum(&sdcardSPI); }});
  #else
    options.push_back({"Spectrum",     [=]() { nrf_spectrum(&SPI); }});
  #endif
    options.push_back({"Jammer 2.4G",  [=]() { nrf_jammer(); }});

    options.push_back({"Main Menu",    [=]() { backToMenu(); }});
    delay(200);
    loopOptions(options,false,true,"Bluetooth");
}

String NRF24Menu::getName() {
    return _name;
}

void NRF24Menu::draw() {
    // Blank
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);

    // Case
    tft.drawRect(0+iconX,40+iconY,60,40,bruceConfig.priColor);
    tft.fillRect(60+iconX,55+iconY,10,10,bruceConfig.priColor);
    //Antenna
    tft.fillRoundRect(70+iconX,10+iconY,10,55,5,bruceConfig.priColor);

    //Chip connecto
    tft.fillCircle(10+iconX, 48+iconY,3,bruceConfig.priColor);
    tft.fillCircle(10+iconX, 56+iconY,3,bruceConfig.priColor);
    tft.fillCircle(10+iconX, 64+iconY,3,bruceConfig.priColor);
    tft.fillCircle(10+iconX, 72+iconY,3,bruceConfig.priColor);

    tft.fillCircle(20+iconX, 48+iconY,3,bruceConfig.priColor);
    tft.fillCircle(20+iconX, 56+iconY,3,bruceConfig.priColor);
    tft.fillCircle(20+iconX, 64+iconY,3,bruceConfig.priColor);
    tft.fillCircle(20+iconX, 72+iconY,3,bruceConfig.priColor);

    //Chip
    tft.fillRect(35+iconX,55+iconY,10,10,bruceConfig.priColor);
}