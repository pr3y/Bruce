//@IncursioHack - github.com/IncursioHack

#include <driver/rmt.h>
#include <RCSwitch.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
//#include "PCA9554.h"
#include <globals.h>
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "core/settings.h"
#include "rf.h"
#include <iostream>
#include <string>
#include <sstream>

// Cria um objeto PCA9554 com o endereço I2C do PCA9554PW
// PCA9554 extIo1(pca9554pw_address);

#define RMT_RX_CHANNEL  RMT_CHANNEL_6
#define RMT_BLOCK_NUM


#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)

#define SIGNAL_STRENGTH_THRESHOLD 1500 // Adjust this threshold as needed

#define LINE_WIDTH 2 // Adjust line width as needed

struct HighLow {
    uint8_t high; // 1
    uint8_t low;  //31
};


struct Protocol {
    uint16_t pulseLength;  // base pulse length in microseconds, e.g. 350
    HighLow syncFactor;
    HighLow zero;
    HighLow one;
    bool invertedSignal;
};


// Global to magane rmt installation.. if it is installed twice, it breakes
bool RxRF = false;
bool sendRF = false;

RfCodes recent_rfcodes[16];  // TODO: save/load in EEPROM
int recent_rfcodes_last_used = 0;  // TODO: save/load in EEPROM


void initRMT() {
    rmt_config_t rxconfig;
    rxconfig.rmt_mode            = RMT_MODE_RX;
    rxconfig.channel             = RMT_RX_CHANNEL;
    rxconfig.gpio_num            = gpio_num_t(bruceConfig.rfRx);
    #ifdef USE_CC1101_VIA_SPI
    if(bruceConfig.rfModule==CC1101_SPI_MODULE)
        rxconfig.gpio_num            = gpio_num_t(CC1101_GDO0_PIN);
    #endif
    rxconfig.clk_div             = RMT_CLK_DIV; // RMT_DEFAULT_CLK_DIV=32
    rxconfig.mem_block_num       = 1;
    rxconfig.flags               = 0;
    rxconfig.rx_config.idle_threshold = 3 * RMT_1MS_TICKS,
    rxconfig.rx_config.filter_ticks_thresh = 200 * RMT_1US_TICKS;
    rxconfig.rx_config.filter_en = true;
    if(!RxRF) { //If spectrum had beed started before, it won't reinstall the driver to prevent mem alloc fail and restart.
        ESP_ERROR_CHECK(rmt_config(&rxconfig));
        ESP_ERROR_CHECK(rmt_driver_install(rxconfig.channel, 2048, 0));
        RxRF=true;
    }

}


void rf_spectrum() { //@IncursioHack - https://github.com/IncursioHack ----thanks @aat440hz - RF433ANY-M5Cardputer

    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.println("");
    tft.println("  RF - Spectrum");
    if(!initRfModule("rx", bruceConfig.rfFreq)) return;
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
                tft.fillRect(0, 20, tftWidth, tftHeight, bruceConfig.bgColor);
                // Draw waveform based on signal strength
                for (size_t i = 0; i < rx_size; i++) {
                    int lineHeight = map(item[i].duration0 + item[i].duration1, 0, SIGNAL_STRENGTH_THRESHOLD, 0, tftHeight/2);
                    int lineX = map(i, 0, rx_size - 1, 0, tftWidth - 1); // Map i to within the display width
                    // Ensure drawing coordinates stay within the box bounds
                    int startY = constrain(20 + tftHeight / 2 - lineHeight / 2, 20, 20 + tftHeight);
                    int endY = constrain(20 + tftHeight / 2 + lineHeight / 2, 20, 20 + tftHeight);
                    tft.drawLine(lineX, startY, lineX, endY, bruceConfig.priColor);
                }
            }
            vRingbufferReturnItem(rb, (void*)item);
        }
        // Checks to leave while
        if (check(EscPress)) {
            break;
        }
    }
    returnToMenu=true;
    rmt_rx_stop(RMT_RX_CHANNEL);
    delay(10);
}


void rf_SquareWave() { //@Pirata

    RCSwitch rcswitch;
    if(!initRfModule("rx", bruceConfig.rfFreq)) return;
    #if defined(USE_CC1101_VIA_SPI)
    if(bruceConfig.rfModule==CC1101_SPI_MODULE)
        rcswitch.enableReceive(CC1101_GDO0_PIN);
    else
    #endif
        rcswitch.enableReceive(bruceConfig.rfRx);

    tft.drawPixel(0,0,0);
    tft.fillScreen(bruceConfig.bgColor);
    tft.setTextSize(1);
    tft.println("");
    tft.setCursor(3,2);
    tft.println("  RF - SquareWave");
    int line_w=0;
    int line_h=15;
    unsigned int* raw;
    while (1) {
        if (rcswitch.RAWavailable()) {
                raw=rcswitch.getRAWReceivedRawdata();
                // Clear the display area
                // tft.fillRect(0, 0, tftWidth, tftHeight, bruceConfig.bgColor);
                // Draw waveform based on signal strength
                for (int i = 0; i < RCSWITCH_RAW_MAX_CHANGES-1; i+=2) {
                    if(raw[i]==0) break;
                    #define TIME_DIVIDER tftWidth/30
                    if(raw[i]>20000) raw[i]=20000;
                    if(raw[i+1]>20000) raw[i+1]=20000;
                    if(line_w+(raw[i]+raw[i+1])/TIME_DIVIDER>tftWidth) { line_w=10; line_h+=10; }
                    if(line_h>tftHeight) {
                        line_h = 15;
                        tft.fillRect(0, 12, tftWidth, tftHeight, bruceConfig.bgColor);
                    }
                    tft.drawFastVLine(line_w                    ,line_h     ,6                      ,bruceConfig.priColor);
                    tft.drawFastHLine(line_w                    ,line_h     ,raw[i]/TIME_DIVIDER    ,bruceConfig.priColor);

                    tft.drawFastVLine(line_w+raw[i]/TIME_DIVIDER,line_h     ,6                      ,bruceConfig.priColor);
                    tft.drawFastHLine(line_w+raw[i]/TIME_DIVIDER,line_h+6   ,raw[i+1]/TIME_DIVIDER  ,bruceConfig.priColor);
                    line_w+=(raw[i] + raw[i+1])/TIME_DIVIDER;
                }
            rcswitch.resetAvailable();
        }
        // Checks to leave while
        if (check(EscPress)) {
            break;
        }
    }
    returnToMenu=true;
    rmt_rx_stop(RMT_RX_CHANNEL);
    delay(10);
}

void setMHZ(float frequency) {
    #ifdef USE_CC1101_VIA_SPI
        if(frequency>928 || frequency < 300)  {
            frequency = 433.92;
            Serial.println("Frequency out of band");
        }
        #if defined(T_EMBED_1101)
            static uint8_t antenna=200; // 0=(<300), 1=(350-468), 2=(>778), 200=start to settle at the fisrt time
            // SW1:1  SW0:0 --- 315MHz
            // SW1:0  SW0:1 --- 868/915MHz
            // SW1:1  SW0:1 --- 434MHz
            if (frequency <= 350 && antenna!=0)
            {
                digitalWrite(CC1101_SW1_PIN, HIGH);
                digitalWrite(CC1101_SW0_PIN, LOW);
                antenna=0;
                delay(10); // time to settle the antenna signal
            }
            else if (frequency > 350 && frequency < 468 && antenna!=1)
            {
                digitalWrite(CC1101_SW1_PIN, HIGH);
                digitalWrite(CC1101_SW0_PIN, HIGH);
                antenna=1;
                delay(10); // time to settle the antenna signal
            }
            else if (frequency > 778 && antenna!=2)
            {
                digitalWrite(CC1101_SW1_PIN, LOW);
                digitalWrite(CC1101_SW0_PIN, HIGH);
                antenna=2;
                delay(10); // time to settle the antenna signal
            }

        #endif
        ELECHOUSE_cc1101.setMHZ(frequency);
    #endif
}

