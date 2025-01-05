#include "custom_ir.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/settings.h"
#include "modules/rf/rf.h" //for hexCharToDecimal()
#include "TV-B-Gone.h" // for checkIrTxPin()
#include <IRutils.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Custom IR

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

    loopOptions(options);
    return(selected_code);
}


bool txIrFile(FS *fs, String filepath) {
  // SPAM all codes of the file

  int total_codes = 0;
  String line;

  File databaseFile = fs->open(filepath, FILE_READ);

  pinMode(bruceConfig.irTx, OUTPUT);
  //digitalWrite(bruceConfig.irTx, LED_ON);

  if (!databaseFile) {
    Serial.println("Failed to open database file.");
    displayError("Fail to open file");
    delay(2000);
    return false;
  }
  Serial.println("Opened database file.");

  bool endingEarly;
  int codes_sent=0;
  uint16_t frequency = 0;
  String rawData = "";
  String protocol = "";
  String address = "";
  String command = "";
  String value = "";
  String bits = "32";

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
          } else if (line.startsWith("value:") || line.startsWith("state:")) {
            value = line.substring(6);
            value.trim();
            Serial.println("Value: "+value);
          } else if (line.startsWith("bits:")) {
            bits = line.substring(strlen("bits:"));
            bits.trim();
            Serial.println("bits: "+bits);
          } else if (line.indexOf("#") != -1) {  // TODO: also detect EOF
            // https://developer.flipper.net/flipperzero/doxygen/infrared_file_format.html
            if (protocol=="NEC") {
              sendNECCommand(address, command);
            } else if (protocol=="NECext") {
              sendNECextCommand(address, command);
            } else if (protocol=="RC5"||protocol=="RC5X") {
              sendRC5Command(address, command);
            } else if (protocol=="RC6") {
              sendRC6Command(address, command);
            } else if (protocol=="Samsung32") {
              sendSamsungCommand(address, command);
            } else if (protocol.startsWith("SIRC")) {
              sendSonyCommand(address, command);
            } else if (protocol=="Kaseikyo"||protocol=="Panasonic") {
              sendPanasonicCommand(address, command);
            } else if (protocol!="" && value!="") {
              sendDecodedCommand(protocol, value, bits);
            }
            protocol = "";
            address = "";
            command = "";
            protocol = "";
            value = "";
            type = "";
            line = "";
            break;
          }
        }
      }
    }
    // if user is pushing (holding down) TRIGGER button, stop transmission early
    if (check(SelPress)) // Pause TV-B-Gone
    {
      while (check(SelPress)) yield();
      displayTextLine("Paused");

      while (!check(SelPress)){ // If Presses Select again, continues
        if(check(EscPress)) {
          endingEarly= true;
          break;
        }
      }
      while (check(SelPress)){
        yield();
      }
      if (endingEarly) break; // Cancels  custom IR Spam
      displayTextLine("Running, Wait");
    }
  } // end while file has lines to process
  databaseFile.close();
  Serial.println("closed");
  Serial.println("EXTRA finished");

  resetCodesArray();
  digitalWrite(bruceConfig.irTx, LED_OFF);
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

  loopOptions(options);


  if(fs == NULL) {  // recent menu was selected
    if(selected_code.filepath!="") { // a code was selected, switch on code type
      // https://developer.flipper.net/flipperzero/doxygen/infrared_file_format.html
      if(selected_code.type=="raw")  sendRawCommand(selected_code.frequency, selected_code.data);
      else if(selected_code.protocol=="NEC") sendNECCommand(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="NECext") sendNECextCommand(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="RC5"||selected_code.protocol=="RC5X") sendRC5Command(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="RC6") sendRC6Command(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="Samsung32") sendSamsungCommand(selected_code.address, selected_code.command);
      else if(selected_code.protocol.startsWith("SIRC")) sendSonyCommand(selected_code.address, selected_code.command);
      else if(selected_code.protocol=="Kaseikyo"||selected_code.protocol=="Panasonic") sendPanasonicCommand(selected_code.address, selected_code.command);
      else if(selected_code.protocol!="") sendDecodedCommand(selected_code.protocol, selected_code.data);
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

  loopOptions(options);


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

  pinMode(bruceConfig.irTx, OUTPUT);
  //digitalWrite(bruceConfig.irTx, LED_ON);

  // Mode to choose and send command by command limitted to 50 commands
  String line;
  String txt;
  while (databaseFile.available() && total_codes<50) {
    line = databaseFile.readStringUntil('\n');
    txt=line.substring(line.indexOf(":") + 1);
    txt.trim();
    if(line.startsWith("name:"))      { 
      // in case that the separation between codes are not made by "#" line
      if(codes[total_codes].name!="") total_codes++; 
      // save signal name
      codes[total_codes].name = txt; 
      codes[total_codes].filepath = txt + " " + filepath.substring( 1 + filepath.lastIndexOf("/") ); 
    }
    if(line.startsWith("type:"))      codes[total_codes].type = txt;
    if(line.startsWith("protocol:"))  codes[total_codes].protocol = txt;
    if(line.startsWith("address:"))   codes[total_codes].address = txt;
    if(line.startsWith("frequency:")) codes[total_codes].frequency = txt.toInt();
    if(line.startsWith("command:"))   { codes[total_codes].command = txt; }
    if(line.startsWith("data:") || line.startsWith("value:") || line.startsWith("state:")) { codes[total_codes].data = txt; }
    // if there are a line with "#", and the code name isnt't "" (there are a signal saved), go to next signal
    if(line.startsWith("#") && codes[total_codes].name!="") total_codes++;
    //if(line.startsWith("duty_cycle:")) codes[total_codes].duty_cycle = txt.toFloat();
  }
  options = { };
  bool exit = false;
  for(int i=0; i<=total_codes; i++) {
    // https://developer.flipper.net/flipperzero/doxygen/infrared_file_format.html
    if(codes[i].type=="raw")        options.push_back({ codes[i].name.c_str(), [=](){ sendRawCommand(codes[i].frequency, codes[i].data); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol=="NEC")    options.push_back({ codes[i].name.c_str(), [=](){ sendNECCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol=="NECext")    options.push_back({ codes[i].name.c_str(), [=](){ sendNECextCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol=="RC5"||codes[i].protocol=="RC5X")    options.push_back({ codes[i].name.c_str(), [=](){ sendRC5Command(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol=="RC6")    options.push_back({ codes[i].name.c_str(), [=](){ sendRC6Command(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol=="Samsung32") options.push_back({ codes[i].name.c_str(), [=](){ sendSamsungCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol.startsWith("SIRC"))   options.push_back({ codes[i].name.c_str(), [=](){ sendSonyCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol=="Kaseikyo"||codes[i].protocol=="Panasonic")   options.push_back({ codes[i].name.c_str(), [=](){ sendPanasonicCommand(codes[i].address, codes[i].command); addToRecentCodes(codes[i]); }});
    else if(codes[i].protocol!="" && codes[i].data!="")   options.push_back({ codes[i].name.c_str(), [=](){ sendDecodedCommand(codes[i].protocol, codes[i].data); addToRecentCodes(codes[i]); }});
  }
  options.push_back({ "Main Menu" , [&](){ exit=true; }});
  databaseFile.close();

  digitalWrite(bruceConfig.irTx, LED_OFF);
  int idx=0;
  while (1) {
    idx=loopOptions(options,idx);
    if(check(EscPress) || exit) break;

  }
}  // end of otherIRcodes


// IR commands
void sendNECCommand(String address, String command) {
  IRsend irsend(bruceConfig.irTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  uint16_t addressValue = strtoul(address.substring(0,2).c_str(), nullptr, 16);
  uint16_t commandValue = strtoul(command.substring(0,2).c_str(), nullptr, 16);
  uint64_t data = irsend.encodeNEC(addressValue, commandValue);
  irsend.sendNEC(data, 32);
  Serial.println("Sent NEC Command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}

void sendNECextCommand(String address, String command) {
  IRsend irsend(bruceConfig.irTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  
  uint8_t first_zero_byte_pos = address.indexOf("00", 2);
  if(first_zero_byte_pos!=-1) address = address.substring(0, first_zero_byte_pos);
  first_zero_byte_pos = command.indexOf("00", 2);
  if(first_zero_byte_pos!=-1) command = command.substring(0, first_zero_byte_pos);
  
  address.replace(" ", "");
  command.replace(" ", "");

  uint16_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint16_t commandValue = strtoul(command.c_str(), nullptr, 16);

  // Invert Endianness
  uint16_t newAddress = (addressValue >> 8) | (addressValue << 8);
  uint16_t newCommand = (commandValue >> 8) | (commandValue << 8);

  // NEC protocol bit order is LSB first
  uint16_t lsbAddress = reverseBits(newAddress, 16);
  uint16_t lsbCommand = reverseBits(newCommand, 16);
  
  uint32_t data = ((uint32_t)lsbAddress << 16) | lsbCommand;
  irsend.sendNEC(data, 32); // Sends MSB first
  Serial.println("Sent NECext Command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}

void sendRC5Command(String address, String command) {
  IRsend irsend(bruceConfig.irTx,true);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  uint8_t addressValue = strtoul(address.substring(0,2).c_str(), nullptr, 16);
  uint8_t commandValue = strtoul(command.substring(0,2).c_str(), nullptr, 16);
  uint16_t data = irsend.encodeRC5(addressValue, commandValue);
  irsend.sendRC5(data, 13);
  Serial.println("Sent RC5 command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}

void sendRC6Command(String address, String command) {
  IRsend irsend(bruceConfig.irTx,true);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  address.replace(" ", "");
  command.replace(" ", "");
  uint32_t addressValue = strtoul(address.substring(0,2).c_str(), nullptr, 16);
  uint32_t commandValue = strtoul(command.substring(0,2).c_str(), nullptr, 16);
  uint64_t data = irsend.encodeRC6(addressValue, commandValue);
  irsend.sendRC6(data, 20);
  Serial.println("Sent RC5 command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}

void sendSamsungCommand(String address, String command) {
  IRsend irsend(bruceConfig.irTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  uint8_t addressValue = strtoul(address.substring(0,2).c_str(), nullptr, 16);
  uint8_t commandValue = strtoul(command.substring(0,2).c_str(), nullptr, 16);
  uint64_t data = irsend.encodeSAMSUNG(addressValue, commandValue);

  irsend.sendSAMSUNG(data, 32);
  Serial.println("Sent Samsung Command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}

void sendSonyCommand(String address, String command) {
  IRsend irsend(bruceConfig.irTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  uint16_t commandValue = strtoul(command.substring(0,2).c_str(), nullptr, 16);
  uint16_t addressValue = strtoul(address.substring(0,2).c_str(), nullptr, 16);
  uint16_t addressValue2 = strtoul(address.substring(3,6).c_str(), nullptr, 16);

  uint16_t nbits = 12; // 12 bits (SIRC)
  if(addressValue2>0) nbits = 20; // 20 bits (SIRC20)
  else if(addressValue>0x1F) nbits = 15; // 15 bits (SIRC15)

  uint32_t data;
  if (nbits == 20) {
    data = irsend.encodeSony(nbits, commandValue, addressValue, addressValue2);
  } else {
    data = irsend.encodeSony(nbits, commandValue, addressValue);
  }

  // 1 initial + 2 repeat
  irsend.sendSony(data, nbits, 2);
  Serial.println("Sent Sony Command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}

void sendPanasonicCommand(String address, String command) {
  IRsend irsend(bruceConfig.irTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  uint8_t first_zero_byte_pos = address.indexOf("00", 2);
  if(first_zero_byte_pos!=-1) address = address.substring(0, first_zero_byte_pos);
  // needs to invert endianess
  // address: "D3 C4 00 00" -> "C4 D3 00 00"
  address = address.substring(3,4) + " " + address.substring(0,1) + " 00 00";
  // command: "02 00 40 64" -> "64 40 00 02"
  command = command.substring(9,10) + " " + command.substring(6,7) + " " + command.substring(3,4) + " " + command.substring(0,1);

  address.replace(" ", "");
  command.replace(" ", "");

  uint16_t addressValue = strtoul(address.c_str(), nullptr, 16);
  uint32_t commandValue = strtoul(command.c_str(), nullptr, 16);
  Serial.println(addressValue);
  Serial.println(commandValue);
  irsend.sendPanasonic(addressValue, commandValue, 48, 10);
  // sendPanasonic(const uint16_t address, const uint32_t data, const uint16_t nbits = kPanasonicBits, const uint16_t repeat = kNoRepeat);
  delay(20);
  Serial.println("Sent Panasonic Command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}

bool sendDecodedCommand(String protocol, String value, String bits) {
  // https://github.com/crankyoldgit/IRremoteESP8266/blob/master/examples/SmartIRRepeater/SmartIRRepeater.ino
  
    decode_type_t type = strToDecodeType(protocol.c_str());
  if(type == decode_type_t::UNKNOWN) return false;
  uint16_t nbit_int = bits.toInt();

  IRsend irsend(bruceConfig.irTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  bool success = false;
  displayTextLine("Sending..");

  if(hasACState(type)) {
    // need to send the state (still passed from value)
    uint8_t state[nbit_int / 8] = {0};
    uint16_t state_pos = 0;
    for (uint16_t i = 0; i < value.length(); i += 3) {
        // parse  value -> state
        uint8_t highNibble = hexCharToDecimal(value[i]);
        uint8_t lowNibble = hexCharToDecimal(value[i + 1]);
        state[state_pos] = (highNibble << 4) | lowNibble;
        state_pos++;
    }
    //success = irsend.send(type, state, nbit_int / 8);
    success = irsend.send(type, state, state_pos);  // safer

  } else {
    value.replace(" ", "");
    uint64_t value_int = strtoull(value.c_str(), nullptr, 16);

    success = irsend.send(type, value_int, nbit_int);  // bool send(const decode_type_t type, const uint64_t data, const uint16_t nbits, const uint16_t repeat = kNoRepeat);
  }

  delay(20);
  Serial.println("Sent Decoded Command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
  return success;
}

void sendRawCommand(uint16_t frequency, String rawData) {
  IRsend irsend(bruceConfig.irTx);  // Set the GPIO to be used to sending the message.
  irsend.begin();
  displayTextLine("Sending..");
  uint16_t dataBuffer[SAFE_STACK_BUFFER_SIZE/2]; // MEMO: stack overflow with full buffer size
  uint16_t count = 0;

  // Parse raw data string
  while (rawData.length() > 0 && count < SAFE_STACK_BUFFER_SIZE/2) {
    int delimiterIndex = rawData.indexOf(' ');
    if (delimiterIndex == -1) {
      delimiterIndex = rawData.length();
    }
    String dataChunk = rawData.substring(0, delimiterIndex);
    rawData.remove(0, delimiterIndex + 1);
    dataBuffer[count++] = (dataChunk.toInt());
  }

  Serial.println("Parsing raw data complete.");
  //Serial.println(count);
  //Serial.println(dataBuffer[count-1]);
  //Serial.println(dataBuffer[0]);

  // Send raw command
  irsend.sendRaw(dataBuffer, count, frequency);

  Serial.println("Sent Raw command");
  digitalWrite(bruceConfig.irTx, LED_OFF);
}
