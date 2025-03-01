#include "nrf_common.h"
#include "../../core/mykeyboard.h"

RF24 NRFradio(NRF24_CE_PIN, NRF24_SS_PIN);
SPIClass* NRFSPI;

void nrf_info() {
  tft.fillScreen(bruceConfig.bgColor);
  tft.setTextSize(FM);
  tft.setTextColor(TFT_RED, bruceConfig.bgColor);
  tft.drawCentreString("_Disclaimer_",tftWidth/2,10,1);
  tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
  tft.setTextSize(FP);
  tft.setCursor(15,33);
  tft.println("These functions were made to be used in a controlled environment for STUDY only.");
  tft.println("\nDO NOT use these functions to harm people or companies, you can go to jail!");
  tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
  tft.println("\nThis device is VERY sensible to noise, so long wires or passing near VCC line can make things go wrong.");
  delay(1000);
  while(!check(AnyKeyPress));
}

bool nrf_start() {
#if defined(USE_NRF24_VIA_SPI)
  pinMode(bruceConfig.NRF24_bus.cs, OUTPUT);
  digitalWrite(bruceConfig.NRF24_bus.cs, HIGH);
  pinMode(bruceConfig.NRF24_bus.io0, OUTPUT);
  digitalWrite(bruceConfig.NRF24_bus.io0, LOW);
  
  #if defined(SMOOCHIEE_BOARD)
    bool smoochie=true;
  #else
    bool smoochie=false;
  #endif
  
  if(bruceConfig.NRF24_bus.mosi == (gpio_num_t)TFT_MOSI && bruceConfig.NRF24_bus.mosi!=GPIO_NUM_NC) { // (T_EMBED), CORE2 and others
    #if TFT_MOSI>0 // condition for Headless and 8bit displays (no SPI bus)
    NRFSPI = &tft.getSPIinstance(); 
    #else
    NRFSPI = &SPI;
    #endif

  }
  else if(bruceConfig.NRF24_bus.mosi==bruceConfig.SDCARD_bus.mosi) { // CC1101 shares SPI with SDCard (Cardputer and CYDs)
    Serial.println("Using this one!!!!!!!!!!!! --    --------- ------------ ------------ ---------- --------- ----");
    NRFSPI = &sdcardSPI;
  }
  else if(smoochie) { // Smoochie board shares CC1101 and NRF24 SPI bus with different CS pins at the same time, different from StickCs that uses the same Bus, but one at a time (same CS Pin)
    NRFSPI = &CC_NRF_SPI;
  } else {  
    NRFSPI = &SPI;
  }
  NRFSPI->begin((int8_t)bruceConfig.NRF24_bus.sck, (int8_t)bruceConfig.NRF24_bus.miso, (int8_t)bruceConfig.NRF24_bus.mosi);
  delay(10);

  if(NRFradio.begin(NRFSPI,rf24_gpio_pin_t(bruceConfig.NRF24_bus.io0),rf24_gpio_pin_t(bruceConfig.NRF24_bus.cs)))
  {
    return true;
  }
  else
  return false;



#else // NRF24 not set in platfrmio.ini
  return false;
#endif
}