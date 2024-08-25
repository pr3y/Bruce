#include "ble_jammer.h"
#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/display.h"


/* **************************************************************************************
** name : ble_jammer
** details : Starts 2.4Gz jammer usinf NRF24
************************************************************************************** */
void ble_jammer() {
  #if defined(USE_NRF24_VIA_SPI)
  #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
    CC_NRF_SPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #elif defined(CARDPUTER) || defined(ESP32S3DEVKITC1)
    sdcardSPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #else 
    SPI.begin(NRF24_SCK_PIN,NRF24_MISO_PIN,NRF24_MOSI_PIN,NRF24_SS_PIN);
  #endif
    
    RF24 radio(NRF24_CE_PIN, NRF24_SS_PIN);                                                               ///ce-csn
    byte hopping_channel[] = {32,34, 46,48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80, 82, 84,86 };  // channel to hop
    byte ptr_hop = 0;  // Pointer to the hopping array
  #if defined(STICK_C_PLUS) || defined(STICK_C_PLUS2)
    if(radio.begin(&CC_NRF_SPI))
  #elif defined(CARDPUTER) || defined(ESP32S3DEVKITC1)
    if(radio.begin(&sdcardSPI))
  #else 
    if(radio.begin(&SPI))
  #endif
    {
        Serial.println("NRF24 turned On");
        
        radio.setPALevel(RF24_PA_MAX);
        radio.startConstCarrier(RF24_PA_MAX, 45);
        radio.setAddressWidth(3);//optional
        radio.setPayloadSize(2);//optional
        if(!radio.setDataRate(RF24_2MBPS)) Serial.println("Fail setting data Rate");

        drawMainBorder();
        tft.setCursor(10,28);
        tft.setTextSize(FM);
        tft.println("BLE Jammer:");
        tft.setCursor(10,tft.getCursorY()+8);
        tft.println("Select to stop!");
        delay(200);

        while(!checkSelPress()) {
            ptr_hop++;                                            /// perform next channel change
            if (ptr_hop >= sizeof(hopping_channel)) ptr_hop = 0;  // To avoid array indexing overflow
            radio.setChannel(hopping_channel[ptr_hop]);           // Change channel        
        }
        radio.powerDown();
    } else { 
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
        while(!checkSelPress()); // wait confirmation
    }
  #endif
}