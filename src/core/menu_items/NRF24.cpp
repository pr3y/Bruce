#include "NRF24.h"
#include "core/display.h"
#include "modules/NRF24/nrf_common.h"
#include "modules/NRF24/nrf_jammer.h"
#include "modules/NRF24/nrf_spectrum.h"
#include "core/utils.h"

void NRF24Menu::optionsMenu() {
    options.clear();
    options.push_back({"Information",  [=]() { nrf_info(); }});

  
    if(bruceConfig.NRF24_bus.mosi==bruceConfig.SDCARD_bus.mosi && bruceConfig.NRF24_bus.mosi!=GPIO_NUM_NC) 
              options.push_back({"Spectrum",      [=]() { nrf_spectrum(&sdcardSPI); }});
    #if TFT_MOSI>0 // Display doesn't use SPI bus
    else if(bruceConfig.NRF24_bus.mosi==(gpio_num_t)TFT_MOSI) options.push_back({"Spectrum",      [=]() { nrf_spectrum(&tft.getSPIinstance()); }});
    #endif
    else      options.push_back({"Spectrum",      [=]() { nrf_spectrum(&SPI); }});

    options.push_back({"Jammer 2.4G",  [=]() { nrf_jammer(); }});

    options.push_back({"Main Menu",    [=]() { backToMenu(); }});

    loopOptions(options,false,true,"Bluetooth");
}

void NRF24Menu::drawIcon(float scale) {
    clearIconArea();

    int iconW = scale * 80;
    int iconH = scale * 60;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 2 != 0) iconH++;

    int caseW = 3*iconW/4;
    int caseH = 2*iconH/3;
    int caseX = iconCenterX - iconW/2;
    int caseY = iconCenterY - iconH/6;

    int antW = iconW / 8;
    int connR = iconH / 20;

    // Case
    tft.drawRect(caseX, caseY, caseW, caseH, bruceConfig.priColor);

    // Antenna
    tft.fillRect(
      caseX + caseW, caseY + caseH/2 - antW/2,
      antW, antW,
      bruceConfig.priColor
    );
    tft.fillRoundRect(
      caseX + caseW + antW, caseY + caseH - iconH,
      antW, iconH - caseH/2 + antW/2,
      antW/2,
      bruceConfig.priColor
    );

    // Connectors
    tft.fillCircle(caseX + caseW/6, caseY + 1*caseH/5, connR, bruceConfig.priColor);
    tft.fillCircle(caseX + caseW/6, caseY + 2*caseH/5, connR, bruceConfig.priColor);
    tft.fillCircle(caseX + caseW/6, caseY + 3*caseH/5, connR, bruceConfig.priColor);
    tft.fillCircle(caseX + caseW/6, caseY + 4*caseH/5, connR, bruceConfig.priColor);

    tft.fillCircle(caseX + caseW/3, caseY + 1*caseH/5, connR, bruceConfig.priColor);
    tft.fillCircle(caseX + caseW/3, caseY + 2*caseH/5, connR, bruceConfig.priColor);
    tft.fillCircle(caseX + caseW/3, caseY + 3*caseH/5, connR, bruceConfig.priColor);
    tft.fillCircle(caseX + caseW/3, caseY + 4*caseH/5, connR, bruceConfig.priColor);

    // Chip
    tft.fillRect(
      caseX + caseW - 2*antW - connR,
      caseY + caseH/2 - antW/2,
      antW, antW,
      bruceConfig.priColor
    );
}