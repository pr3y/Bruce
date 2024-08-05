//@IncursioHack - github.com/IncursioHack

#include <driver/rmt.h>
#include <RCSwitch.h>
#include "PCA9554.h"
#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "rf.h"

// Cria um objeto PCA9554 com o endereço I2C do PCA9554PW
// PCA9554 extIo1(pca9554pw_address);

#define RMT_RX_CHANNEL  RMT_CHANNEL_6
#define RMT_BLOCK_NUM


#define RMT_CLK_DIV   80 /*!< RMT counter clock divider */
#define RMT_1US_TICKS (80000000 / RMT_CLK_DIV / 1000000)
#define RMT_1MS_TICKS (RMT_1US_TICKS * 1000)

#define SIGNAL_STRENGTH_THRESHOLD 1500 // Adjust this threshold as needed

#define DISPLAY_HEIGHT 130 // Height of the display area for the waveform
#define DISPLAY_WIDTH  240 // Width of the display area
#define LINE_WIDTH 2 // Adjust line width as needed


struct RfCodes {
  uint32_t frequency = 0;
  uint64_t key=0;
  String protocol = "";
  String preset = "";
  String data = "";
  int te = 0;
  String filepath = "";
  int Bit=0;
  int BitRAW=0;
};


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
    rxconfig.gpio_num            = gpio_num_t(RfRx);
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
                tft.fillRect(0, 20, WIDTH, HEIGHT, TFT_BLACK);
                // Draw waveform based on signal strength
                for (size_t i = 0; i < rx_size; i++) {
                    int lineHeight = map(item[i].duration0 + item[i].duration1, 0, SIGNAL_STRENGTH_THRESHOLD, 0, HEIGHT/2);
                    int lineX = map(i, 0, rx_size - 1, 0, WIDTH - 1); // Map i to within the display width
                    // Ensure drawing coordinates stay within the box bounds
                    int startY = constrain(20 + HEIGHT / 2 - lineHeight / 2, 20, 20 + HEIGHT);
                    int endY = constrain(20 + HEIGHT / 2 + lineHeight / 2, 20, 20 + HEIGHT);
                    tft.drawLine(lineX, startY, lineX, endY, TFT_PURPLE);
                }
            }
            vRingbufferReturnItem(rb, (void*)item);
        }
        // Checks to leave while
        if (checkEscPress()) {
            break;
        }
    }
    returnToMenu=true;
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
                // keeps the pin active for a while and increase increase
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(50);
                }

                digitalWrite(RfTx, LOW); // Desativa o pino
                // keeps the pin inactive for the same time as before
                for (int widthsize = 1; widthsize <= (1 + sequence); widthsize++) {
                    delayMicroseconds(50);
                }
            }
        }
    }

    digitalWrite(RfTx, LOW); // Deactivate pin
}


