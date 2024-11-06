#include "nrf_common.h"
#include "../../core/mykeyboard.h"

RF24 NRFradio(NRF24_CE_PIN, NRF24_SS_PIN);

void nrf_info() {
  tft.fillScreen(bruceConfig.bgColor);
  tft.setTextSize(FM);
  tft.setTextColor(TFT_RED, bruceConfig.bgColor);
  tft.drawCentreString("_Disclaimer_",WIDTH/2,10,1);
  tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
  tft.setTextSize(FP);
  tft.setCursor(15,33);
  tft.println("These functions were made to be used in a controlled environment for STUDY only.");
  tft.println("\nDO NOT use these functions to harm people or companies, you can go to jail!");
  tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
  tft.println("\nThis device is VERY sensible to noise, so long wires or passing near VCC line can make things go wrong.");
  delay(1000);
  while(!checkAnyKeyPress());
}

bool nrf_start() {
#if defined(USE_NRF24_VIA_SPI)
  pinMode(NRF24_SS_PIN, OUTPUT);
  digitalWrite(NRF24_SS_PIN, HIGH);
  pinMode(NRF24_CE_PIN, OUTPUT);
  digitalWrite(NRF24_CE_PIN, LOW);

  #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
    CC_NRF_SPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #elif defined(CARDPUTER) || defined(ESP32S3DEVKITC1)
    sdcardSPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #elif defined(T_EMBED)
    tft.getSPIinstance().begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #else 
    SPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #endif

  #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
    if(NRFradio.begin(&CC_NRF_SPI))
  #elif defined(CARDPUTER) || defined(ESP32S3DEVKITC1)
    if(NRFradio.begin(&sdcardSPI))
  #else
    if(NRFradio.begin(&SPI))
  #endif
  {
    return true;
  }
  else
  return false;



#else // NRF24 not set in platfrmio.ini
  return false;
#endif
}