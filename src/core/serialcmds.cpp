
#include "serialcmds.h"
#include "globals.h"
#include <IRsend.h>
#include <string>
#include "modules/others/TV-B-Gone.h"
#include "cJSON.h"
#include <inttypes.h> // for PRIu64

#include <ESP8266Audio.h>
#include <ESP8266SAM.h>
#include "sd_functions.h"
#include "settings.h"
#include "display.h"
#include "modules/rf/rf.h"


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

    // ir tx <protocol> <address> <command>
    // <protocol>: NEC, NECext, NEC42, NEC42ext, Samsung32, RC6, RC5, RC5X, SIRC, SIRC15, SIRC20, Kaseikyo, RCA
    // <address> and <command> must be in hex format
    // e.g. ir tx NEC 04000000 08000000

    if(cmd_str.startsWith("ir tx nec ")){
       String address = cmd_str.substring(10, 10+8);
       String command = cmd_str.substring(19, 19+8);
       sendNECCommand(address, command);  // TODO: add arg for displayRedStripe optional
       return;
      }
    if(cmd_str.startsWith("ir tx rc5 ")){
       String address = cmd_str.substring(10, 10+8);
       String command = cmd_str.substring(19, 19+8);
       sendRC5Command(address, command);
       return;
      }
    if(cmd_str.startsWith("ir tx rc6 ")){
       String address = cmd_str.substring(10, 10+8);
       String command = cmd_str.substring(19, 19+8);
       sendRC6Command(address, command);
       return;
      }
      // TODO: more protocols: Samsung32, SIRC
      //if(cmd_str.startsWith("ir tx raw")){
    
    if(cmd_str.startsWith("irsend")) {
      // tasmota json command  https://tasmota.github.io/docs/Tasmota-IR/#sending-ir-commands
      // e.g. IRSend {"Protocol":"NEC","Bits":32,"Data":"0x20DF10EF"}
      // TODO: rewrite using ArduinoJson parser?
      // TODO: decode "data" into "address, command" and use existing "send*Command" funcs
      if(IrTx==0) IrTx = LED;  // quickfix init issue? CARDPUTER is 44

      //IRsend irsend(IrTx);  //inverted = false
      //Serial.println(IrTx);
      IRsend irsend(IrTx,true);  // Set the GPIO to be used to sending the message.
      //IRsend irsend(IrTx);  //inverted = false
      irsend.begin();
      cJSON *root = cJSON_Parse(cmd_str.c_str() + 6);	
      if (root == NULL) {
        Serial.println("This is NOT json format");
        return;
      }
      uint16_t bits = 32; // defaults to 32 bits
      const char *dataStr = "";
      String protocolStr = "nec";  // defaults to NEC protocol

      cJSON * protocolItem = cJSON_GetObjectItem(root,"protocol");    
      cJSON * dataItem = cJSON_GetObjectItem(root, "data");
      cJSON * bitsItem = cJSON_GetObjectItem(root,"bits");

      if(protocolItem && cJSON_IsString(protocolItem)) protocolStr = protocolItem->valuestring;
      if(bitsItem && cJSON_IsNumber(bitsItem)) bits = bitsItem->valueint;
      if(dataItem && cJSON_IsString(dataItem)) {
        dataStr = dataItem->valuestring;
      } else {
        Serial.println("missing or invalid data to send");
        return;      
      }
      //String dataStr = cmd_str.substring(36, 36+8);
      uint64_t data = strtoul(dataStr, nullptr, 16);
      //Serial.println(dataStr);
      //SerialPrintHexString(data);
      //Serial.println(bits);
      //Serial.println(protocolItem->valuestring);
      
      cJSON_Delete(root);
      
      if(protocolStr == "nec"){
        // sendNEC(uint64_t data, uint16_t nbits, uint16_t repeat) 
        irsend.sendNEC(data, bits, 10);
      }
      // TODO: more protocols
    }
    
    // turn off the led
    digitalWrite(IrTx, LED_OFF);
    //backToMenu();
    return;
  }  // end of ir commands
    
  if(cmd_str.startsWith("rf") || cmd_str.startsWith("subghz" )) {
    if(RfTx==0) RfTx=GROVE_SDA; // quick fix
    pinMode(RfTx, OUTPUT);
    //Serial.println(RfTx);
        
    /* WIP:
    if(cmd_str.startsWith("subghz tx")) {
      // flipperzero-like cmd  https://docs.flipper.net/development/cli/#wLVht
      // e.g. subghz tx 0000000000200001 868250000 403 10  // https://forum.flipper.net/t/friedland-libra-48249sl-wireless-doorbell-request/4528/20
      //                {hex_key} {frequency} {te} {count}
    }*/
    if(cmd_str.startsWith("rfsend")) {
      // tasmota json command  https://tasmota.github.io/docs/Tasmota-IR/#sending-ir-commands
      // e.g. RfSend {"Data":"0x447503","Bits":24,"Protocol":1,"Pulse":174,"Repeat":10}  // on
      // e.g. RfSend {"Data":"0x44750C","Bits":24,"Protocol":1,"Pulse":174,"Repeat":10}  // off
    
      cJSON *root = cJSON_Parse(cmd_str.c_str() + 6);	
      if (root == NULL) {
        Serial.println("This is NOT json format");
        return;
      }
      unsigned int bits = 32; // defaults to 32 bits
      const char *dataStr = "";
      int protocol = 1;  // defaults to 1
      int pulse = 0; // 0 leave the library use the default value depending on protocol
      int repeat = 10;
    
      cJSON * protocolItem = cJSON_GetObjectItem(root,"protocol");    
      cJSON * dataItem = cJSON_GetObjectItem(root, "data");
      cJSON * bitsItem = cJSON_GetObjectItem(root,"bits");
      cJSON * pulseItem = cJSON_GetObjectItem(root,"pulse");
      cJSON * repeatItem = cJSON_GetObjectItem(root,"repeat");

      if(protocolItem && cJSON_IsNumber(protocolItem)) protocol = protocolItem->valueint;
      if(bitsItem && cJSON_IsNumber(bitsItem)) bits = bitsItem->valueint;
      if(pulseItem && cJSON_IsNumber(pulseItem)) pulse = pulseItem->valueint;
      if(repeatItem && cJSON_IsNumber(repeatItem)) repeat = repeatItem->valueint;
      if(dataItem && cJSON_IsString(dataItem)) {
        dataStr = dataItem->valuestring;
      } else {
        Serial.println("missing or invalid data to send");
        cJSON_Delete(root);
        return;      
      }
      //String dataStr = cmd_str.substring(36, 36+8);
      uint64_t data = strtoul(dataStr, nullptr, 16);
      //Serial.println(dataStr);
      //SerialPrintHexString(data);
      //Serial.println(bits);
      
      RCSwitch_send(data, bits, pulse, protocol, repeat);
      
      cJSON_Delete(root);
      return;
    }
  }  // endof rf
  
  if(cmd_str.startsWith("music_player " ) || cmd_str.startsWith("ttf" ) ) {
    // TODO: move in audio.cpp module
      AudioOutputI2S *audioout = new AudioOutputI2S();  // https://github.com/earlephilhower/ESP8266Audio/blob/master/src/AudioOutputI2S.cpp#L32
  #ifdef CARDPUTER
      audioout->SetPinout(41, 43, 42);
      // TODO: other pinouts
  #endif
      AudioGenerator* generator = NULL;
      AudioFileSource* source = NULL;
  
      if(cmd_str.startsWith("music_player " ) ) {  // || cmd_str.startsWith("play " )
        String song = cmd_str.substring(13, cmd_str.length());
        if(song.indexOf(":") != -1) {
          // RTTTL player
          // music_player mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6
          // derived from https://github.com/earlephilhower/ESP8266Audio/blob/master/examples/PlayRTTTLToI2SDAC/PlayRTTTLToI2SDAC.ino
          generator = new AudioGeneratorRTTTL();
          source = new AudioFileSourcePROGMEM( song.c_str(), song.length() );
        } else if(song.indexOf(".") != -1) {
          // try to open "song" as a file
          // e.g. music_player audio/Axel-F.txt
          if(!song.startsWith("/")) song = "/" + song;  // add "/" if missing
          // try opening on SD
          //if(setupSdCard()) source = new AudioFileSourceFS(SD, song.c_str());
          if(setupSdCard()) source = new AudioFileSourceSD(song.c_str());
          // try opening on LittleFS
          //if(!source) source = new AudioFileSourceFS(LittleFS, song.c_str());
          if(!source) source = new AudioFileSourceLittleFS(song.c_str());
          if(!source) {
            Serial.print("audio file not found: ");
            Serial.println(song);
            return;
          }
          if(source){
            // switch on extension
            song.toLowerCase(); // case-insensitive match
            if(song.endsWith(".txt") || song.endsWith(".rtttl"))  generator = new AudioGeneratorRTTTL();
            /* 2FIX: compilation issues
            if(song.endsWith(".mid"))  {
              // need to load a soundfont
              AudioFileSource* sf2 = NULL;
              if(setupSdCard()) sf2 = new AudioFileSourceSD("audio/1mgm.sf2");  // TODO: make configurable
              if(!sf2) sf2 = new AudioFileSourceLittleFS("audio/1mgm.sf2");  // TODO: make configurable
              if(sf2) {
                // a soundfount was found
                AudioGeneratorMIDI* midi = new AudioGeneratorMIDI();
                generator->SetSoundfont(sf2);
                generator = midi;
              }
            }*/
            if(song.endsWith(".wav"))  generator = new AudioGeneratorWAV();
            if(song.endsWith(".mod"))  generator = new AudioGeneratorMOD();
            if(song.endsWith(".mp3")) {
                generator = new AudioGeneratorMP3();
                source = new AudioFileSourceID3(source);
            }
            if(song.endsWith(".opus"))  generator = new AudioGeneratorOpus();
            // TODO: more formats
          }
        }
      }
      
      //TODO: tone
      // https://github.com/earlephilhower/ESP8266Audio/issues/643

      //TODO: webradio
      // https://github.com/earlephilhower/ESP8266Audio/tree/master/examples/WebRadio

      if(cmd_str.startsWith("tts " ) || cmd_str.startsWith("say " )) {
        // https://github.com/earlephilhower/ESP8266SAM/blob/master/examples/Speak/Speak.ino
        audioout->begin();
        ESP8266SAM *sam = new ESP8266SAM;
        sam->Say(audioout, cmd_str.c_str() + strlen("tts "));
        delete sam;
        return;
      }
      
      if(generator && source && audioout) {
        generator->begin(source, audioout);
        // TODO async play
        while (generator->isRunning()) {
          if (!generator->loop()) generator->stop();
        }
        delete generator; delete source, delete audioout;
        return;
      }
    }  // end of music_player

  // WIP: record | mic
  // https://github.com/earlephilhower/ESP8266Audio/issues/70
  // https://github.com/earlephilhower/ESP8266Audio/pull/118

/* TODO: rewrite using the new screen dimmer feat.
  if(cmd_str.startsWith("lcd " ) || cmd_str.startsWith("tft" ) ) {
    String new_status = cmd_str.substring(strlen("lcd "), cmd_str.length());
    if(new_status=="off") {
      analogWrite(BACKLIGHT, 0);
      esp_timer_stop(screensaver_timer);
    } else if(new_status=="on") {
      getBrightness();  // reinit brightness
      reset_screensaver_timer();
    }
    return;
  }
*/

  if(cmd_str == "clock" ) {
      //esp_timer_stop(screensaver_timer);  // disable screensaver while the clock is running
      runClockLoop();
      return;
  }
    
  Serial.println("unsupported serial command: " + cmd_str);


}
 
