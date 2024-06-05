//@IncursioHack - github.com/IncursioHack

#include "rf.h"
#include "globals.h"
#include "mykeyboard.h"
#include "display.h"
#include "PCA9554.h"
#include "sd_functions.h"
#include <driver/rmt.h>

// Cria um objeto PCA9554 com o endereço I2C do PCA9554PW
// PCA9554 extIo1(pca9554pw_address);

#define RMT_RX_CHANNEL  RMT_CHANNEL_6
#define RMT_BLOCK_NUM   1

#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)

#define SIGNAL_STRENGTH_THRESHOLD 1500 // Adjust this threshold as needed

#define DISPLAY_HEIGHT 130 // Height of the display area for the waveform
#define DISPLAY_WIDTH  240 // Width of the display area
#define LINE_WIDTH 2 // Adjust line width as needed

void initRMT() {
    rmt_config_t rxconfig;
    rxconfig.rmt_mode            = RMT_MODE_RX;
    rxconfig.channel             = RMT_RX_CHANNEL;
    rxconfig.gpio_num            = gpio_num_t(RfRx);
    rxconfig.mem_block_num       = RMT_BLOCK_NUM;
    rxconfig.clk_div             = RMT_CLK_DIV;
    rxconfig.rx_config.filter_en = true;
    rxconfig.rx_config.filter_ticks_thresh = 200 * RMT_1US_TICKS;
    rxconfig.rx_config.idle_threshold = 3 * RMT_1MS_TICKS;

    ESP_ERROR_CHECK(rmt_config(&rxconfig));
    ESP_ERROR_CHECK(rmt_driver_install(rxconfig.channel, 2048, 0));
}

bool sendRF = false;

void rf_spectrum() { //@IncursioHack - https://github.com/IncursioHack ----thanks @aat440hz - RF433ANY-M5Cardputer

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.println("");    
    tft.println("  RF433 - Spectrum");
    pinMode(RfRx, INPUT);
    initRMT();

        RingbufHandle_t rb = nullptr;
        rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
        rmt_rx_start(RMT_RX_CHANNEL, true);
        while (rb) {
            size_t rx_size = 0;
            rmt_item32_t* item = (rmt_item32_t*)xRingbufferReceive(rb, &rx_size, 500);
            if (item != nullptr) {
                if (rx_size != 0) {
                    // Clear the display area
                    tft.fillRect(0, 20, DISPLAY_WIDTH, DISPLAY_HEIGHT, TFT_BLACK);
                    // Draw waveform based on signal strength
                    for (size_t i = 0; i < rx_size; i++) {
                        int lineHeight = map(item[i].duration0 + item[i].duration1, 0, SIGNAL_STRENGTH_THRESHOLD, 0, DISPLAY_HEIGHT/2);
                        int lineX = map(i, 0, rx_size - 1, 0, DISPLAY_WIDTH - 1); // Map i to within the display width
                        // Ensure drawing coordinates stay within the box bounds
                        int startY = constrain(20 + DISPLAY_HEIGHT / 2 - lineHeight / 2, 20, 20 + DISPLAY_HEIGHT);
                        int endY = constrain(20 + DISPLAY_HEIGHT / 2 + lineHeight / 2, 20, 20 + DISPLAY_HEIGHT);
                        tft.drawLine(lineX, startY, lineX, endY, TFT_PURPLE);
                    }
                }
                vRingbufferReturnItem(rb, (void*)item);
            }
                    if (checkEscPress()) {
                    rmt_rx_stop(RMT_RX_CHANNEL);
                    returnToMenu=true;
                    break;                       
                }
        }
            // Checks para sair do while
        
    rmt_rx_stop(RMT_RX_CHANNEL);
    delay(10);



}


void rf_jammerFull() { //@IncursioHack - https://github.com/IncursioHack -  thanks @EversonPereira - rfcardputer
    pinMode(RfTx, OUTPUT);
    tft.fillScreen(TFT_BLACK);
    tft.println("");    
    tft.println("  RF433 - Jammer Full");
    tft.println(""); 
    tft.println("");         
    tft.setTextSize(2);
    sendRF = true;
    digitalWrite(RfTx, HIGH); // Turn on Jammer
    int tmr0=millis();             // control total jammer time;
    tft.println("Sending... Press ESC to stop.");
    while (sendRF) {
        if (checkEscPress() || (millis() - tmr0 >20000)) {
            sendRF = false;
            returnToMenu=true;
            break;                       
        }
    }
    digitalWrite(RfTx, LOW); // Turn Jammer OFF
}


void rf_jammerIntermittent() { //@IncursioHack - https://github.com/IncursioHack -  thanks @EversonPereira - rfcardputer
    pinMode(RfTx, OUTPUT);
    tft.fillScreen(TFT_BLACK);
    tft.println("");    
    tft.println("  RF433 - Jammer Intermittent");
    tft.println("");
    tft.println("");         
    tft.setTextSize(2);
    sendRF = true;
    tft.println("Sending... Press ESC to stop.");
    int tmr0 = millis();
    while (sendRF) {
        for (int sequence = 1; sequence < 50; sequence++) {
            for (int duration = 1; duration <= 3; duration++) {
                // Moved Escape check into this loop to check every cycle
                if (checkEscPress() || (millis()-tmr0)>20000) {
                    sendRF = false;
                    returnToMenu=true;
                    break;
                }
                digitalWrite(RfTx, HIGH); // Ativa o pino
                // Mantém o pino ativo por um período que aumenta com cada sequência
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(50);
                }

                digitalWrite(RfTx, LOW); // Desativa o pino
                // Mantém o pino inativo pelo mesmo período
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(50);
                }
            }
        }    
    }
    
    digitalWrite(RfTx, LOW); // Desativa o pino
}
