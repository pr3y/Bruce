#include "nrf_common.h"
#include "nrf_spectrum.h"
#include "../../core/display.h"
#include "../../core/mykeyboard.h"


#define CHANNELS 80
#define RGB565(r, g, b) ((((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)))
uint8_t channel[CHANNELS];

// Register Access Functions
inline byte getRegister(SPIClass &SSPI, byte r) {
  digitalWrite(NRF24_SS_PIN, LOW);
  byte c = SSPI.transfer(r & 0x1F);
  c = SSPI.transfer(0);
  digitalWrite(NRF24_SS_PIN, HIGH);
  return c;
}

inline void setRegister(SPIClass &SSPI, byte r, byte v) {
  digitalWrite(NRF24_SS_PIN, LOW);
  SSPI.transfer((r & 0x1F) | 0x20);
  SSPI.transfer(v);
  digitalWrite(NRF24_SS_PIN, HIGH);
}

inline void powerDown(SPIClass &SSPI) {
  setRegister(SSPI,0x00, getRegister(SSPI,0x00) & ~0x02);
}

// Scanning Channels
#define _BW WIDTH/CHANNELS
String scanChannels(SPIClass* SSPI, bool web) {
  String result="{";
  digitalWrite(NRF24_CE_PIN, LOW);
  for (int i = 0; i < CHANNELS; i++) {
    NRFradio.setChannel(i);
    NRFradio.startListening();
    delayMicroseconds(128);
    NRFradio.stopListening();
    int rpd=0;
    if(NRFradio.testCarrier()) rpd=200;
    channel[i] = (channel[i] * 3 + rpd) / 4;

    int level = (channel[i] > 125) ? 125 : channel[i];  // Clamp values

    tft.drawFastVLine(i*_BW, 0, 125, (i % 8) ? TFT_BLACK : RGB565(25, 25, 25));
    tft.drawFastVLine(i*_BW, HEIGHT-(10+level), level, (i % 2 == 0) ? bruceConfig.priColor : TFT_DARKGREY); // Use green for even indices
    tft.drawFastVLine(i*_BW, 0, HEIGHT-(9+level), (i % 8) ? TFT_BLACK : RGB565(25, 25, 25));
    tft.drawFastVLine(i*_BW, 0, rpd ? 2 : 0, TFT_DARKGREY);
    if(web) {
      if(i>0) result+=",";
      result+=String(level);
    }
  }
  if(web) result+="}";
  return result; // return a string in this format "{1,32,45,32,84,32 .... 12,54,65}" with 80 values to be used in the WebUI (Future)
}


void nrf_spectrum(SPIClass* SSPI) {
  tft.fillScreen(bruceConfig.bgColor);
  tft.setTextSize(FP);
  tft.drawString("2.40Ghz",0,HEIGHT-LH);
  tft.drawCentreString("2.44Ghz", WIDTH/2,HEIGHT-LH,1);
  tft.drawRightString("2.48Ghz",WIDTH,HEIGHT-LH,1);
  memset(channel,0,CHANNELS);

  if(nrf_start()) {
    NRFradio.setAutoAck(false);
    NRFradio.disableCRC();        // accept any signal we find
    NRFradio.setAddressWidth(2);  // a reverse engineering tactic (not typically recommended)
    const uint8_t noiseAddress[][2] = { { 0x55, 0x55 }, { 0xAA, 0xAA }, { 0xA0, 0xAA }, { 0xAB, 0xAA }, { 0xAC, 0xAA }, { 0xAD, 0xAA } };
    for (uint8_t i = 0; i < 6; ++i) {
      NRFradio.openReadingPipe(i, noiseAddress[i]);
    }
    NRFradio.setDataRate(RF24_1MBPS);

    while(!checkEscPress()) {
      scanChannels(SSPI);
    }
    NRFradio.stopListening();
    powerDown(*SSPI); //
    delay(250);
    return;

  }
  else {
      Serial.println("Fail Starting radio");
      displayError("NRF24 not found");
      delay(500);
      return;
  }
}
