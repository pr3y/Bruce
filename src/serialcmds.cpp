
#include "serialcmds.h"
#include "globals.h"
#include <IRsend.h>
#include <string>
#include "TV-B-Gone.h"


void handleSerialCommands() {
  String cmd_str;
  const int MIN_CMD_LEN = 1;
  
    if (Serial.available() >= MIN_CMD_LEN) {    
      cmd_str = Serial.readStringUntil('\n');
    } else {
      // try again on next iteration
      return;
    }

  log_printf("received: %s\n", cmd_str.c_str());
  
  // https://docs.flipper.net/development/cli#0Z9fs
  cmd_str = cmd_str.toLowerCase();

  if(cmd_str.startsWith("ir tx ")){  // ir tx <protocol> <address> <command>
    // <protocol>: NEC, NECext, NEC42, NEC42ext, Samsung32, RC6, RC5, RC5X, SIRC, SIRC15, SIRC20, Kaseikyo, RCA
    // <address> and <command> must be in hex format
    // e.g. ir tx NEC 04000000 08000000

     if(cmd_str.startsWith("ir tx nec ")){
       String address = cmd_str.substring(10, 8);
       String command = cmd_str.substring(19, 8);
       // check if valid address, command
       sendNECCommand(address, command);
      log_printf("address: %x\tcommand=%x\n", address, command);

      }

  }
  
  //if(cmd_str.startsWith("ir tx raw")){
  
  

}
 