void rf_jammerFull() { //@IncursioHack - https://github.com/IncursioHack -  thanks @EversonPereira - rfcardputer
    // init rf module
    int nTransmitterPin = bruceConfig.rfTx;
    if(!initRfModule("tx")) return;
    if(bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        #ifdef USE_CC1101_VIA_SPI
            nTransmitterPin = CC1101_GDO0_PIN;
        #else
            return;
        #endif
    }

    tft.fillScreen(TFT_BLACK);
    drawMainBorder();
    tft.setCursor(10,30);
    tft.setTextSize(FP);
    padprintln("RF - Jammer Full");
    tft.println("");
    tft.println("");
    tft.setTextSize(FP);
    sendRF = true;
    digitalWrite(nTransmitterPin, HIGH); // Turn on Jammer
    int tmr0=millis();             // control total jammer time;
    padprintln("Sending... Press ESC to stop.");
    while (sendRF) {
        if (check(EscPress) || (millis() - tmr0 >20000)) {
            sendRF = false;
            returnToMenu=true;
            break;
        }
    }
    deinitRfModule(); // Turn Jammer OFF
}


void rf_jammerIntermittent() { //@IncursioHack - https://github.com/IncursioHack -  thanks @EversonPereira - rfcardputer
    int nTransmitterPin = bruceConfig.rfTx;
    if(!initRfModule("tx")) return;
    if(bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        #ifdef USE_CC1101_VIA_SPI
            nTransmitterPin = CC1101_GDO0_PIN;
        #else
            return;
        #endif
    }
    tft.fillScreen(TFT_BLACK);
    drawMainBorder();
    tft.setCursor(10,30);
    tft.setTextSize(FP);
    padprintln("RF - Jammer Intermittent");
    tft.println("");
    tft.println("");
    sendRF = true;
    padprintln("Sending... Press ESC to stop.");
    int tmr0 = millis();
    while (sendRF) {
        for (int sequence = 1; sequence < 50; sequence++) {
            for (int duration = 1; duration <= 3; duration++) {
                // Moved Escape check into this loop to check every cycle
                if (check(EscPress) || (millis()-tmr0)>20000) {
                    sendRF = false;
                    returnToMenu=true;
                    break;
                }
                digitalWrite(nTransmitterPin, HIGH); // Ativa o pino
                // keeps the pin active for a while and increase increase
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(10);
                }

                digitalWrite(nTransmitterPin, LOW); // Desativa o pino
                // keeps the pin inactive for the same time as before
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(10);
                }
            }
        }
    }

    deinitRfModule();
}

String rf_scan(float start_freq, float stop_freq, int max_loops)
{
    // derived from https://github.com/mcore1976/cc1101-tool/blob/main/cc1101-tool-esp32.ino#L480

    if(bruceConfig.rfModule != CC1101_SPI_MODULE) {
        displayError("rf scanning is available with CC1101 only", true);
        return ""; // only CC1101 is supported for this
    }
    if(!initRfModule("rx", start_freq)) return "";

    ELECHOUSE_cc1101.setRxBW(256);

    float settingf1 = start_freq;
    float settingf2 = stop_freq;
    float freq;
    long compare_freq;
    float mark_freq;
    int rssi;
    int mark_rssi=-100;
    String out="";

    while(max_loops || !check(EscPress)) {
        delay(1);
        max_loops -= 1;

        setMHZ(freq);

        rssi = ELECHOUSE_cc1101.getRssi();
        if (rssi>-75)
           {
                if (rssi > mark_rssi)
                {
                      mark_rssi = rssi;
                      mark_freq = freq;
                };
          };

       freq+=0.01;

       if (freq > settingf2)
          {
               freq = settingf1;

               if (mark_rssi>-75)
                {
                  long fr = mark_freq*100;
                  if (fr == compare_freq)
                      {
                        Serial.print(F("\r\nSignal found at  "));
                        Serial.print(F("Freq: "));
                        Serial.print(mark_freq);
                        Serial.print(F(" Rssi: "));
                        Serial.println(mark_rssi);
                        mark_rssi=-100;
                        compare_freq = 0;
                        mark_freq = 0;
                        out += String(mark_freq) + ",";
                      }
                  else
                      {
                        compare_freq = mark_freq*100;
                        freq = mark_freq -0.10;
                        mark_freq=0;
                        mark_rssi=-100;
                      };
                };
          }; // end of IF freq>stop frequency
      };  // End of While

    deinitRfModule();
    return out;
}

void RCSwitch_send(uint64_t data, unsigned int bits, int pulse, int protocol, int repeat)
{
    // derived from https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/examples/Rc-Switch%20examples%20cc1101/SendDemo_cc1101/SendDemo_cc1101.ino

    RCSwitch mySwitch = RCSwitch();

    if(bruceConfig.rfModule==CC1101_SPI_MODULE) {
        #ifdef USE_CC1101_VIA_SPI
            mySwitch.enableTransmit(CC1101_GDO0_PIN);
        #else
            Serial.println("USE_CC1101_VIA_SPI not defined");
            return;  // not enabled for this board
        #endif
    } else {
        mySwitch.enableTransmit(bruceConfig.rfTx);
    }

    mySwitch.setProtocol(protocol);  // override
    if (pulse) { mySwitch.setPulseLength(pulse); }
    mySwitch.setRepeatTransmit(repeat);
    mySwitch.send(data, bits);

    /*
    Serial.println(data,HEX);
    Serial.println(bits);
    Serial.println(pulse);
    Serial.println(protocol);
    Serial.println(repeat);
    */

    mySwitch.disableTransmit();

    deinitRfModule();
}


// Example from https://github.com/sui77/rc-switch/blob/master/examples/ReceiveDemo_Advanced/output.ino

