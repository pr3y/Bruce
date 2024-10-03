#include "nrf_common.h"

RF24 NRFradio(NRF24_CE_PIN, NRF24_SS_PIN);

bool nrf_start() {
#if defined(USE_NRF24_VIA_SPI)	
  #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
    CC_NRF_SPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #elif defined(CARDPUTER) || defined(ESP32S3DEVKITC1)
    sdcardSPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
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