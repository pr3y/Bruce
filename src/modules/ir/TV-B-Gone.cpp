/*
Last Updated: 30 Mar. 2018
By Anton Grimpelhuber (anton.grimpelhuber@gmail.com)

------------------------------------------------------------
LICENSE:
------------------------------------------------------------
Distributed under Creative Commons 2.5 -- Attribution & Share Alike

*/

#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/settings.h"
#include "WORLD_IR_CODES.h"
#include "TV-B-Gone.h"

char16_t FGCOLOR;

/*
Last Updated: 30 Mar. 2018
By Anton Grimpelhuber (anton.grimpelhuber@gmail.com)
*/

// The TV-B-Gone for Arduino can use either the EU (European Union) or the NA (North America) database of POWER CODES
// EU is for Europe, Middle East, Australia, New Zealand, and some countries in Africa and South America
// NA is for North America, Asia, and the rest of the world not covered by EU

// Two regions!
#define NA 1 //set by a HIGH on REGIONSWITCH pin
#define EU 0 //set by a LOW on REGIONSWITCH pin

// Lets us calculate the size of the NA/EU databases
#define NUM_ELEM(x) (sizeof (x) / sizeof (*(x)));

// set define to 0 to turn off debug output
#define DEBUG 0
#define DEBUGP(x) if (DEBUG == 1) { x ; }

// Shortcut to insert single, non-optimized-out nop
#define NOPP __asm__ __volatile__ ("nop")

// Not used any more on esp8266, so don't bother
// Tweak this if  cessary to change timing
// -for 8MHz Arduinos, a good starting value is 11
// -for 16MHz Arduinos, a good starting value is 25
#define DELAY_CNT 25

void xmitCodeElement(uint16_t ontime, uint16_t offtime, uint8_t PWM_code );
void quickflashLEDx( uint8_t x );
void delay_ten_us(uint16_t us);
void quickflashLED( void );
uint8_t read_bits(uint8_t count);
uint16_t rawData[300];
#define MAX_WAIT_TIME 65535 //tens of us (ie: 655.350ms)
extern const IrCode* const NApowerCodes[];
extern const IrCode* const EUpowerCodes[];
uint8_t num_NAcodes = NUM_ELEM(NApowerCodes);
uint8_t num_EUcodes = NUM_ELEM(EUpowerCodes);
uint8_t bitsleft_r = 0;
uint8_t bits_r = 0;
uint8_t code_ptr;
volatile const IrCode * powerCode;
uint8_t read_bits(uint8_t count)
{
  uint8_t i;
  uint8_t tmp = 0;
  for (i = 0; i < count; i++) {
    if (bitsleft_r == 0) {
      bits_r = powerCode->codes[code_ptr++];
      bitsleft_r = 8;
    }
    bitsleft_r--;
    tmp |= (((bits_r >> (bitsleft_r)) & 1) << (count - 1 - i));
  }
  return tmp;
}
uint16_t ontime, offtime;
uint8_t i, num_codes;
uint8_t region;

void delay_ten_us(uint16_t us) {
  uint8_t timer;
  while (us != 0) {
    for (timer = 0; timer <= DELAY_CNT; timer++) {
      NOPP;
      NOPP;
    }
    NOPP;
    us--;
  }
}

void quickflashLED( void ) {
#if defined(M5LED)
  digitalWrite(IRLED, M5LED_ON);
  delay_ten_us(3000);   // 30 ms ON-time delay
  digitalWrite(IRLED, M5LED_OFF);
#endif
}

void quickflashLEDx( uint8_t x ) {
  quickflashLED();
  while (--x) {
    delay_ten_us(25000);     // 250 ms OFF-time delay between flashes
    quickflashLED();
  }
}

void checkIrTxPin(){
  const std::vector<std::pair<std::string, int>> pins = IR_TX_PINS;
  int count=0;
  for (auto pin : pins) {
    if(pin.second==IrTx) count++; 
  }
  if(count>0) return;
  else gsetIrTxPin(true);
}