// Converts a Hex char to decimal
uint8_t hexCharToDecimal(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

// converts a Hex string like "11 22 AE FF"  to decimal
uint32_t hexStringToDecimal(const char* hexString) {
    uint32_t decimal = 0;
    int length = strlen(hexString);

    for (int i = 0; i < length; i += 3) {
        decimal <<= 8; // Shift left to accommodate next byte

        // Converts two characters hex to a single byte
        uint8_t highNibble = hexCharToDecimal(hexString[i]);
        uint8_t lowNibble = hexCharToDecimal(hexString[i + 1]);
        decimal |= (highNibble << 4) | lowNibble;
    }

    return decimal;
}

void decimalToHexString(uint64_t decimal, char* output) {
    char hexDigits[] = "0123456789ABCDEF";
    char temp[65];
    int index = 15;

    // Initialize tem string with zeros
    for (int i = 0; i < 64; i++) {
        temp[i] = '0';
    }
    temp[65] = '\0';

    // Convert decimal to hexadecimal
    while (decimal > 0) {
        temp[index--] = hexDigits[decimal % 16];
        decimal /= 16;
    }

    // Format string with spaces
    int outputIndex = 0;
    for (int i = 0; i < 16; i++) {
        output[outputIndex++] = temp[i];
        if ((i % 2) == 1 && i != 15) {
            output[outputIndex++] = ' ';
        }
    }
    output[outputIndex] = '\0';
}

String hexStrToBinStr(const String& hexStr) {
    String binStr = "";
    String hexByte = "";

    // Variables for decimal value
    int value;

    for (int i = 0; i < hexStr.length(); i++) {
        char c = hexStr.charAt(i);

        // Check if the character is a hexadecimal digit
        if (c >= '0' && c <= '9' || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f') {
            hexByte += c;
            if (hexByte.length() == 2) {
                // Convert the hexadecimal pair to a decimal value
                value = strtol(hexByte.c_str(), NULL, 16);

                // Convert the decimal value to binary and add to the binary string
                for (int j = 7; j >= 0; j--) {
                    binStr += (value & (1 << j)) ? '1' : '0';
                }
                //binStr += ' ';

                // Clear the hexByte string for the next byte
                hexByte = "";
            }
        }
    }

    // Remove the extra trailing space, if any
    if (binStr.length() > 0 && binStr.charAt(binStr.length() - 1) == ' ') {
        binStr.remove(binStr.length() - 1);
    }

    return binStr;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';

  return bin;
}

void initCC1101once(SPIClass* SSPI) {
    // the init (); command may only be executed once in the entire program sequence. Otherwise problems can arise.  https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/issues/65

    #ifdef USE_CC1101_VIA_SPI
        // derived from https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/examples/Rc-Switch%20examples%20cc1101/ReceiveDemo_Advanced_cc1101/ReceiveDemo_Advanced_cc1101.ino
        if(SSPI!=NULL) ELECHOUSE_cc1101.setSPIinstance(SSPI); // New, to use the SPI instance we want.
        ELECHOUSE_cc1101.setSpiPin(CC1101_SCK_PIN, CC1101_MISO_PIN, CC1101_MOSI_PIN, CC1101_SS_PIN);
        #ifdef CC1101_GDO2_PIN
            ELECHOUSE_cc1101.setGDO(CC1101_GDO0_PIN, CC1101_GDO2_PIN); 	//Set Gdo0 (tx) and Gdo2 (rx) for serial transmission function.
        #else
            ELECHOUSE_cc1101.setGDO0(CC1101_GDO0_PIN);  // use Gdo0 for both Tx and Rx
        #endif
        /*
        Don't need to start comunications now
        if (ELECHOUSE_cc1101.getCC1101()){       // Check the CC1101 Spi connection.
            Serial.println("cc1101 Connection OK");
        } else {
            Serial.println("cc1101 Connection Error");
            return;
        }
        ELECHOUSE_cc1101.Init();
        */

    #else
        Serial.println("Error: USE_CC1101_VIA_SPI not defined for this board");
        //TODO: interface using PCA9554
    #endif
    return;
}

void deinitRfModule() {
    if(bruceConfig.rfModule==CC1101_SPI_MODULE)
        #ifdef USE_CC1101_VIA_SPI
            #if CC1101_MOSI_PIN==TFT_MOSI || CC1101_MOSI_PIN==SDCARD_MOSI // (T_EMBED), CORE2 and others
                ELECHOUSE_cc1101.setSidle();
            #else // (STICK_C_PLUS) || (STICK_C_PLUS2) and others that doesn´t share SPI with other devices (need to change it when Bruce board comes to shore)
                ELECHOUSE_cc1101.getSPIinstance()->end();
            #endif
        #else
            return;
        #endif
    else

        digitalWrite(bruceConfig.rfTx, LED_OFF);

}

bool initRfModule(String mode, float frequency) {
    #if CC1101_MOSI_PIN==TFT_MOSI // (T_EMBED), CORE2 and others
        initCC1101once(&tft.getSPIinstance());
    #elif CC1101_MOSI_PIN==SDCARD_MOSI // (CARDPUTER) and (ESP32S3DEVKITC1) and devices that share CC1101 pin with only SDCard
        ELECHOUSE_cc1101.setSPIinstance(&sdcardSPI);
    #elif defined(SMOOCHIEE_BOARD) 	// This board uses the same Bus for NRF and CC1101, but with different CS pins, different from Stick_Cs down below.. 
					// It will be like that until we fin a better solution or other board come with a setup like that.
	ELECHOUSE_cc1101.setSPIinstance(&CC_NRF_SPI);
    #else // (STICK_C_PLUS) || (STICK_C_PLUS2) and others that doesn´t share SPI with other devices (need to change it when Bruce board comes to shore)
        ELECHOUSE_cc1101.setBeginEndLogic(true); // make sure to use BeginEndLogic for StickCs in the shared pins (not bus) config
        initCC1101once(NULL);
    #endif

    // use default frequency if no one is passed
    if(!frequency) frequency = bruceConfig.rfFreq;

    if(bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        #ifdef USE_CC1101_VIA_SPI
            ELECHOUSE_cc1101.Init();
            if (ELECHOUSE_cc1101.getCC1101()){       // Check the CC1101 Spi connection.
                Serial.println("cc1101 Connection OK");
            } else {
                displayError("CC1101 not found");
                Serial.println("cc1101 Connection Error");
                return false;
            }

            // make sure it is in idle state when changing frequency and other parameters
            // "If any frequency programming register is altered when the frequency synthesizer is running, the synthesizer may give an undesired response. Hence, the frequency programming should only be updated when the radio is in the IDLE state." https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/issues/65
            // ELECHOUSE_cc1101.setSidle();
            // Serial.println("cc1101 setSidle();");

            if(!(   (frequency>=300 && frequency<=350) ||
                    (frequency>=387 && frequency<=468) ||
                    (frequency>=779 && frequency<=928))) {
                        Serial.println("Invalid Frequency, setting default");
                        frequency=433.92;
                        displayWarning("Wrong freq, setted 433.92",true);
                    }
            // else
            //ELECHOUSE_cc1101.setRxBW(812.50);  // reset to default
            ELECHOUSE_cc1101.setRxBW(256);      // narrow band for better accuracy
            ELECHOUSE_cc1101.setClb(1,13,15);   // Calibration Offset
            ELECHOUSE_cc1101.setClb(2,16,19);   // Calibration Offset
            ELECHOUSE_cc1101.setModulation(2);  // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
            ELECHOUSE_cc1101.setDRate(50);     // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
            ELECHOUSE_cc1101.setPktFormat(3);   // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX.
                                                // 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
                                                // 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX.
                                                // 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
            setMHZ(frequency);
            Serial.println("cc1101 setMHZ(frequency);");

            /* MEMO: cannot change other params after this is executed */
            if(mode=="tx") {
                pinMode(CC1101_GDO0_PIN, OUTPUT);
                ELECHOUSE_cc1101.setPA(12);       // set TxPower. The following settings are possible depending
                Serial.println("cc1101 setPA();");
                ELECHOUSE_cc1101.SetTx();
                Serial.println("cc1101 SetTx();");
            }
            else if(mode=="rx") {
                pinMode(CC1101_GDO0_PIN, INPUT);
                ELECHOUSE_cc1101.SetRx();
                Serial.println("cc1101 SetRx();");
            }
            // else if mode is unspecified wont start TX/RX mode here -> done by the caller

        #else
            // TODO: PCA9554-based implmentation
            return false;
        #endif

    } else {
        // single-pinned module
        if(frequency!=bruceConfig.rfFreq) {
            Serial.println("unsupported frequency");
            return false;
        }

        if(mode=="tx") {
            gsetRfTxPin(false);
            pinMode(bruceConfig.rfTx, OUTPUT);
            digitalWrite(bruceConfig.rfTx, LOW);

        }
        else if(mode=="rx") {
            // Rx Mode
            gsetRfRxPin(false);
            pinMode(bruceConfig.rfRx, INPUT);
        }
    }
    // no error
    return true;
}

String RCSwitch_Read(float frequency, int max_loops, bool raw) {
    RCSwitch rcswitch = RCSwitch();
    RfCodes received;

    if(!frequency) frequency = bruceConfig.rfFreq; // default from config

    char hexString[64];

RestartRec:
    drawMainBorder();
    tft.setCursor(10, 28);
    tft.setTextSize(FP);
    tft.println("Waiting for a " + String(frequency) + " MHz " + "signal.");

    // init receive
    if(!initRfModule("rx", frequency)) return "";
    if(bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        #ifdef USE_CC1101_VIA_SPI
            #ifdef CC1101_GDO2_PIN
                rcswitch.enableReceive(CC1101_GDO2_PIN);
            #else
                rcswitch.enableReceive(CC1101_GDO0_PIN);
            #endif
            Serial.println("CC1101 enableReceive()");
        #else
            return "";
        #endif
    } else {
        rcswitch.enableReceive(bruceConfig.rfRx);
    }
    while(!check(EscPress)) {
        if(rcswitch.available()) {
            //Serial.println("Available");
            long value = rcswitch.getReceivedValue();
            //Serial.println("getReceivedValue()");
            if(value) {
                //Serial.println("has value");
                unsigned int* _raw = rcswitch.getReceivedRawdata();
                received.frequency=long(frequency*1000000);
                received.key=rcswitch.getReceivedValue();
                received.protocol="RcSwitch";
                received.preset=rcswitch.getReceivedProtocol();
                received.te=rcswitch.getReceivedDelay();
                received.Bit=rcswitch.getReceivedBitlength();
                received.filepath="unsaved";
                //Serial.println(received.te*2);
                // derived from https://github.com/sui77/rc-switch/tree/master/examples/ReceiveDemo_Advanced
                received.data="";
                int sign = +1;
                //if(received.preset.invertedSignal) sign = -1;
                for(int i=0; i<received.Bit*2; i++) {
                    if(i>0) received.data+=" ";
                    if(i % 2 == 0) sign = +1;
                        else sign = -1;
                    received.data += String(sign * (int)_raw[i]);
                }
                //Serial.println(received.protocol);
                //Serial.println(received.data);
                decimalToHexString(received.key,hexString);

                rf_scan_copy_draw_signal(received, 1, raw);
            }
            rcswitch.resetAvailable();
        }
        if(raw && rcswitch.RAWavailable()) {
            // if no value were decoded, show raw data to be saved
            delay(100); //give it time to process and store all signal

            unsigned int* _raw = rcswitch.getRAWReceivedRawdata();
            int transitions = 0;
            signed int sign=1;
            for(transitions=0; transitions<RCSWITCH_RAW_MAX_CHANGES; transitions++) {
                if(_raw[transitions]==0) break;
                if(transitions>0) received.data+=" ";
                if(transitions % 2 == 0) sign = +1;
                    else sign = -1;
                received.data += String(sign * (int)_raw[transitions]);
            }
            if(transitions>20) {
                received.frequency = long(frequency*1000000);
                received.protocol = "RAW";
                received.preset = "0"; // ????
                received.filepath = "unsaved";
                received.data = "";

                rf_scan_copy_draw_signal(received, 1, raw);
            }
            //ResetSignal:
            rcswitch.resetAvailable();
        }

        if(received.key>0 || received.data.length()>20) { // RAW data does not have "key", 20 is more than 5 transitions
            #ifndef HAS_SCREEN
                // switch to raw mode if decoding failed
                if(received.preset == 0) {
                    Serial.println("signal decoding failed, switching to RAW mode");
                    //displayWarning("signal decoding failed, switching to RAW mode", true);
                    raw = true;
                    // TODO: show a dialog/warning?
                    // raw = yesNoDialog("decoding failed, save as RAW?");
                }
                String subfile_out = "Filetype: Bruce SubGhz File\nVersion 1\n";
                subfile_out += "Frequency: " + String(int(frequency*1000000)) + "\n";
                if(!raw) {
                    subfile_out += "Preset: " + String(received.preset) + "\n";
                    subfile_out += "Protocol: RcSwitch\n";
                    subfile_out += "Bit: " + String(received.Bit) + "\n";
                    subfile_out += "Key: " + String(hexString) + "\n";
                    subfile_out += "TE: " + String(received.te) + "\n";
                } else {
                    // save as raw
                    if(received.preset=="1") received.preset="FuriHalSubGhzPresetOok270Async";
                    else if (received.preset=="2") received.preset="FuriHalSubGhzPresetOok650Async";
                    subfile_out += "Preset: " + String(received.preset) + "\n";
                    subfile_out += "Protocol: RAW\n";
                    subfile_out += "RAW_Data: " + received.data;
                }
                // headless mode
                return subfile_out;
            #endif

            if(check(SelPress)) {
                int chosen=0;
                options = {
                    {"Replay signal",   [&]()  { chosen=1; } },
                    {"Save signal",     [&]()  { chosen=2; } },
                };
                loopOptions(options);
                if(chosen==1) {
                    rcswitch.disableReceive();
                    sendRfCommand(received);
                    addToRecentCodes(received);
                    goto RestartRec;
                }
                else if (chosen==2) {
                    decimalToHexString(received.key,hexString);
                    RCSwitch_SaveSignal(frequency, received, raw, hexString);

                    delay(2000);
                    drawMainBorder();
                    tft.setCursor(10, 28);
                    tft.setTextSize(FP);
                    tft.println("Waiting for a " + String(frequency) + " MHz " + "signal.");
                }
            }
        }
        //#ifndef HAS_SCREEN
        if(max_loops>0) {
            // headless mode, quit if nothing received after max_loops
            max_loops -= 1;
            delay(1000);
            if(max_loops==0) {
                Serial.println("timeout");
                return "";
            }
        }
        //#endif
    }
    Exit:
    delay(1);

    deinitRfModule();

    return "";
}

bool RCSwitch_SaveSignal(float frequency, RfCodes codes, bool raw, char* key)
{
    if (!codes.key && codes.data=="") {
        Serial.println("Empty signal, it was not saved.");
        return false;
    }

    String subfile_out = "Filetype: Bruce SubGhz File\nVersion 1\n";
    subfile_out += "Frequency: " + String(int(frequency * 1000000)) + "\n";
    if(!raw) {
        subfile_out += "Preset: " + String(codes.preset) + "\n";
        subfile_out += "Protocol: RcSwitch\n";
        subfile_out += "Bit: " + String(codes.Bit) + "\n";
        subfile_out += "Key: " + String(key) + "\n";
        subfile_out += "TE: " + String(codes.te) + "\n";
        //subfile_out += "RAW_Data: " + codes.data;
    } else {
        // save as raw
        if (codes.preset=="1") {
            codes.preset="FuriHalSubGhzPresetOok270Async";
        }
        else if (codes.preset=="2") {
            codes.preset="FuriHalSubGhzPresetOok650Async";
        }

        subfile_out += "Preset: " + String(codes.preset) + "\n";
        subfile_out += "Protocol: RAW\n";
        subfile_out += "RAW_Data: " + codes.data;
    }

    int i = 0;
    File file;
    String FS = "";

    if (SD.begin()) {
        if (!SD.exists("/BruceRF")) {
            SD.mkdir("/BruceRF");
        }

        while (SD.exists("/BruceRF/bruce_" + String(i) + ".sub")) {
            i++;
        }

        file = SD.open("/BruceRF/bruce_"+ String(i) +".sub", FILE_WRITE);
        FS="SD";
    } else if (LittleFS.begin()) {
        sdcardMounted=false;
        if (!checkLittleFsSize()) {
            return false;
        }
        if (!LittleFS.exists("/BruceRF")) {
            LittleFS.mkdir("/BruceRF");
        }

        while(LittleFS.exists("/BruceRF/bruce_" + String(i) + ".sub")) {
            i++;
        }

        file = LittleFS.open("/BruceRF/bruce_" + String(i) +".sub", FILE_WRITE);
        FS = "LittleFS";
    }

    if (file) {
        file.println(subfile_out);
        displaySuccess(FS + "/bruce_" + String(i) + ".sub");
    } else {
        Serial.println("Fail saving data to LittleFS");
        displayError("Error saving file", true);
    }

    file.close();
    return true;
}

// ported from https://github.com/sui77/rc-switch/blob/3a536a172ab752f3c7a58d831c5075ca24fd920b/RCSwitch.cpp
void RCSwitch_RAW_Bit_send(RfCodes data) {
  int nTransmitterPin = bruceConfig.rfTx;
  if(bruceConfig.rfModule==CC1101_SPI_MODULE) {
      #ifdef USE_CC1101_VIA_SPI
         nTransmitterPin = CC1101_GDO0_PIN;
      #else
        return;
      #endif
  }

  if (data.data == "")
    return;
  bool currentlogiclevel = false;
  int nRepeatTransmit = 1;
  for (int nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
    int currentBit = data.data.length();
    while(currentBit >= 0) {  // Starts from the end of the string until the max number of bits to send
        char c = data.data[currentBit];
        if(c=='1') {
            currentlogiclevel = true;
        } else if(c=='0') {
            currentlogiclevel = false;
        } else {
            Serial.println("Invalid data");
            currentBit--;
            continue;
            //return;
        }

      digitalWrite(nTransmitterPin, currentlogiclevel ? HIGH : LOW);
      delayMicroseconds(data.te);

      //Serial.print(currentBit);
      //Serial.print("=");
      //Serial.println(currentlogiclevel);

      currentBit--;
    }
  digitalWrite(nTransmitterPin, LOW);
  }
}


void RCSwitch_RAW_send(int * ptrtransmittimings) {
  int nTransmitterPin = bruceConfig.rfTx;
  if(bruceConfig.rfModule==CC1101_SPI_MODULE) {
      #ifdef USE_CC1101_VIA_SPI
         nTransmitterPin = CC1101_GDO0_PIN;
      #else
        return;
      #endif
  }

  if (!ptrtransmittimings)
    return;

  bool currentlogiclevel = true;
  int nRepeatTransmit = 1; // repeats RAW signal twice!
  //HighLow pulses ;

  for (int nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
    unsigned int currenttiming = 0;
    while( ptrtransmittimings[currenttiming] ) {  // && currenttiming < RCSWITCH_MAX_CHANGES
        if(ptrtransmittimings[currenttiming] >= 0) {
            currentlogiclevel = true;
        } else {
            // negative value
            currentlogiclevel = false;
            ptrtransmittimings[currenttiming] = (-1) * ptrtransmittimings[currenttiming];  // invert sign
        }

      digitalWrite(nTransmitterPin, currentlogiclevel ? HIGH : LOW);
      delayMicroseconds( ptrtransmittimings[currenttiming] );

      /*
      Serial.print(ptrtransmittimings[currenttiming]);
      Serial.print("=");
      Serial.println(currentlogiclevel);
      */

      currenttiming++;
    }
  digitalWrite(nTransmitterPin, LOW);
  }  // end for
}

void sendRfCommand(struct RfCodes rfcode) {
      uint32_t frequency = rfcode.frequency;
      String protocol = rfcode.protocol;
      String preset = rfcode.preset;
      String data = rfcode.data;
      uint64_t key = rfcode.key;
      byte modulation = 2;  // possible values for CC1101: 0 = 2-FSK, 1 =GFSK, 2=ASK, 3 = 4-FSK, 4 = MSK
      float deviation = 0;
      float rxBW = 0;  // Receive bandwidth
      float dataRate = 0; // Data Rate
/*
    Serial.println("sendRawRfCommand");
    Serial.println(data);
    Serial.println(frequency);
    Serial.println(preset);
    Serial.println(protocol);
  */

    // Radio preset name (configures modulation, bandwidth, filters, etc.).
    /*  supported flipper presets:
        FuriHalSubGhzPresetIDLE, // < default configuration
        FuriHalSubGhzPresetOok270Async, ///< OOK, bandwidth 270kHz, asynchronous
        FuriHalSubGhzPresetOok650Async, ///< OOK, bandwidth 650kHz, asynchronous
        FuriHalSubGhzPreset2FSKDev238Async, //< FM, deviation 2.380371 kHz, asynchronous
        FuriHalSubGhzPreset2FSKDev476Async, //< FM, deviation 47.60742 kHz, asynchronous
        FuriHalSubGhzPresetMSK99_97KbAsync, //< MSK, deviation 47.60742 kHz, 99.97Kb/s, asynchronous
        FuriHalSubGhzPresetGFSK9_99KbAsync, //< GFSK, deviation 19.042969 kHz, 9.996Kb/s, asynchronous
        FuriHalSubGhzPresetCustom, //Custom Preset
    */
    //struct Protocol rcswitch_protocol;
    int rcswitch_protocol_no = 1;
    if(preset == "FuriHalSubGhzPresetOok270Async") {
        rcswitch_protocol_no = 1;
        //  pulseLength , syncFactor , zero , one, invertedSignal
        //rcswitch_protocol = { 350, {  1, 31 }, {  1,  3 }, {  3,  1 }, false };
        modulation = 2;
        rxBW = 270;
    }
    else if(preset == "FuriHalSubGhzPresetOok650Async") {
        rcswitch_protocol_no = 2;
        //rcswitch_protocol = { 650, {  1, 10 }, {  1,  2 }, {  2,  1 }, false };
        modulation = 2;
        rxBW = 650;
    }
    else if(preset == "FuriHalSubGhzPreset2FSKDev238Async") {
        modulation = 0;
        deviation = 2.380371;
        rxBW = 238;
    }
    else if(preset == "FuriHalSubGhzPreset2FSKDev476Async") {
        modulation = 0;
        deviation = 47.60742;
        rxBW = 476;
    }
    else if(preset == "FuriHalSubGhzPresetMSK99_97KbAsync") {
        modulation = 4;
        deviation = 47.60742;
        dataRate = 99.97;
    }
    else if(preset == "FuriHalSubGhzPresetGFSK9_99KbAsync") {
        modulation = 1;
        deviation = 19.042969;
        dataRate = 9.996;
    }
    else {
        bool found=false;
        for(int p=0;p<30;p++) {
            if(preset == String(p)) {
                rcswitch_protocol_no = preset.toInt();
                found=true;
            }
        }
        if(!found) {
            Serial.print("unsupported preset: ");
            Serial.println(preset);
            return;
        }
    }


    // init transmitter
    if(!initRfModule("", frequency/1000000.0)) return;
    if(bruceConfig.rfModule == CC1101_SPI_MODULE) { // CC1101 in use
        #ifdef USE_CC1101_VIA_SPI
            // derived from https://github.com/LSatan/SmartRC-CC1101-Driver-Lib/blob/master/examples/Rc-Switch%20examples%20cc1101/SendDemo_cc1101/SendDemo_cc1101.ino
            ELECHOUSE_cc1101.setModulation(modulation);
            if(deviation) ELECHOUSE_cc1101.setDeviation(deviation);
            if(rxBW) ELECHOUSE_cc1101.setRxBW(rxBW);		// Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
            if(dataRate) ELECHOUSE_cc1101.setDRate(dataRate);
            pinMode(CC1101_GDO0_PIN, OUTPUT);
            ELECHOUSE_cc1101.setPA(12);       // set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12)   Default is max!
            ELECHOUSE_cc1101.SetTx();
        #else
            Serial.println("USE_CC1101_VIA_SPI not defined");
            return;
        #endif
    } else {
        // other single-pinned modules in use
        if(modulation != 2) {
            Serial.print("unsupported modulation: ");
            Serial.println(modulation);
            return;
        }
        initRfModule("tx", frequency/1000000.0);
    }

    if(protocol == "RAW") {
        // count the number of elements of RAW_Data
        int buff_size=0;
        int index=0;
        while(index>=0) {
            index=data.indexOf(' ', index+1);
            buff_size++;
        }
        // alloc buffer for transmittimings
        int* transmittimings  = (int *) calloc(sizeof(int), buff_size+1);  // should be smaller the data.length()
        size_t transmittimings_idx = 0;

        // split data into words, convert to int, and store them in transmittimings
        int startIndex = 0;
        index=0;
        for(transmittimings_idx=0; transmittimings_idx<buff_size; transmittimings_idx++) {
            index = data.indexOf(' ', startIndex);
            if (index == -1) {
                transmittimings[transmittimings_idx] = data.substring(startIndex).toInt();
            } else {
                transmittimings[transmittimings_idx] = data.substring(startIndex, index).toInt();
            }
            startIndex = index + 1;
        }
        transmittimings[transmittimings_idx] = 0;  // termination

        // send rf command
        displayTextLine("Sending..");
        RCSwitch_RAW_send(transmittimings);
        free(transmittimings);
    }
    else if (protocol == "BinRAW") {
        rfcode.data = hexStrToBinStr(rfcode.data);        // transform from "00 01 02 ... FF" into "00000000 00000001 00000010 .... 11111111"
        //Serial.println(rfcode.data);
        rfcode.data.trim();
        RCSwitch_RAW_Bit_send(rfcode);
    }

    else if(protocol == "RcSwitch") {
        data.replace(" ", "");  // remove spaces
        //uint64_t data_val = strtoul(data.c_str(), nullptr, 16);
        uint64_t data_val = rfcode.key;
        int bits = rfcode.Bit;
        int pulse = rfcode.te;  // not sure about this...
        int repeat = 10;
        /*
        Serial.print("RcSwitch: ");
        Serial.println(data);
        Serial.println(bits);
        Serial.println(pulse);
        Serial.println(rcswitch_protocol_no);
        * */
        displayTextLine("Sending..");
        RCSwitch_send(data_val, bits, pulse, rcswitch_protocol_no, repeat);
    }
    else if(protocol.startsWith("Princeton")) {
        RCSwitch_send(rfcode.key, rfcode.Bit, 350, 1, 10);
    }
    else {
        Serial.print("unsupported protocol: "); Serial.println(protocol);
        Serial.println("Sending RcSwitch 11 protocol");
        //if(protocol.startsWith("CAME") || protocol.startsWith("HOLTEC" || NICE)) {
            RCSwitch_send(rfcode.key, rfcode.Bit, 270, 11, 10);
        //}

        return;
    }

    //digitalWrite(bruceConfig.rfTx, LED_OFF);
    deinitRfModule();
}


void addToRecentCodes(struct RfCodes rfcode)  {
    // copy rfcode -> recent_rfcodes[recent_rfcodes_last_used]
    recent_rfcodes[recent_rfcodes_last_used] = rfcode;
    recent_rfcodes_last_used += 1;
    if(recent_rfcodes_last_used == 16) recent_rfcodes_last_used  = 0; // cycle
}


struct RfCodes selectRecentRfMenu() {
    // show menu with filenames
    options = { };
    bool exit = false;
    struct RfCodes selected_code;
    for(int i=0; i<16; i++) {
        if(recent_rfcodes[i].filepath=="") continue; // not inited
        // else
        options.push_back({ recent_rfcodes[i].filepath.c_str(), [i, &selected_code](){ selected_code = recent_rfcodes[i]; }});
    }
    options.push_back({ "Main Menu" , [&](){ exit=true; }});
    loopOptions(options);
    return(selected_code);
}


void otherRFcodes() {
  // interactive menu part only
  FS *fs = NULL;
  String filepath = "";
  struct RfCodes selected_code;

  returnToMenu=true;  // make sure menu is redrawn when quitting in any point

  options = {
      {"Recent", [&]()  { selected_code = selectRecentRfMenu(); }},
      {"LittleFS", [&]()   { fs=&LittleFS; }},
  };
  if(setupSdCard()) options.push_back({"SD Card", [&]()  { fs=&SD; }});

  loopOptions(options);

  if(fs == NULL) {  // recent menu was selected
    if(selected_code.filepath!="") sendRfCommand(selected_code);  // a code was selected
    return;
    // no need to proceed, go back
  }

  while (1) {
    delay(200);
    filepath = loopSD(*fs, true, "SUB");
    if(filepath=="" || check(EscPress)) return;  //  cancelled
    // else trasmit the file
    txSubFile(fs, filepath);
    delay(200);
  }
}


bool txSubFile(FS *fs, String filepath) {
  struct RfCodes selected_code;
  File databaseFile;
  String line;
  String txt;
  int total=0;
  int sent=0;

  if(!fs) return false;

  databaseFile = fs->open(filepath, FILE_READ);
  drawMainBorder();

  if (!databaseFile) {
    Serial.println("Failed to open database file.");
    displayError("Fail to open file", true);
    return false;
  }
  Serial.println("Opened sub file.");
  selected_code.filepath = filepath.substring( 1 + filepath.lastIndexOf("/") );

  // format specs: https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/file_formats/SubGhzFileFormats.md

  // Count the number of signals present in the .sub file
  displayTextLine("Reading File..");
  while (databaseFile.available()) {
    line = databaseFile.readStringUntil('\n');
    if( line.startsWith("Bit_RAW:") ||
        line.startsWith("Key:") ||
        line.startsWith("RAW_Data:") || 
        line.startsWith("Data_RAW:")) 
        {
            total++;
        }
  }
  databaseFile.close();
  Serial.printf("\nFound a total of %d code(s)\n", total);
  databaseFile = fs->open(filepath, FILE_READ);
  if(!databaseFile) Serial.println("Fail opening file again");
  // Analyse and send the signals
  while (databaseFile.available()) {
      line = databaseFile.readStringUntil('\n');
      txt=line.substring(line.indexOf(":") + 1);
      if(txt.endsWith("\r")) txt.remove(txt.length() - 1);
      txt.trim();
      if(line.startsWith("Protocol:"))  selected_code.protocol = txt;
      if(line.startsWith("Preset:"))   selected_code.preset = txt;
      if(line.startsWith("Frequency:")) selected_code.frequency = txt.toInt();
      if(line.startsWith("TE:")) selected_code.te = txt.toInt();
      if(line.startsWith("Bit:")) selected_code.Bit = txt.toInt();
      if(line.startsWith("Bit_RAW:")) selected_code.BitRAW = txt.toInt();
      if(line.startsWith("Key:")) selected_code.key = hexStringToDecimal(txt.c_str());
      if(line.startsWith("RAW_Data:") || line.startsWith("Data_RAW:")) { 
        selected_code.data = txt;
      }
      
      // If the signal is complete, send it and reset the signal to send the next command in the file, in case it has more RAW_Data
      if(selected_code.protocol!="" && selected_code.preset!="" && selected_code.frequency>0 && (selected_code.BitRAW>0 || selected_code.data!="" || selected_code.key>0)) {
        selected_code.data.trim(); // remove initial and final spaces and special characters
        addToRecentCodes(selected_code);

        // To send the signal using CC1101 sharing the SPI Bus with SDCard, we need to close the file first
        // Does not apply for Smoochiee board and StickCPlus for now.
        if(bruceConfig.rfModule==CC1101_SPI_MODULE) {
            #if SDCARD_MOSI==CC1101_MOSI_PIN
                size_t point = databaseFile.position(); // Save the last position read
                databaseFile.close();                   // Close the File
            #endif
            sendRfCommand(selected_code);
            #if SDCARD_MOSI==CC1101_MOSI_PIN
                databaseFile = fs->open(filepath, FILE_READ); // Open the file
                databaseFile.seek(point);                     // Head back to where we were
            #endif
        } 
        else sendRfCommand(selected_code);

        selected_code.BitRAW=0;
        selected_code.data="";
        selected_code.key=0;
        sent++;
        displayTextLine("Sent " + String(sent) + "/" + String(total));
        Serial.print(".");
        delay(50);
      }

      if(check(EscPress)) break;
  }
  Serial.printf("\nSent %d of %d signals\n", sent, total);
  
  databaseFile.close();
  delay(1000);
  deinitRfModule();
  return true;
}

// Static array of sub-GHz frequencies in MHz
static const float subghz_frequency_list[] = {
  /* 300 - 348 MHz Frequency Range */
  300.000f, 302.757f, 303.875f, 303.900f, 304.250f,
  307.000f, 307.500f, 307.800f, 309.000f, 310.000f,
  312.000f, 312.100f, 312.200f, 313.000f, 313.850f,
  314.000f, 314.350f, 314.980f, 315.000f, 318.000f,
  330.000f, 345.000f, 348.000f, 350.000f,

  /* 387 - 464 MHz Frequency Range */
  387.000f, 390.000f, 418.000f, 430.000f, 430.500f,
  431.000f, 431.500f, 433.075f, 433.220f, 433.420f,
  433.657f, 433.889f, 433.920f, 434.075f, 434.177f,
  434.190f, 434.390f, 434.420f, 434.620f, 434.775f,
  438.900f, 440.175f, 464.000f, 467.750f,

  /* 779 - 928 MHz Frequency Range */
  779.000f, 868.350f, 868.400f, 868.800f, 868.950f,
  906.400f, 915.000f, 925.000f, 928.000f
};

#define _MAX_TRIES 5

struct FreqFound {
    float freq;
    int rssi;
};

void rf_scan_copy_draw_signal(RfCodes received, int signals, bool ReadRAW) {
    char hexString[64];
    const char* b;
    std::string txt=received.data.c_str();
    std::stringstream ss(txt);
    std::string palavra;
    int transitions = 0;

    while (ss >> palavra) transitions++;

    drawMainBorder();
    tft.setCursor(10, 28);
    tft.setTextSize(FP);


    if(received.key>0) {
        b = dec2binWzerofill(received.key, received.Bit);
        decimalToHexString(received.key,hexString);
    } else strcpy(hexString,"RAW data");

    tft.println("Key: " + String(hexString));

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
        tft.setCursor(10, tft.getCursorY());
        int rssi=ELECHOUSE_cc1101.getRssi();
        tft.drawPixel(0,0,0);
        tft.println("Rssi: " + String(rssi));
    }
    if(received.key>0) {
        tft.setCursor(10, tft.getCursorY());
        tft.println("Binary: " + String(b));
        tft.setCursor(10, tft.getCursorY());
        tft.println("Lenght: " + String(received.Bit) + " bits");
        tft.setCursor(10, tft.getCursorY());
        tft.println("PulseLenght: " + String(received.te) + "ms");
        tft.setCursor(10, tft.getCursorY());
        tft.println("Protocol: " + String(received.protocol) +  "(" + received.preset + ")");
    } else {
        tft.setCursor(10, tft.getCursorY());
        tft.println("Transitions: " + String(transitions));
        tft.setCursor(10, tft.getCursorY());
        tft.println("Protocol: " + String(received.protocol));
    }
    tft.setCursor(10, tft.getCursorY());
    tft.println("Frequency: " + String(received.frequency) + " MHz");
    tft.setCursor(10, tft.getCursorY());
    tft.println("Total signals found: " + String(signals));
    if(ReadRAW) {
        tft.setCursor(10, tft.getCursorY()+LH);
        tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
        tft.println("Reading RAW data.");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    }
    tft.setCursor(10, tft.getCursorY()+LH);
    tft.println("Press [NEXT] for options.");

}

