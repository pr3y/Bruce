
#include "serialcmds.h"
#include "globals.h"
#include <IRsend.h>
#include <string>
#include "TV-B-Gone.h"
#include "cJSON.h"
#include <inttypes.h> // for PRIu64


void SerialPrintHexString(uint64_t val) {
  char s[18] = {0};
  //snprintf(s, 10, "%x", val);
  //snprintf(s, sizeof(s), "%" PRIx64, val);
  snprintf(s, sizeof(s), "%llx", val);
  Serial.println(s);
}

void handleSerialCommands() {
  String cmd_str;
  
  /*
  	if (Serial.available() >= MIN_CMD_LEN ) {
      size_t len = Serial.available();
      char sbuf[len] = {0};
      Serial.readBytes(sbuf, len);
      Serial.print("received:");
      Serial.println(sbuf);
      //log_d(sbuf);
      cmd_str = String(sbuf);
	} else  {
    //Serial.println("nothing received");
    //log_d("nothing received");
    return;
  }*/
  
    if (Serial.available() >= 1) {    
      cmd_str = Serial.readStringUntil('\n');
    } else {
      // try again on next iteration
      return;
    }  

  //log_d(cmd_str.c_str());
  cmd_str.trim();
  cmd_str.toLowerCase();  // case-insensitive matching
  
  //  TODO: more commands https://docs.flipper.net/development/cli#0Z9fs

  if(cmd_str.startsWith("ir") ) {

    //IRsend irsend(IrTx);  //inverted = false
    IRsend irsend(IrTx,true);  // Set the GPIO to be used to sending the message.
    irsend.begin();

    // ir tx <protocol> <address> <command>
    // <protocol>: NEC, NECext, NEC42, NEC42ext, Samsung32, RC6, RC5, RC5X, SIRC, SIRC15, SIRC20, Kaseikyo, RCA
    // <address> and <command> must be in hex format
    // e.g. ir tx NEC 04000000 08000000

    /*
    const int ADD_LEN = 8;
    const int CMD_LEN = 8;
    void* sendIrCommandFuncPrt = NULL;
    */


    if(cmd_str.startsWith("ir tx nec ")){
       String address = cmd_str.substring(10, 10+8);
       String command = cmd_str.substring(19, 19+8);
      Serial.println(address+","+command);
      
      //displayRedStripe("Sending..",TFT_WHITE,FGCOLOR);
      uint64_t addressValue = strtoul(address.c_str(), nullptr, 16);
      uint64_t commandValue = strtoul(command.c_str(), nullptr, 16);
      uint64_t data = irsend.encodeNEC(addressValue, commandValue);
      SerialPrintHexString(data);

      irsend.sendNEC(data, 32, 10);
    
      }
      // TODO: more protocols
      //if(cmd_str.startsWith("ir tx raw")){

    if(cmd_str.startsWith("irnec")) {
      // irnec 20DF10EF
      String dataStr = cmd_str.substring(6, 6+8);
      uint64_t data = strtoul(dataStr.c_str(), nullptr, 16);
      SerialPrintHexString(data);
      IRsend irsend(IrTx);  //inverted = false
      irsend.begin();
      irsend.sendNEC(data, 32, 10);

    }
    if(cmd_str.startsWith("irsend")) {
      // tasmota format https://tasmota.github.io/docs/Tasmota-IR/#sending-ir-commands
      // e.g. IRSend {"Protocol":"NEC","Bits":32,"Data":"0x20DF10EF"}
      cJSON *root = cJSON_Parse(cmd_str.c_str() + 6);	
      if (root == NULL) {
        Serial.println("This is NOT json format");
        return;
      }
      int bits = 32;
      const char *dataStr = "";

      cJSON * protocolItem = cJSON_GetObjectItem(root,"protocol");    
      cJSON * dataItem = cJSON_GetObjectItem(root, "data");
      cJSON * bitsItem = cJSON_GetObjectItem(root,"bits");

      //if(protocolItem) ...
      if(bitsItem && cJSON_IsNumber(bitsItem)) bits = bitsItem->valueint;
      if(dataItem && cJSON_IsString(dataItem)) {
        dataStr = dataItem->valuestring;
      } else {
        Serial.println("missing or invalid data to send");
        return;      
      }
      //String dataStr = cmd_str.substring(36, 36+8);
      uint64_t data = strtoul(dataStr, nullptr, 16);
      Serial.println(dataStr);
      SerialPrintHexString(data);
      Serial.println(bits);

      cJSON_Delete(root);

      // sendNEC(uint64_t data, uint16_t nbits, uint16_t repeat) 
      irsend.sendNEC(data, bits, 10);
    }
    digitalWrite(IrTx, LED_OFF);
    //backToMenu();
    return;
  }  // end of ir commands

  Serial.println("unsupported serial command" + cmd_str);


}
 
