/*
Last Updated: 30 Mar. 2018
By Anton Grimpelhuber (anton.grimpelhuber@gmail.com)

------------------------------------------------------------
LICENSE:
------------------------------------------------------------
Distributed under Creative Commons 2.5 -- Attribution & Share Alike

*/

#include "TV-B-Gone.h"
#include "display.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include "WORLD_IR_CODES.h"

uint8_t read_bits(uint8_t count);
uint16_t rawData[300];

#define IR_DATA_BUFFER_SIZE 300

#define MAX_WAIT_TIME 65535 //tens of us (ie: 655.350ms)

IRsend irsend(LED);  // Set the GPIO to be used to sending the message.

uint8_t bitsleft_r = 0;
uint8_t bits_r=0;
uint8_t code_ptr;
volatile const IrCode * powerCode;
uint8_t num_NAcodes = NUM_ELEM(NApowerCodes);
uint8_t num_EUcodes = NUM_ELEM(EUpowerCodes);
uint16_t ontime, offtime;
uint8_t i,num_codes;
uint8_t region;


// we cant read more than 8 bits at a time so dont try!
uint8_t read_bits(uint8_t count)
{
  uint8_t i;
  uint8_t tmp=0;

  // we need to read back count bytes
  for (i=0; i<count; i++) {
    if (bitsleft_r == 0) {
      bits_r = powerCode->codes[code_ptr++];
      bitsleft_r = 8;
    }
    bitsleft_r--;
    tmp |= (((bits_r >> (bitsleft_r)) & 1) << (count-1-i));
  }
  return tmp;
}

void chooseRegion(int reg) {
region = reg;
  if (reg) num_codes=num_NAcodes;
  else num_codes=num_EUcodes;
}

void StartTvBGone()   
{
  Serial.begin(115200);

  irsend.begin();

  digitalWrite(LED, LED_ON);
  pinMode(LED, OUTPUT);

  delay_ten_us(5000); //50ms (5000x10 us) delay: let everything settle for a bit

  // determine region
  options = {
      {"Region NA", [=]() { chooseRegion(NA); }},
      {"Region EU", [=]()    { chooseRegion(EU); }},
  };
  delay(200);
  loopOptions(options);
  delay(200);

  sendAllCodes();
}

void sendAllCodes() 
{
  bool endingEarly = false; //will be set to true if the user presses the button during code-sending 

  checkSelPress();
  for (i=0 ; i<num_codes; i++) 
  {
    if (region == NA) powerCode = NApowerCodes[i];
    else powerCode = EUpowerCodes[i];
    
    const uint8_t freq = powerCode->timer_val;
    const uint8_t numpairs = powerCode->numpairs;
    const uint8_t bitcompression = powerCode->bitcompression;

    // For EACH pair in this code....
    code_ptr = 0;
    for (uint8_t k=0; k<numpairs; k++) {
      uint16_t ti;
      ti = (read_bits(bitcompression)) * 2;
      ontime = powerCode->times[ti];  // read word 1 - ontime
      offtime = powerCode->times[ti+1];  // read word 2 - offtime

      rawData[k*2] = ontime * 10;
      rawData[(k*2)+1] = offtime * 10;
      yield();
    }

    progressHandler(i, num_codes);
    irsend.sendRaw(rawData, (numpairs*2) , freq);
    yield();
    bitsleft_r=0;

    quickflashLED();
    // delay 205 milliseconds before transmitting next POWER code
    delay_ten_us(20500);

    // if user is pushing (holding down) TRIGGER button, stop transmission early 
    if (checkSelPress()) // Pause TV-B-Gone
    {
      while (checkSelPress()){
        yield();
      }
      displayRedStripe("Paused", TFT_WHITE, BGCOLOR);

      while (!checkSelPress()){ // Se apertar Select denovo, continua
        if(checkEscPress()) { 
          endingEarly= true;
          break;
        }
      }
      while (checkSelPress()){
        yield();
      }
      if (endingEarly) break; // Cancela o TV-B-Gone
      displayRedStripe("Running, Wait", TFT_WHITE, FGCOLOR);
    }
    
  } //end of POWER code for loop


  if (endingEarly==false)
  {
    displayRedStripe("All codes sent!", TFT_WHITE, FGCOLOR);
    //pause for ~1.3 sec, then flash the visible LED 8 times to indicate that we're done
    delay_ten_us(MAX_WAIT_TIME); // wait 655.350ms
    delay_ten_us(MAX_WAIT_TIME); // wait 655.350ms
  } else {
    displayRedStripe("User Stoped");
  }

  //turnoff LED
  digitalWrite(LED,LED_OFF);

} //end of sendAllCodes

/****************************** LED AND DELAY FUNCTIONS ********/