void rf_scan_copy() {
	RfCodes received;
	RCSwitch rcswitch = RCSwitch();
    bool OnlyRAW = false;
    const char* sz_range[] = {"300-348 MHz", "387-464 MHz", "779-928 MHz", "All ranges" };
	int range_limits[][2] = {
		{ 0, 23 },  // 300-348 MHz
		{ 24, 47 }, // 387-464 MHz
		{ 48, 56 }, // 779-928 MHz
		{ 0, sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]) - 1} // All ranges
	};
	uint8_t _try = 0;
	char hexString[64];
	int signals = 0, idx = range_limits[bruceConfig.rfScanRange][0];
	float found_freq = 0.f, frequency = 0.f;
	int rssi=-80, rssiThreshold = -55;
	FreqFound _freqs[_MAX_TRIES]; // get the best RSSI out of 5 tries
    bool ReadRAW=true;

RestartScan:
    // Resets the Scan arrays
    for(int i=0; i<_MAX_TRIES;i++) {_freqs[i].freq=433.92; _freqs[i].rssi=-75; }
    _try=0;

	if (!initRfModule("rx",bruceConfig.rfFreq)) {
		return;
	}

	if (bruceConfig.rfModule == CC1101_SPI_MODULE) {
		#ifdef USE_CC1101_VIA_SPI
			#ifdef CC1101_GDO2_PIN
				rcswitch.enableReceive(CC1101_GDO2_PIN);
			#else
				rcswitch.enableReceive(CC1101_GDO0_PIN);
				Serial.println("CC1101 enableReceive()");
			#endif
		#else
			return;
		#endif
	} else {
		rcswitch.enableReceive(bruceConfig.rfRx);
	}

	if (bruceConfig.rfScanRange < 0 || bruceConfig.rfScanRange > 3) {
		bruceConfig.setRfScanRange(3);
	}

	if (bruceConfig.rfModule != CC1101_SPI_MODULE) {
		bruceConfig.setRfFxdFreq(1);
	}

	drawMainBorder();
	tft.setCursor(10, 28);
	tft.setTextSize(FP);
    if(received.data!="") rf_scan_copy_draw_signal(received,signals,ReadRAW);
    else {
        tft.println("Waiting for signal.");
        tft.setCursor(10, tft.getCursorY());
        if (bruceConfig.rfFxdFreq) {
            if (_try >= _MAX_TRIES) {
                tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
            }

            tft.println("Freq: " + String(bruceConfig.rfFreq) + " MHz");

            if (_try >= _MAX_TRIES) {
                tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
            }
        }
        else {
            tft.println("Range: " + String(sz_range[bruceConfig.rfScanRange]));
        }

        if(ReadRAW) {
            tft.setCursor(10, tft.getCursorY()+LH);
            tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
            tft.println("Reading RAW data.");
            tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        }
        tft.setCursor(10, tft.getCursorY()+LH*2);
        tft.println("Press [NEXT] for range.");
    }

	if (bruceConfig.rfFxdFreq) {
		frequency = bruceConfig.rfFreq;
	}
    // Clear cache for RAW signal
    rcswitch.resetAvailable();
    returnToMenu=false;
	for (;;) {
	FastScan:
		if (idx < range_limits[bruceConfig.rfScanRange][0] || idx > range_limits[bruceConfig.rfScanRange][1]) {
			idx = range_limits[bruceConfig.rfScanRange][0];
		}

		if (check(EscPress) || returnToMenu) {
			break;
		}

		if (!bruceConfig.rfFxdFreq) { // Try FastScan
        #if defined(USE_CC1101_VIA_SPI)
			frequency = subghz_frequency_list[idx];

			setMHZ(frequency);
            tft.drawPixel(0,0,0); // To make sure CC1101 shared with TFT works properly

			delay(5);
			rssi = ELECHOUSE_cc1101.getRssi();
            if (check(SelPress)) {
                Serial.println("Frequency: " + String(frequency) + " - rssi: " + String(rssi));
            }

			if (rssi > rssiThreshold) {
				_freqs[_try].freq = frequency;
				_freqs[_try].rssi = rssi;
				_try++;
				if (_try >= _MAX_TRIES) {
					int max_index = 0;
					for (int i = 1; i < _MAX_TRIES; ++i) {
						if (_freqs[i].rssi > _freqs[max_index].rssi) {
							max_index = i;
						}
					}

					bruceConfig.setRfFreq(_freqs[max_index].freq, true);
					frequency = _freqs[max_index].freq;
					Serial.println("Frequency Found: " + String(frequency));
                    deinitRfModule();
					goto RestartScan;
				}
				else {
					++idx;
				}
			}
			else {
				++idx;
				if (check(NextPress)) {
					goto Menu;
				}

				goto FastScan;
			}
        #else
        displayWarning("Freq Scan not available", true);
        bruceConfig.setRfFreq(433.92, 1);
        #endif
		}

		if (rcswitch.available() && !OnlyRAW) {     // Decoded by the lib
			unsigned long value = rcswitch.getReceivedValue();
			if (value) { // if there are a value decoded by RCSwitch, shows it first
            	found_freq = frequency;
				++signals;

				unsigned int* raw = rcswitch.getReceivedRawdata();
				received.frequency = long(frequency*1000000);
				received.key = value;
                received.preset = String(rcswitch.getReceivedProtocol());
				received.protocol = "RcSwitch";
				received.te = rcswitch.getReceivedDelay();
				received.Bit = rcswitch.getReceivedBitlength();
				received.filepath = "signal_"+String(signals);;
				received.data = "";
				int sign = +1;
				//if(received.preset.invertedSignal) sign = -1;
				for (int i = 0; i < received.Bit * 2; i++) {
					if (i > 0) received.data += " ";

					if (i % 2 == 0) sign = +1;
					else sign = -1;

					received.data += String(sign * (int)raw[i]);
				}

                rf_scan_copy_draw_signal(received,signals,ReadRAW);

			}
            rcswitch.resetAvailable();
        }

        if(rcswitch.RAWavailable() && ReadRAW){ // if no value were decoded, show raw data to be saved
            delay(400); // wait for all the signal to be read
            found_freq = frequency;
            ++signals;

            unsigned int* raw = rcswitch.getRAWReceivedRawdata();
            int transitions = 0;
            signed int sign=1;
            String _data="";
            for(transitions=0; transitions<RCSWITCH_RAW_MAX_CHANGES; transitions++) {
                if(raw[transitions]==0) break;
                if(transitions>0) _data+=" ";
                if(transitions % 2 == 0) sign = +1;
                    else sign = -1;
                _data += String(sign * (int)raw[transitions]);
            }

            received.te = 0;
            received.key = 0;
            received.Bit = 0;
            received.frequency = long(frequency*1000000);
            received.protocol = "RAW";
            received.filepath = "signal_"+String(signals);
            received.data = _data;
            received.preset = "0"; // ????
            rf_scan_copy_draw_signal(received,signals,ReadRAW);

            rcswitch.resetAvailable();
        }

		if (check(NextPress)) {
        Menu:
			int option = -1;
            options={};
            if(received.data!="") {
                                                        options.push_back({ "Replay",       [&]()  { option = 0; } });
                                                        options.push_back({ "Save Signal",  [&]()  { option = 2; } });
                                                        options.push_back({ "Reset Signal", [&]()  { option = 3; } });
            }
            if(bruceConfig.rfModule==CC1101_SPI_MODULE) options.push_back({ "Range",        [&]()  { option = 1; } });
            
            if(ReadRAW)                                 options.push_back({ "Stop RAW",     [&]()  {  ReadRAW=false; } });
            else                                        options.push_back({ "Read RAW",     [&]()  {  ReadRAW=true; } });
            if(bruceConfig.devMode && !OnlyRAW)         options.push_back({ "Only RAW",     [&]()  {  ReadRAW=true; OnlyRAW=true; } });
            else if(bruceConfig.devMode && OnlyRAW)     options.push_back({ "RAW+Decode",   [&]()  {  ReadRAW=true; OnlyRAW=false; } });
                                                        options.push_back({ "Close Menu",   [&]()  {  option =-1; } });
                                                        options.push_back({ "Main Menu",    [&]()  {  option =-2; } });


            loopOptions(options);

            if(option==-1) goto RestartScan;

            if(option==-2) { returnToMenu=true; goto END; }

            if(option ==0 ) { // Replay signal
            ReplaySignal:
                rcswitch.disableReceive();
                sendRfCommand(received);
                addToRecentCodes(received);

                deinitRfModule();
                delay(200);
                goto RestartScan;
            }

			if (option == 1) { // Range 
                option=0;
				options = {
					{ String("Fxd [" + String(bruceConfig.rfFreq) + "]").c_str(), [=]()  { bruceConfig.setRfScanRange(bruceConfig.rfScanRange, 1); } },
                    { String("Choose Fxd").c_str(), [&]()  { option = 1; } },
					{ sz_range[0], [=]()  { bruceConfig.setRfScanRange(0); } },
					{ sz_range[1], [=]()  { bruceConfig.setRfScanRange(1); } },
					{ sz_range[2], [=]()  { bruceConfig.setRfScanRange(2); } },
					{ sz_range[3], [=]()  { bruceConfig.setRfScanRange(3); } },
				};

				loopOptions(options);

                if(option == 1) {
                    options = {};
                    int ind=0;
                    int arraySize = sizeof(subghz_frequency_list) / sizeof(subghz_frequency_list[0]);
                    for(int i=0; i<arraySize;i++) {
                        options.push_back({ String(String(subghz_frequency_list[i],2) + "Mhz").c_str(), [=]()  { bruceConfig.rfFreq=subghz_frequency_list[i]; } });
                        if(int(frequency*100)==int(subghz_frequency_list[i]*100)) ind=i;
                    }
				    loopOptions(options,ind);
                    bruceConfig.setRfScanRange(bruceConfig.rfScanRange, 1);
                }

				if (bruceConfig.rfFxdFreq) displayTextLine("Scan freq set to " + String(bruceConfig.rfFreq));
				else displayTextLine("Range set to " + String(sz_range[bruceConfig.rfScanRange]));
                
                deinitRfModule();
				delay(1500);
				goto RestartScan;
			}
			else if (option == 2) { // Save Signal
                Serial.println(received.protocol=="RAW"? "RCSwitch_SaveSignal RAW true":"RCSwitch_SaveSignal RAW false");
                decimalToHexString(received.key,hexString);
                RCSwitch_SaveSignal(found_freq, received, received.protocol=="RAW"? true:false, hexString);
                deinitRfModule();
                delay(200);
                goto RestartScan;
            }
            else if (option == 3) { // Set Default
                received.Bit=0;
                received.data="";
                received.key=0;
                received.preset="";
                received.protocol="";
                deinitRfModule();
                delay(1500);
                goto RestartScan;
		    }
        }
		++idx;
	}
    END:
	deinitRfModule();
}
