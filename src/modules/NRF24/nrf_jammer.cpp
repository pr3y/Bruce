#include "nrf_common.h"
#include "nrf_jammer.h"
#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/display.h"

/* **************************************************************************************
** name : nrf_jammer
** details : Starts 2.4Gz jammer usinf NRF24
************************************************************************************** */
void nrf_jammer() {
  #if defined(NRF24_CE_PIN) && defined(NRF24_SS_PIN) && defined(USE_NRF24_VIA_SPI)
    RF24 radio(NRF24_CE_PIN, NRF24_SS_PIN);                                                               ///ce-csn
    byte hopping_channel[] = {32,34, 46,48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80, 82, 84,86 };  // channel to hop
    byte ptr_hop = 0;  // Pointer to the hopping array
    if(nrf_start())
    {
        Serial.println("NRF24 turned On");
        
        NRFradio.setPALevel(RF24_PA_MAX);
        NRFradio.startConstCarrier(RF24_PA_MAX, 45);
        NRFradio.setAddressWidth(3);//optional
        NRFradio.setPayloadSize(2);//optional
        if(!NRFradio.setDataRate(RF24_2MBPS)) Serial.println("Fail setting data Rate");

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
            NRFradio.setChannel(hopping_channel[ptr_hop]);           // Change channel        
        }
        NRFradio.powerDown();
    } else { 
        Serial.println("Fail Starting radio");
        displayError("NRF24 not found");
        delay(500);
    }
  #endif
}