void delay_ten_us(uint16_t us) {
  uint8_t timer;
  while (us != 0) {
    // for 8MHz we want to delay 80 cycles per 10 microseconds
    // this code is tweaked to give about that amount.
    for (timer=0; timer <= DELAY_CNT; timer++) {
      NOPP;
      NOPP;
    }
    NOPP;
    us--;
  }
}


// This function quickly pulses the visible LED (connected to PB0, pin 5)
// This will indicate to the user that a code is being transmitted
void quickflashLED( void ) {
  digitalWrite(LED, LED_ON);
  delay_ten_us(3000);   // 30 ms ON-time delay
  digitalWrite(LED, LED_OFF);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom IR

struct Codes {
  String name;
  String type;
  String protocol;
  String address;
  String command;
  uint32_t frequency;
  //float duty_cycle;
  String data;
};

Codes codes[50];

void resetCodesArray() {
  for (int i = 0; i < 25; i++) {
    codes[i].name = "";
    codes[i].type = "";
    codes[i].protocol = "";
    codes[i].address = "";
    codes[i].command = "";
    codes[i].frequency = 0;
    //codes[i].duty_cycle = 0.0;
    codes[i].data = "";
  }
}

void otherIRcodes() {
  resetCodesArray();
  int total_codes = 1;
  String filepath;
  File databaseFile;
  FS *fs;
  if(setupSdCard()) {
    bool teste=false;
    options = {
      {"SD Card", [&]()  { fs=&SD; }}, 
      {"LittleFS", [&]()   { fs=&LittleFS; }},
    };
    delay(200);
    loopOptions(options);
    delay(200);

  } else fs=&LittleFS;

  filepath = loopSD(*fs, true);
  databaseFile = fs->open(filepath, FILE_READ);
  drawMainBorder();
  pinMode(LED, OUTPUT);
  //digitalWrite(LED, LED_ON);

  if (!databaseFile) {
    Serial.println("Failed to open database file.");
    displayError("Fail to open file");
    delay(2000);
    return;
  }
  Serial.println("Opened database file.");
  bool mode_cmd=true;
  options = {
    {"Choose cmd", [&]()  { mode_cmd=true; }}, 
    {"Spam all", [&]()    { mode_cmd=false; }},
  };
  delay(200);
  loopOptions(options);
  delay(200);


  String line;

  // Mode to choose and send command by command limitted to 50 commands
  if(mode_cmd) {
    String txt;
    while (databaseFile.available() && total_codes<=50) {
      line = databaseFile.readStringUntil('\n');
      txt=line.substring(line.indexOf(":") + 1);
      txt.trim();
      if(line.startsWith("name:"))      codes[total_codes].name = txt;
      if(line.startsWith("type:"))      codes[total_codes].type = txt;
      if(line.startsWith("protocol:"))  codes[total_codes].protocol = txt;
      if(line.startsWith("address:"))   codes[total_codes].address = txt;
      if(line.startsWith("frequency:")) codes[total_codes].frequency = txt.toInt();
      //if(line.startsWith("duty_cycle:")) codes[total_codes].duty_cycle = txt.toFloat();
      if(line.startsWith("command:")) { codes[total_codes].command = txt; total_codes++; }
      if(line.startsWith("data:")) {    codes[total_codes].data = txt;  total_codes++; }
    }
    options = { };
    bool exit = false;
    for(int i=0; i<total_codes; i++) {
      if(codes[i].type=="raw")        options.push_back({ codes[i].name.c_str(), [=](){ sendRawCommand(codes[i].frequency, codes[i].data); }}); 
      if(codes[i].protocol=="NECext") options.push_back({ codes[i].name.c_str(), [=](){ sendNECextCommand(codes[i].address, codes[i].command); }});
      if(codes[i].protocol=="NEC")    options.push_back({ codes[i].name.c_str(), [=](){ sendNECCommand(codes[i].address, codes[i].command); }});
      if(codes[i].protocol=="RC5")    options.push_back({ codes[i].name.c_str(), [=](){ sendRC5Command(codes[i].address, codes[i].command); }});
      if(codes[i].protocol.startsWith("Samsung")) options.push_back({ codes[i].name.c_str(), [=](){ sendSamsungCommand(codes[i].address, codes[i].command); }});
      if(codes[i].protocol=="SIRC")   options.push_back({ codes[i].name.c_str(), [=](){ sendSonyCommand(codes[i].address, codes[i].command); }});
    }
    options.push_back({ "Main Menu" , [&](){ exit=true; }});
    databaseFile.close();

    digitalWrite(LED, LED_OFF);
    while (1) {
      delay(200);
      loopOptions(options);
      if(checkEscPress() || exit) break;
      delay(200);
    }
  } 

  
  else {  // SPAM all codes of the file
    
    int codes_sent=0;
    databaseFile.seek(0); // comes back to first position
    while (databaseFile.available()) {
      line = databaseFile.readStringUntil('\n');
      if(line.startsWith("type:")) total_codes++;
    }
    
    Serial.printf("\nStarted SPAM all codes with: %d codes", total_codes);
    databaseFile.seek(0); // comes back to first position
    while (databaseFile.available()) {
      progressHandler(codes_sent,total_codes);
      line = databaseFile.readStringUntil('\n');
      if (line.startsWith("type:")) {
        codes_sent++;
        String type = line.substring(5);
        type.trim();
        Serial.println("Type: "+type);
        if (type == "raw") {
          Serial.println("RAW");
          int frequency = 0;
          String rawData = "";
          while (databaseFile.available()) {
            line = databaseFile.readStringUntil('\n');
            if (line.startsWith("frequency:")) {
              String frequencyString = line.substring(10);
              frequencyString.trim();
              frequency = frequencyString.toInt();
            } else if (line.startsWith("data:")) {
              rawData = line.substring(5);
              rawData.trim();
            } else if (line.indexOf("#") != -1) {
              Serial.println("Frequency: "+frequency);
              Serial.println("RawData: "+rawData);
              sendRawCommand(frequency, rawData);
              rawData = "";
              frequency = 0;
              type = "";
              line = "";
              break;
            }
          }
        } else if (type == "parsed") {
          String protocol = "";
          String address = "";
          String command = "";
          Serial.println("PARSED");
          while (databaseFile.available()) {
            line = databaseFile.readStringUntil('\n');
            if (line.startsWith("protocol:")) {
              protocol = line.substring(9);
              protocol.trim();
              Serial.println("Protocol: "+protocol);
            } else if (line.startsWith("address:")) {
              address = line.substring(8);
              address.trim();
              Serial.println("Address: "+address);
            } else if (line.startsWith("command:")) {
              command = line.substring(8);
              command.trim();
              Serial.println("Command: "+command);
            } else if (line.indexOf("#") != -1) {
              if (protocol == "NECext") {
                sendNECextCommand(address, command);
              } else if (protocol == "NEC") {
                sendNECCommand(address, command);
              } else if (protocol == "RC5") {
                sendRC5Command(address, command);
              } else if (protocol.startsWith("Samsung")) {
                sendSamsungCommand(address, command);
              } else if (protocol.startsWith("SIRC")) {
                sendSonyCommand(address, command);
              }
              protocol = "";
              address = "";
              command = "";
              type = "";
              line = "";
              break;
            }
          }
        }
      }
      
    }
    databaseFile.close();
    Serial.println("closed");
    Serial.println("EXTRA finished");
  }
  resetCodesArray();
  digitalWrite(LED, LED_OFF);
}

//IR commands
void sendNECCommand(String address, String command) {
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint32_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint32_t commandValue = strtoul(command.c_str(), nullptr, 16);
  irsend.sendNEC(addressValue, commandValue, 32);
  Serial.println("Sent1");
}

void sendNECextCommand(String address, String command) {
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint32_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint32_t commandValue = strtoul(command.c_str(), nullptr, 16);
  irsend.sendNEC(addressValue, commandValue, 32);
  Serial.println("Sent2");
}

void sendRC5Command(String address, String command) {
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint32_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint32_t commandValue = strtoul(command.c_str(), nullptr, 16);
  irsend.sendRC5(addressValue, commandValue, 12);
  Serial.println("Sent3");
}

void sendSamsungCommand(String address, String command) {
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint64_t data = ((uint64_t)strtoul(address.c_str(), nullptr, 16) << 32) | strtoul(command.c_str(), nullptr, 16);
  irsend.sendSamsung36(data, 36);
  Serial.println("Sent4");
}

void sendSonyCommand(String address, String command) {
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint16_t data = (uint16_t)strtoul(command.c_str(), nullptr, 16);
  uint16_t addressValue = (uint16_t)strtoul(address.c_str(), nullptr, 16);
  irsend.sendSony(addressValue, data);
  Serial.println("Sent5");
}

void sendRawCommand(int frequency, String rawData) {
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint16_t dataBuffer[IR_DATA_BUFFER_SIZE];
  int count = 0;

  // Parse raw data string
  while (rawData.length() > 0 && count < IR_DATA_BUFFER_SIZE) {
    int delimiterIndex = rawData.indexOf(' ');
    if (delimiterIndex == -1) {
      delimiterIndex = rawData.length();
    }
    String dataChunk = rawData.substring(0, delimiterIndex);
    rawData.remove(0, delimiterIndex + 1);
    dataBuffer[count++] = dataChunk.toInt();
  }

  Serial.println("Parsing raw data complete.");

  // Send raw command
  irsend.sendRaw(dataBuffer, count, frequency);

  Serial.println("Sent6");
}