void RCSwitch_send(uint64_t data, unsigned int bits, int pulse, int protocol, int repeat)
{
    RCSwitch mySwitch = RCSwitch();
    mySwitch.enableTransmit(RfTx);
    mySwitch.setProtocol(protocol);
    if (pulse) { mySwitch.setPulseLength(pulse); }
    mySwitch.setPulseLength(pulse);
    mySwitch.setRepeatTransmit(repeat);
    mySwitch.send(data, bits);

    /*
    Serial.println(data,HEX);
    Serial.println(bits);
    Serial.println(pulse);
    Serial.println(protocol);
    Serial.println(repeat);
    * */

    mySwitch.disableTransmit();
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
        decimal <<= 8; // Shift left to accomodate next byte

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

void RCSwitch_Read_Raw() {
    RCSwitch rcswitch = RCSwitch();
    RfCodes received;

    drawMainBorder();
    tft.setCursor(10, 28);
    tft.setTextSize(FP);
    tft.println("Waiting for signal.");
    char hexString[64];
RestartRec:
    pinMode(RfRx, INPUT);
    rcswitch.enableReceive(RfRx);
    while(!checkEscPress()) {
        if(rcswitch.available()) {
            long value = rcswitch.getReceivedValue();
            Serial.println("Available");
            if(value) {
                Serial.println("has value");

                unsigned int* raw = rcswitch.getReceivedRawdata();
                received.frequency=433920000;
                received.key=rcswitch.getReceivedValue();
                received.protocol="RcSwitch";
                received.preset=rcswitch.getReceivedProtocol();
                received.te=rcswitch.getReceivedDelay();
                received.Bit=rcswitch.getReceivedBitlength();
                received.filepath="Last copied";

                for(int i=0; i<received.te*2;i++) {
                    if(i>0) received.data+=" ";
                    received.data+=raw[i];
                }
                Serial.println(received.protocol);
                Serial.println(received.data);
                const char* b = dec2binWzerofill(received.key, received.Bit);
                drawMainBorder();
                tft.setCursor(10, 28);
                tft.setTextSize(FP);
                decimalToHexString(received.key,hexString);
                tft.println("Key: " + String(hexString));
                tft.setCursor(10, tft.getCursorY());
                tft.println("Binary: " + String(b));
                tft.setCursor(10, tft.getCursorY());
                tft.println("Lenght: " + String(received.Bit) + " bits");
                tft.setCursor(10, tft.getCursorY());
                tft.println("PulseLenght: " + String(received.te) + "ms");
                tft.setCursor(10, tft.getCursorY());
                tft.println("Protocol: " + String(received.protocol));
                tft.setCursor(10, tft.getCursorY()+LH*2);
                tft.println("Press " + String(BTN_ALIAS) + "for options.");
            }
            rcswitch.resetAvailable();
            previousMillis = millis();
        }
        if(received.key>0) {
            if(checkSelPress()) {
                int chosen=0;
                options = {
                    {"Replay signal",   [&]()  { chosen=1; } },
                    {"Save signal",     [&]()  { chosen=2; } },
                };
                delay(200);
                loopOptions(options);
                if(chosen==1) {
                    rcswitch.disableReceive();
                    sendRfCommand(received);
                    addToRecentCodes(received);
                    displayRedStripe("Waiting Signal",TFT_WHITE, FGCOLOR);
                    goto RestartRec;
                }
                else if (chosen==2) {
                    int i=0;
                    File file;
                    String FS="";
                    if(SD.begin()) {
                        if (!SD.exists("/BruceRF")) SD.mkdir("/BruceRF");
                        while(SD.exists("/BruceRF/bruce_" + String(i) + ".sub")) i++;
                        file = SD.open("/BruceRF/bruce_"+ String(i) +".sub", FILE_WRITE);
                        FS="SD";
                    } else if(LittleFS.begin()) {
                        if(!checkLittleFsSize()) goto Exit;
                        if (!LittleFS.exists("/BruceRF")) LittleFS.mkdir("/BruceRF");
                        while(LittleFS.exists("/BruceRF/bruce_" + String(i) + ".sub")) i++;
                        file = LittleFS.open("/BruceRF/bruce_"+ String(i) +".sub", FILE_WRITE);
                        FS="LittleFS";
                    }
                    if(file) {
                        file.println("Filetype: Bruce SubGhz RAW File\nVersion 1\nFrequency: 433920000");
                        if(received.preset=="1") received.preset="FuriHalSubGhzPresetOok270Async";
                        else if (received.preset=="2") received.preset="FuriHalSubGhzPresetOok650Async";
                        file.println("Preset: " + String(received.preset));
                        file.println("Protocol: RcSwitch");
                        file.println("Bit: " + String(received.Bit));
                        file.println("Key: " + String(hexString));
                        //file.println("RAW_Data: " + received.data); // not in flipper pattern
                        file.println("TE: " + String(received.te));
                        displaySuccess(FS + "/bruce_" + String(i) + ".sub");
                    } else {
                        Serial.println("Fail saving data to LittleFS");
                        displayError("Error saving file");
                    }
                    file.close();
                    delay(2000);
                    drawMainBorder();
                    tft.setCursor(10, 28);
                    tft.setTextSize(FP);
                    tft.println("Waiting for signal.");
                }
            }
        }
    }
    Exit:
    delay(1);
}


// ported from https://github.com/sui77/rc-switch/blob/3a536a172ab752f3c7a58d831c5075ca24fd920b/RCSwitch.cpp
void RCSwitch_RAW_Bit_send(int nTransmitterPin, RfCodes data) {
  if (nTransmitterPin == -1)
    return;
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

      Serial.print(currentBit);
      Serial.print("=");
      Serial.println(currentlogiclevel);

      currentBit--;
    }
  digitalWrite(nTransmitterPin, LOW);
  }
}
void RCSwitch_RAW_send(int nTransmitterPin, int * ptrtransmittimings, struct Protocol protocol) {
  if (nTransmitterPin == -1)
    return;

  if (!ptrtransmittimings)
    return;

  bool currentlogiclevel = true;
  int nRepeatTransmit = 1;
  //HighLow pulses ;

  for (int nRepeat = 0; nRepeat < nRepeatTransmit; nRepeat++) {
    unsigned int currenttiming = 0;
    while( ptrtransmittimings[currenttiming] ) {  // && currenttiming < RCSWITCH_MAX_CHANGES
        if(ptrtransmittimings[currenttiming] >= 0) {
            currentlogiclevel = true;
            //pulses = protocol.one;
        } else {
            // negative value
            currentlogiclevel = false;
            ptrtransmittimings[currenttiming] = (-1) * ptrtransmittimings[currenttiming];  // invert sign
            //pulses = protocol.zero;
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
/*
    Serial.println("sendRawRfCommand");
    Serial.println(data);
    Serial.println(frequency);
    Serial.println(preset);
    Serial.println(protocol);
  */
    if(frequency != 433920000) {
        Serial.print("unsupported frequency: ");
        Serial.println(frequency);
        return;
    }
    // MEMO: frequency is fixed with some transmitters https://github.com/sui77/rc-switch/issues/256
    // TODO: add frequency switching via CC1101  https://github.com/LSatan/SmartRC-CC1101-Driver-Lib

    // Radio preset name (configures modulation, bandwidth, filters, etc.).
    struct Protocol rcswitch_protocol;
    int rcswitch_protocol_no = 1;
    if(preset == "FuriHalSubGhzPresetOok270Async") {
        rcswitch_protocol_no = 1;
        //  pulseLength , syncFactor , zero , one, invertedSignal
        rcswitch_protocol = { 350, {  1, 31 }, {  1,  3 }, {  3,  1 }, false };
    }
    else if(preset == "FuriHalSubGhzPresetOok650Async") {
        rcswitch_protocol_no = 2;
        rcswitch_protocol = { 650, {  1, 10 }, {  1,  2 }, {  2,  1 }, false };
    } else if(preset == "1" || preset == "2" || preset == "3" || preset == "4" || preset == "5" || preset == "6" || preset == "7" || preset == "8" || preset == "9" || preset == "10" || preset == "11" || preset == "12"|| preset == "13" || preset == "14") {
        rcswitch_protocol_no = preset.toInt();
    }
    else {
        Serial.print("unsupported preset: ");
        Serial.println(preset);
        return;
    }
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

    // init output pin
    digitalWrite(RfTx, LED_OFF);
    if(RfTx==0) RfTx=GROVE_SDA; // quick fix
    pinMode(RfTx, OUTPUT);

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
        displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
        RCSwitch_RAW_send(RfTx, transmittimings, rcswitch_protocol);
        free(transmittimings);
    }
    else if (protocol == "BinRAW") {
        rfcode.data = hexStrToBinStr(rfcode.data);        // transform from "00 01 02 ... FF" into "00000000 00000001 00000010 .... 11111111"
        Serial.println(rfcode.data);
        rfcode.data.trim();
        RCSwitch_RAW_Bit_send(RfTx,rfcode);
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
        displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
        RCSwitch_send(data_val, bits, pulse, rcswitch_protocol_no, repeat);
    }
    else {
        Serial.print("unsupported protocol: ");
        Serial.println(protocol);
        return;
    }

    digitalWrite(RfTx, LED_OFF);
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
    delay(200);
    loopOptions(options);
    return(selected_code);
}


void otherRFcodes() {
  // interactive menu part only
  FS *fs = NULL;
  String filepath = "";
  struct RfCodes selected_code;
  options = {
      {"Recent", [&]()  { selected_code = selectRecentRfMenu(); }},
      {"LittleFS", [&]()   { fs=&LittleFS; }},
  };
  if(setupSdCard()) options.push_back({"SD Card", [&]()  { fs=&SD; }});

  delay(200);
  loopOptions(options);
  delay(200);

  if(fs == NULL) {  // recent menu was selected
    if(selected_code.filepath!="") sendRfCommand(selected_code);  // a code was selected
    return;
    // no need to proceed, go back
  }

  filepath = loopSD(*fs, true, "SUB");
  if(filepath=="") return;  //  cancelled
  // else
  txSubFile(fs, filepath);
  returnToMenu=true;
}
  
  
bool txSubFile(FS *fs, String filepath) {
  struct RfCodes selected_code;
  File databaseFile;
  
  if(!fs) return false;
  
  databaseFile = fs->open(filepath, FILE_READ);
  drawMainBorder();

  if (!databaseFile) {
    Serial.println("Failed to open database file.");
    displayError("Fail to open file");
    delay(2000);
    return false;
  }
  Serial.println("Opened sub file.");
  selected_code.filepath = filepath.substring( 1 + filepath.lastIndexOf("/") );

  // format specs: https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/file_formats/SubGhzFileFormats.md
  String line;
  String txt;
  while (databaseFile.available() ) {
      line = databaseFile.readStringUntil('\n');
      txt=line.substring(line.indexOf(":") + 1);
      txt.trim();
      if(line.startsWith("Protocol:"))  selected_code.protocol = txt;
      if(line.startsWith("Preset:"))   selected_code.preset = txt;
      if(line.startsWith("Frequency:")) selected_code.frequency = txt.toInt();
      if(line.startsWith("TE:")) selected_code.te = txt.toInt();
      if(line.startsWith("Bit:")) selected_code.Bit = txt.toInt();
      if(line.startsWith("Bit_RAW:")) selected_code.BitRAW = txt.toInt();
      if(line.startsWith("RAW_Data:") || line.startsWith("Data_RAW:")) selected_code.data +=" " + txt; // add a space at the end, some files have more than one RAW_Data. This initial space will be trimmed
      if(line.startsWith("Key:")) selected_code.key = hexStringToDecimal(txt.c_str());
  }
  selected_code.data.trim(); // remove initial and final spaces and special characters
  databaseFile.close();

  addToRecentCodes(selected_code);
  sendRfCommand(selected_code);

  digitalWrite(RfTx, LED_OFF);
  return true;
}