void StartTvBGone() {
  Serial.begin(115200);
  checkIrTxPin();
  IRsend irsend(IrTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  pinMode(IrTx, OUTPUT);

  // determine region
  options = {
      {"Region NA", [&]() { region = NA; }},
      {"Region EU", [&]() { region = EU; }},
      {"Main Menu", [=]() { backToMenu(); }},
  };
  delay(300);
  loopOptions(options);
  delay(300);

  if (!returnToMenu) {
      if (region) num_codes=num_NAcodes;
      else num_codes=num_EUcodes;

      bool endingEarly = false; //will be set to true if the user presses the button during code-sending

      checkSelPress();
      for (i=0 ; i<num_codes; i++) {
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
          offtime = powerCode->times[ti];  // read word 1 - ontime
          ontime = powerCode->times[ti + 1]; // read word 2 - offtime

          rawData[k*2] = offtime * 10;
          rawData[(k*2)+1] = ontime * 10;
        }
        progressHandler(i, num_codes);
        irsend.sendRaw(rawData, (numpairs*2) , freq);
        bitsleft_r=0;
        delay_ten_us(20500);

        // if user is pushing (holding down) TRIGGER button, stop transmission early
        if (checkSelPress()) // Pause TV-B-Gone
        {
          while (checkSelPress()) yield();
          displayRedStripe("Paused", TFT_WHITE, BGCOLOR);

          while (!checkSelPress()){ // If Presses Select again, continues
            if(checkEscPress()) {
              endingEarly= true;
              break;
            }
          }
          while (checkSelPress()){
            yield();
          }
          if (endingEarly) break; // Cancels  TV-B-Gone
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
        delay(2000);
      }

      //turnoff LED
      digitalWrite(IrTx,LED_OFF);
   }
} //end of sendAllCodes


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom IR
#define IR_DATA_BUFFER_SIZE 300

struct Codes {
  String name="";
  String type="";
  String protocol="";
  String address="";
  String command="";
  uint32_t frequency=0;
  //float duty_cycle;
  String data="";
  String filepath="";
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


Codes recent_ircodes[16];
int recent_ircodes_last_used = 0;  // TODO: save/load in EEPROM

void addToRecentCodes(struct Codes ircode)  {
    // copy rfcode -> recent_ircodes[recent_ircodes_last_used]
    recent_ircodes[recent_ircodes_last_used] = ircode;
    recent_ircodes_last_used += 1;
    if(recent_ircodes_last_used == 16) recent_ircodes_last_used  = 0; // cycle
}

struct Codes selectRecentIrMenu() {
    // show menu with filenames
    checkIrTxPin();
    options = { };
    bool exit = false;
    struct Codes selected_code;
    for(int i=0; i<16; i++) {
        if(recent_ircodes[i].filepath=="") continue; // not inited
        // else
        options.push_back({ recent_ircodes[i].filepath.c_str(), [i, &selected_code](){ selected_code = recent_ircodes[i]; }});
    }
    options.push_back({ "Main Menu" , [&](){ exit=true; }});    
    delay(200);
    loopOptions(options);
    return(selected_code);
}


bool txIrFile(FS *fs, String filepath) {
  // SPAM all codes of the file

  int total_codes = 0;
  String line;
  
  File databaseFile = fs->open(filepath, FILE_READ);

  pinMode(IrTx, OUTPUT);
  //digitalWrite(IrTx, LED_ON);

  if (!databaseFile) {
    Serial.println("Failed to open database file.");
    displayError("Fail to open file");
    delay(2000);
    return false;
  }
  Serial.println("Opened database file.");
  
  bool endingEarly;
  int codes_sent=0;
  int frequency = 0;
  String rawData = "";
  String protocol = "";
  String address = "";
  String command = "";
    
  databaseFile.seek(0); // comes back to first position
  
  // count the number of codes to replay
  while (databaseFile.available()) {
    line = databaseFile.readStringUntil('\n');
    if(line.startsWith("type:")) total_codes++;
  }

  Serial.printf("\nStarted SPAM all codes with: %d codes", total_codes);
  // comes back to first position, beggining of the file
  databaseFile.seek(0);
  while (databaseFile.available()) {
    progressHandler(codes_sent,total_codes);
    line = databaseFile.readStringUntil('\n');
    if (line.endsWith("\r")) line.remove(line.length() - 1);

    if (line.startsWith("type:")) {
      codes_sent++;
      String type = line.substring(5);
      type.trim();
      Serial.println("Type: "+type);
      if (type == "raw") {
        Serial.println("RAW code");
        while (databaseFile.available()) {
          line = databaseFile.readStringUntil('\n');
          if (line.endsWith("\r")) line.remove(line.length() - 1);

          if (line.startsWith("frequency:")) {
            line = line.substring(10);
            line.trim();
            frequency = line.toInt();
            Serial.println("Frequency: " + String(frequency));
          } else if (line.startsWith("data:")) {
            rawData = line.substring(5);
            rawData.trim();
            Serial.println("RawData: "+rawData);
          } else if ((frequency != 0 && rawData != "") || line.startsWith("#")) {
            sendRawCommand(frequency, rawData);
            rawData = "";
            frequency = 0;
            type = "";
            line = "";
            break;
          }
        }
      } else if (type == "parsed") {
        Serial.println("PARSED");
        while (databaseFile.available()) {
          line = databaseFile.readStringUntil('\n');
          if (line.endsWith("\r")) line.remove(line.length() - 1);

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
          } else if (line.indexOf("#") != -1) {  // TODO: also detect EOF
            if (protocol.startsWith("NEC")) {
              sendNECCommand(address, command);
            } else if (protocol.startsWith("RC5")) {
              sendRC5Command(address, command);
            } else if (protocol.startsWith("RC6")) {
              sendRC6Command(address, command);
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
    // if user is pushing (holding down) TRIGGER button, stop transmission early
    if (checkSelPress()) // Pause TV-B-Gone
    {
      while (checkSelPress()) yield();
      displayRedStripe("Paused", TFT_WHITE, BGCOLOR);

      while (!checkSelPress()){ // If Presses Select again, continues
        if(checkEscPress()) {
          endingEarly= true;
          break;
        }
      }
      while (checkSelPress()){
        yield();
      }
      if (endingEarly) break; // Cancels  custom IR Spam
      displayRedStripe("Running, Wait", TFT_WHITE, FGCOLOR);
    }
  } // end while file has lines to process
  databaseFile.close();
  Serial.println("closed");
  Serial.println("EXTRA finished");

  resetCodesArray();
  digitalWrite(IrTx, LED_OFF);
  return true;
}


void otherIRcodes() {
  checkIrTxPin();
  resetCodesArray();
  int total_codes = 0;
  String filepath;
  File databaseFile;
  FS *fs = NULL;
  struct Codes selected_code;
  
  returnToMenu = true;  // make sure menu is redrawn when quitting in any point
  
  options = {
      {"Recent", [&]()  { selected_code = selectRecentIrMenu(); }},
      {"LittleFS", [&]()   { fs=&LittleFS; }},
  };
  if(setupSdCard()) options.push_back({"SD Card", [&]()  { fs=&SD; }});    

  delay(200);
  loopOptions(options);
  delay(200);
    
  if(fs == NULL) {  // recent menu was selected
    if(selected_code.filepath!="") { // a code was selected, switch on code type
      if(selected_code.type=="raw")  sendRawCommand(selected_code.frequency, selected_code.data);
      else if(selected_code.protocol=="NEC") sendNECCommand(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="RC5") sendRC5Command(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="RC6") sendRC6Command(selected_code.address, selected_code.command);
      else if(selected_code.protocol.startsWith("Samsung")) sendSamsungCommand(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="SIRC") sendSonyCommand(selected_code.address, selected_code.command);
    }
    return;
    // no need to proceed, go back
  }
    
  // select a file to tx
  filepath = loopSD(*fs, true, "IR");
  if(filepath=="") return;  //  cancelled
  // else
  
  // select mode
  bool mode_cmd=true;
  options = {
    {"Choose cmd", [&]()  { mode_cmd=true; }},
    {"Spam all", [&]()    { mode_cmd=false; }},
  };
  delay(200);
  loopOptions(options);
  delay(200);

  if(mode_cmd == false) {
    // Spam all selected
    txIrFile(fs, filepath);
    return;
  }
  
  // else continue and try to parse the file
  
  databaseFile = fs->open(filepath, FILE_READ);
  drawMainBorder();

  if (!databaseFile) {
    Serial.println("Failed to open database file.");
    //displayError("Fail to open file");
    //delay(2000);
    return;
  }
  Serial.println("Opened database file.");
  
  pinMode(IrTx, OUTPUT);
  //digitalWrite(IrTx, LED_ON);

  // Mode to choose and send command by command limitted to 50 commands  
  String line;
  String txt;
  while (databaseFile.available() && total_codes<50) {
    line = databaseFile.readStringUntil('\n');
    txt=line.substring(line.indexOf(":") + 1);
    txt.trim();
    if(line.startsWith("name:"))      { codes[total_codes].name = txt; codes[total_codes].filepath = txt + " " + filepath.substring( 1 + filepath.lastIndexOf("/") ) ;}
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
  for(int i=0; i<=total_codes; i++) {
    if(codes[i].type=="raw")        options.push_back({ codes[i].name.c_str(), [=](){ sendRawCommand(codes[i].frequency, codes[i].data); addToRecentCodes(codes[i]); }});
    if(codes[i].protocol.startsWith("NEC"))    options.push_back({ codes[i].name.c_str(), [=](){ sendNECCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    if(codes[i].protocol.startsWith("RC5"))    options.push_back({ codes[i].name.c_str(), [=](){ sendRC5Command(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    if(codes[i].protocol.startsWith("RC6"))    options.push_back({ codes[i].name.c_str(), [=](){ sendRC6Command(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    if(codes[i].protocol.startsWith("Samsung")) options.push_back({ codes[i].name.c_str(), [=](){ sendSamsungCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    if(codes[i].protocol=="SIRC")   options.push_back({ codes[i].name.c_str(), [=](){ sendSonyCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
  }
  options.push_back({ "Main Menu" , [&](){ exit=true; }});
  databaseFile.close();

  digitalWrite(IrTx, LED_OFF);
  int idx=0;
  while (1) {
    delay(200);
    idx=loopOptions(options,idx);
    if(checkEscPress() || exit) break;
    delay(200);
  }
}  // end of otherIRcodes


//IR commands
void sendNECCommand(String address, String command) {
  IRsend irsend(IrTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint8_t first_zero_byte_pos = address.indexOf("00", 2);
  if(first_zero_byte_pos!=-1) address = address.substring(0, first_zero_byte_pos);
  first_zero_byte_pos = command.indexOf("00", 2);
  if(first_zero_byte_pos!=-1) command = command.substring(0, first_zero_byte_pos);
  uint16_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint16_t commandValue = strtoul(command.c_str(), nullptr, 16);
  uint64_t data = irsend.encodeNEC(addressValue, commandValue);
  irsend.sendNEC(data, 32, 10);
  Serial.println("Sent NEC Command");
  digitalWrite(IrTx, LED_OFF);
}


void sendRC5Command(String address, String command) {
  IRsend irsend(IrTx,true);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint8_t addressValue = strtoul(address.substring(0,2).c_str(), nullptr, 16);
  uint8_t commandValue = strtoul(command.substring(0,2).c_str(), nullptr, 16);
  uint16_t data = irsend.encodeRC5(addressValue, commandValue);
  irsend.sendRC5(data, 13, 10);
  Serial.println("Sent RC5 command");
  digitalWrite(IrTx, LED_OFF);
}

void sendRC6Command(String address, String command) {
  IRsend irsend(IrTx,true);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint32_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint32_t commandValue = strtoul(command.c_str(), nullptr, 16);
  uint64_t data = irsend.encodeRC6(addressValue, commandValue);
  irsend.sendRC6(data,20, 10);
  Serial.println("Sent RC5 command");
  digitalWrite(IrTx, LED_OFF);
}

void sendSamsungCommand(String address, String command) {
  IRsend irsend(IrTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  //uint64_t data = ((uint64_t)strtoul(address.c_str(), nullptr, 16) << 32) | strtoul(command.c_str(), nullptr, 16);
  uint32_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint32_t commandValue = strtoul(command.c_str(), nullptr, 16);  
  uint64_t data = irsend.encodeSAMSUNG(addressValue, commandValue);
  irsend.sendSAMSUNG(data, 32, 10);
  //delay(20);
  //irsend.sendSamsung36(data, 36, 10);
  Serial.println("Sent Samsung Command");
  digitalWrite(IrTx, LED_OFF);
}

void sendSonyCommand(String address, String command) {
  IRsend irsend(IrTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
  uint16_t commandValue = strtoul(command.substring(0,2).c_str(), nullptr, 16);
  uint16_t addressValue = strtoul(address.substring(0,2).c_str(), nullptr, 16);
  uint16_t addressValue2 = strtoul(address.substring(3,6).c_str(), nullptr, 16);
  uint16_t nbits = 12;
  if(addressValue2>0) nbits = 20;
  else if(addressValue>=0x80) nbits = 15;
  uint32_t data = irsend.encodeSony(nbits,commandValue,addressValue);
  irsend.sendSony(data,20,10);
  Serial.println("Sent Sony Command");
  digitalWrite(IrTx, LED_OFF);
}

void sendRawCommand(int frequency, String rawData) {
  IRsend irsend(IrTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
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

  Serial.println("Sent Raw command");
  digitalWrite(IrTx, LED_OFF);
}
