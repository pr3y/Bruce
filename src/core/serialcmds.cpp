
#include "serialcmds.h"
#include "globals.h"
#include <IRsend.h>
#include "cJSON.h"
#include <inttypes.h> // for PRIu64
#include <Wire.h>

#include <esp_sleep.h>
#include <driver/uart.h>

#include "sd_functions.h"
#include "settings.h"
#include "display.h"
#include "powerSave.h"
#include "wifi_common.h"
#include "passwords.h"
#include "modules/rf/rf.h"
//#include "modules/rf/rtl433.h"
//#include "modules/rf/radiolib_test.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/ir_read.h"
#include "modules/others/bad_usb.h"
#include "modules/others/webInterface.h"
#include "modules/bjs_interpreter/interpreter.h" // for JavaScript interpreter

#if defined(HAS_NS4168_SPKR) || defined(BUZZ_PIN)
  #include "modules/others/audio.h"
#endif


#include <PSRamFS.h>

bool setupPsramFs() {
  // https://github.com/tobozo/ESP32-PsRamFS/blob/main/examples/PSRamFS_Test/PSRamFS_Test.ino
  static bool psRamFSMounted = false;
  if(psRamFSMounted) return true;  // avoid reinit

  #ifdef BOARD_HAS_PSRAM
  PSRamFS.setPartitionSize( ESP.getFreePsram()/2 ); // use half of psram
  #else
  PSRamFS.setPartitionSize( SAFE_STACK_BUFFER_SIZE );
  #endif

  if(!PSRamFS.begin()){
    Serial.println("PSRamFS Mount Failed");
    psRamFSMounted = false;
    return false;
  }
  // else
  psRamFSMounted = true;
  return true;
}


String readSmallFileFromSerial() {
  String buf = "";
  String curr_line = "";
  Serial.flush();
  while (true) {
      if (!Serial.available()) {
        delay(500);
        Serial.flush();
        continue;
      }
      curr_line = Serial.readStringUntil('\n');
      if(curr_line.startsWith("EOF")) break;
      buf += curr_line + "\n";
      if(buf.length()>SAFE_STACK_BUFFER_SIZE) break;  // trim?
  }
  return buf;
}
/*
String readSmallFileFromSerialAlt() {
  String buf = "";
  Serial.flush();
  while (true) {
      if (!Serial.available()) {
        delay(500);
        Serial.flush();
        continue;
      }
      buf = Serial.readStringUntil(4);  // ascii code 4 = EOT (End of Transmit)
      return buf;
  }
}
* */


/* task to handle serial commands, currently used in headless mode only */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void serialcmds_loop(void* pvParameters) {
  Serial.begin (115200);
  while (1) {
    handleSerialCommands();
    //delay (500); // wait for half a second
    vTaskDelay(500);  // sleep this task only
  }
}

void startSerialCommandsHandlerTask() {
    TaskHandle_t serialcmdsTaskHandle;

	  xTaskCreatePinnedToCore (
      serialcmds_loop,     // Function to implement the task
      "serialcmds",   // Name of the task (any string)
      20000,      // Stack size in bytes
      NULL,      // This is a pointer to the parameter that will be passed to the new task. We are not using it here and therefore it is set to NULL.
      2,         // Priority of the task
      &serialcmdsTaskHandle,      // Task handle (optional, can be NULL).
      1          // Core where the task should run. By default, all your Arduino code runs on Core 1 and the Wi-Fi and RF functions (these are usually hidden from the Arduino environment) use the Core 0.
      );
}


bool is_free_gpio_pin(int pin_no ){
  // check if pin_no is usable for general GPIO
  std::vector<int> usable_pins = {GROVE_SDA, GROVE_SCL};

  #if defined(STICK_C_PLUS2) || defined(STICK_C_PLUS)
    usable_pins.insert(usable_pins.end(), { 25, 26, 32, 33, 0 });
  #elif defined(ESP32S3DEVKITC1)
    usable_pins.insert(usable_pins.end(), {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  // GPIO1 to GPIO25
      33,                                                                                        // GPIO33
      38, 39, 40, 41, 42,                                                                         // GPIO38 to GPIO42
      47, 48                                                                                     // GPIO47 to GPIO48
    });
  #endif

  for (int usable_pin : usable_pins)
    if (pin_no == usable_pin )
      return true;
  // else
  return false;
}


void SerialPrintHexString(uint64_t val) {
  char s[18] = {0};
  //snprintf(s, 10, "%x", val);
  //snprintf(s, sizeof(s), "%" PRIx64, val);
  snprintf(s, sizeof(s), "%llx", val);
  Serial.println(s);
}


void handleSerialCommands() {
  // read and process a single command

  String cmd_str;

  if (Serial.available() >= 1) {
    cmd_str = Serial.readStringUntil('\n');
  } else {
    // try again on next iteration
    return;
  }

  bool r = processSerialCommand(cmd_str);
  if(r) setup_gpio(); // temp fix for menu inf. loop
  else Serial.println("failed: " + cmd_str);

  Serial.print("$ ");  // prompt

  returnToMenu = true; // forced menu redrawn
}

bool processSerialCommand(String cmd_str) {
  // return true on success, false on error
  // TODO: rewrite using https://github.com/SpacehuhnTech/SimpleCLI  (auto-generated help and args checking)

  cmd_str.trim();

  if(cmd_str == "" || cmd_str.startsWith("#") || cmd_str.startsWith(";") || cmd_str.startsWith("/")) {
    // ignore empty lines and comments
    return true;
  }

  // check cmd aliases
  if(cmd_str == "ls") cmd_str = "storage list ";
  if(cmd_str == "dir") cmd_str = "storage list ";
  if(cmd_str.startsWith("ls ")) cmd_str = "storage list " + cmd_str.substring(strlen("ls "));
  if(cmd_str.startsWith("dir ")) cmd_str = "storage list " + cmd_str.substring(strlen("dir "));
  if(cmd_str.startsWith("rm ")) cmd_str = "storage remove " + cmd_str.substring(strlen("rm "));
  if(cmd_str.startsWith("del ")) cmd_str = "storage remove " + cmd_str.substring(strlen("del "));
  if(cmd_str.startsWith("md ")) cmd_str = "storage mkdir " + cmd_str.substring(strlen("md "));
  if(cmd_str.startsWith("cat ")) cmd_str = "storage read " + cmd_str.substring(strlen("cat "));
  if(cmd_str.startsWith("type ")) cmd_str = "storage read " + cmd_str.substring(strlen("type "));
  if(cmd_str.startsWith("md5 ")) cmd_str = "storage md5 " + cmd_str.substring(strlen("md5 "));
  if(cmd_str.startsWith("crc32 ")) cmd_str = "storage crc32 " + cmd_str.substring(strlen("crc32 "));
  if(cmd_str.startsWith("play ")) cmd_str = "music_player " + cmd_str.substring(strlen("play "));
  if(cmd_str.startsWith("rf ")) cmd_str = "subghz " + cmd_str.substring(strlen("rf "));
  if(cmd_str.startsWith("bu ")) cmd_str = "badusb " + cmd_str.substring(strlen("bu "));
  if(cmd_str.startsWith("set ")) cmd_str = "settings " + cmd_str.substring(strlen("set "));
  if(cmd_str.startsWith("decrypt ")) cmd_str = "crypto decrypt_from_file " + cmd_str.substring(strlen("decrypt "));
  if(cmd_str.startsWith("encrypt ")) cmd_str = "crypto encrypt_to_file " + cmd_str.substring(strlen("encrypt "));
  if(cmd_str.startsWith("run ")) cmd_str = "js " + cmd_str.substring(strlen("run "));
  if(cmd_str == "poweroff") cmd_str = "power off";
  if(cmd_str == "reboot") cmd_str = "power reboot";

  // case-insensitive matching only in some cases -- TODO: better solution for this
  if(cmd_str.indexOf("from_file ") == -1 && cmd_str.indexOf("storage ") == -1 && cmd_str.indexOf("settings ") && cmd_str.indexOf("js "))
    cmd_str.toLowerCase();

  // switch on cmd_str
  if(cmd_str.startsWith("ir") ) {

    if(cmd_str.startsWith("ir rx")) {
      IrRead* i = NULL;  // avoid calling the constructor here
      if(cmd_str == "ir rx") i = new IrRead(true);  // true -> headless mode
      else if(cmd_str == "ir rx raw") i = new IrRead(true, true);  // true -> headless mode, true = raw mode
      else return false;
      String r = i->loop_headless(10);  // 10s timeout
      if(r.length()==0) return false;
      // else
      Serial.println(r);
      delete i;
      return true;
    }

    if(cmd_str.startsWith("ir tx")) {
      // make sure it is initted
      gsetIrTxPin(false);
      //if(bruceConfig.irTx==0) bruceConfig.irTx = LED;  // quickfix init issue? CARDPUTER is 44

      // ir tx <protocol> <address> <command>
      // <protocol>: NEC, NECext, NEC42, NEC42ext, Samsung32, RC6, RC5, RC5X, SIRC, SIRC15, SIRC20, Kaseikyo, RCA
      // <address> and <command> must be in hex format
      // e.g. ir tx NEC 04000000 08000000
    }
    //TODO: if(cmd_str.startsWith("ir tx raw ")){

    if(cmd_str.startsWith("ir tx nec ")){
       String address = cmd_str.substring(10, 10+8);
       String command = cmd_str.substring(19, 19+8);
       sendNECCommand(address, command);  // TODO: add arg for displayRedStripe optional
       return true;
      }
    if(cmd_str.startsWith("ir tx rc5 ")){
       String address = cmd_str.substring(10, 10+8);
       String command = cmd_str.substring(19, 19+8);
       sendRC5Command(address, command);
       return true;
      }
    if(cmd_str.startsWith("ir tx rc6 ")){
       String address = cmd_str.substring(10, 10+8);
       String command = cmd_str.substring(19, 19+8);
       sendRC6Command(address, command);
       return true;
      }
    //if(cmd_str.startsWith("ir tx sirc")){
    //if(cmd_str.startsWith("ir tx samsung")){
    //if(cmd_str.startsWith("ir tx raw")){

    if(cmd_str.startsWith("ir tx_from_file ")){
      // example: ir tx_from_file LG_AKB72915206_power.ir
      String filepath = cmd_str.substring(strlen("ir tx_from_file "));
      filepath.trim();
      if(filepath.indexOf(".ir") == -1) return false;  // invalid filename
      if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
      if(SD.exists(filepath)) return  txIrFile(&SD, filepath);
      if(LittleFS.exists(filepath)) return  txIrFile(&LittleFS, filepath);
      // else file not found
      return false;
    }

    if(cmd_str.startsWith("ir tx_from_buffer")){
      if(!(setupPsramFs())) return false;
      String txt = readSmallFileFromSerial();
      String tmpfilepath = "/tmpramfile";  // TODO: random name?
      File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
      if(!f) return false;
      f.write((const uint8_t*) txt.c_str(), txt.length());
      f.close();
      bool r = txIrFile(&PSRamFS, tmpfilepath);
      PSRamFS.remove(tmpfilepath);  // TODO: keep cached?
      return r;
    }

    if(cmd_str.startsWith("irsend")) {
      // tasmota json command  https://tasmota.github.io/docs/Tasmota-IR/#sending-ir-commands
      // e.g. IRSend {"Protocol":"NEC","Bits":32,"Data":"0x20DF10EF"}
      // TODO: rewrite using ArduinoJson parser?
      // TODO: decode "data" into "address, command" and use existing "send*Command" funcs

      cJSON *root = cJSON_Parse(cmd_str.c_str() + 6);
      if (root == NULL) {
        Serial.println("This is NOT json format");
        return false;
      }
      uint16_t bits = 32; // defaults to 32 bits
      const char *dataStr = "";
      String protocolStr = "nec";  // defaults to NEC protocol

      cJSON * protocolItem = cJSON_GetObjectItem(root,"protocol");
      cJSON * dataItem = cJSON_GetObjectItem(root, "data");
      cJSON * bitsItem = cJSON_GetObjectItem(root,"bits");

      if(protocolItem && cJSON_IsString(protocolItem)) {
        protocolStr = protocolItem->valuestring;
      }  else {
        Serial.println("missing or invalid protocol to send");
        return false;
      }
      if(bitsItem && cJSON_IsNumber(bitsItem)) bits = bitsItem->valueint;
      if(dataItem && cJSON_IsString(dataItem)) {
        dataStr = dataItem->valuestring;
      } else {
        Serial.println("missing or invalid data to send");
        return false;
      }
      //String dataStr = cmd_str.substring(36, 36+8);
      uint64_t data = strtoul(dataStr, nullptr, 16);
      //Serial.println(dataStr);
      //SerialPrintHexString(data);
      //Serial.println(bits);
      //Serial.println(protocolItem->valuestring);

      cJSON_Delete(root);

      /*if(protocolStr == "nec"){
        // sendNEC(uint64_t data, uint16_t nbits, uint16_t repeat)
        irsend.sendNEC(data, bits, 10);
        return true;
      }
      */

      return sendDecodedCommand(protocolStr, dataStr, String(bits));
    }

    // turn off the led
    digitalWrite(bruceConfig.irTx, LED_OFF);
    //backToMenu();
    return false;
  }  // end of ir commands

  if(cmd_str.startsWith("rf") || cmd_str.startsWith("subghz" )) {

    if(cmd_str.startsWith("subghz rx")) {
      /*
      const char* args = cmd_str.c_str() + strlen("subghz rx");
      float frequency=bruceConfig.rfFreq;  // global default
      if(strlen(args)>1) sscanf(args, " %f", &frequency);
      * */
      String args = cmd_str.substring(cmd_str.indexOf(" ", strlen("subghz rx")));
      float frequency=bruceConfig.rfFreq;  // global default
      if(args.length()>1) {
        sscanf(args.c_str(), " %f", &frequency);
        frequency /= 1000000; // passed as a long int (e.g. 433920000)
      }
      //Serial.print("frequency:");
      //Serial.println(frequency);
      String r = "";
      if(cmd_str.startsWith("subghz rx_raw") || cmd_str.startsWith("subghz rx raw"))
        r = RCSwitch_Read(frequency, 10, true);  // true -> raw mode
      else
        r = RCSwitch_Read(frequency, 10, false);  // false -> decoded mode
      if(r.length()==0) return false;
      // else
      Serial.println(r);
      return true;

    }
    if(cmd_str.startsWith("subghz tx_from_file")) {
      // example: subghz tx_from_file plug1_on.sub
      String filepath = cmd_str.substring(strlen("subghz tx_from_file "));
      filepath.trim();
      if(filepath.indexOf(".sub") == -1) return false;  // invalid filename
      if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
      if(SD.exists(filepath)) return  txSubFile(&SD, filepath);
      if(LittleFS.exists(filepath)) return  txSubFile(&LittleFS, filepath);
      // else file not found
      return false;
    }
    if(cmd_str == "subghz tx_from_buffer") {
      if(!(setupPsramFs())) return false;
      String txt = readSmallFileFromSerial();
      String tmpfilepath = "/tmpramfile";  // TODO: random name?
      File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
      if(!f) return false;
      f.write((const uint8_t*) txt.c_str(), txt.length());
      f.close();
      //if(PSRamFS.exists(filepath))
      bool r = txSubFile(&PSRamFS, tmpfilepath);
      PSRamFS.remove(tmpfilepath);  // TODO: keep cached?
      return r;
    }

    if(cmd_str.startsWith("subghz tx")) {
      // flipperzero-like cmd  https://docs.flipper.net/development/cli/#wLVht
      // e.g. subghz tx 0000000000200001 868250000 403 10  // https://forum.flipper.net/t/friedland-libra-48249sl-wireless-doorbell-request/4528/20
      //                {hex_key} {frequency} {te} {count}
      // subghz tx 445533 433920000 174 10
      const char* args = cmd_str.c_str() + strlen("subghz tx");
      uint64_t key=0;
      unsigned long frequency=433920000;
      unsigned int te=0;
      unsigned int count=10;
      if(strlen(args)<=1) return false;
      if(sscanf(args, " %llx %lu %u %u", &key, &frequency, &te, &count)<=0) return false;  // missing 1 req arg
      unsigned int bits=24;  // TODO: compute from key
      if(!initRfModule("tx", float(frequency/1000000.0))) return false;  // check valid frequency and init the rf module
      RCSwitch_send( key, bits, te, 1, count );
      deinitRfModule();
      return true;
    }

    if(cmd_str.startsWith("subghz scan")) {
      // subghz scan 433 434
      String args = cmd_str.substring(cmd_str.indexOf(" ", strlen("subghz rx")));
      float start_frequency=0;  // global default
      float stop_frequency=0;  // global default
      if(args.length()>1) {
        sscanf(args.c_str(), " %f %f", &start_frequency, &stop_frequency);
        if(!start_frequency || !stop_frequency) return false;  // invalid args
        // passed as a long int (e.g. 433920000)
        start_frequency /= 1000000;
        stop_frequency /= 1000000;
      } else return false;  // missing args
      rf_scan(start_frequency, stop_frequency, 10*1000);  // 10s timeout
      return true;
    }

    if(cmd_str.startsWith("rfsend")) {
      // tasmota json command  https://tasmota.github.io/docs/RF-Protocol/
      // e.g. RfSend {"Data":"0x447503","Bits":24,"Protocol":1,"Pulse":174,"Repeat":10}  // on
      // e.g. RfSend {"Data":"0x44750C","Bits":24,"Protocol":1,"Pulse":174,"Repeat":10}  // off

      cJSON *root = cJSON_Parse(cmd_str.c_str() + 6);
      if (root == NULL) {
        Serial.println("This is NOT json format");
        return false;
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
        return false;
      }
      //String dataStr = cmd_str.substring(36, 36+8);
      uint64_t data = strtoul(dataStr, nullptr, 16);
      //Serial.println(dataStr);
      //SerialPrintHexString(data);
      //Serial.println(bits);

      if(!initRfModule("tx")) return false;

      RCSwitch_send(data, bits, pulse, protocol, repeat);

      cJSON_Delete(root);
      return true;
    }
  }  // endof rf

  #if defined(USB_as_HID)
    // badusb available
    if(cmd_str.startsWith("badusb run_from_file ")) {
      // badusb run_from_file HelloWorld.txt
      String filepath = cmd_str.substring(strlen("badusb run_from_file "));
      filepath.trim();
      if(filepath.indexOf(".txt") == -1) return false;  // invalid filename
      if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
      FS* fs = NULL;
      if(SD.exists(filepath)) fs = &SD;
      if(LittleFS.exists(filepath)) fs = &LittleFS;
      if(!fs) return false;  // file not found
      Kb.begin();
      USB.begin();
      key_input(*fs, filepath);
      //TODO: need to reinit serial when finished
      //Kb.end();
      //USB.~ESPUSB(); // Explicit call to destructor
      //Serial.begin(115200);
      return true;
    }
    if(cmd_str == "badusb run_from_buffer") {
      if(!(setupPsramFs())) return false;
      String txt = readSmallFileFromSerial();
      String tmpfilepath = "/tmpramfile";  // TODO: random name?
      File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
      if(!f) return false;
      f.write((const uint8_t*) txt.c_str(), txt.length());
      f.close();
      Kb.begin();
      USB.begin();
      key_input(PSRamFS, tmpfilepath);
      PSRamFS.remove(tmpfilepath);  // TODO: keep cached?
      return true;
    }
  #endif

  #if defined(HAS_NS4168_SPKR) || defined(BUZZ_PIN)
    if(cmd_str.startsWith("tone") || cmd_str.startsWith("beep")) {  // || cmd_str.startsWith("music_player beep" )
      const char* args = cmd_str.c_str() + 4;
      unsigned long frequency = 500UL;
      unsigned long duration = 500UL;  // default to 2 sec
      if(strlen(args)>1) sscanf(args, " %lu %lu", &frequency, &duration);  // try to read the args, keep the defaults if missing
      //Serial.print((int) frequency);
      //Serial.print((int) duration);
      _tone(frequency, duration);
      //delay(1000);
      //playTone(frequency, duration, 1);  // sine
      return true;
    }
  #endif

  #if defined(HAS_NS4168_SPKR) //M5StickCs doesn't have speakers.. they have buzzers on pin 02 that only beeps in different frequencies
    if(cmd_str.startsWith("music_player " ) ) {  // || cmd_str.startsWith("play " )
      String song = cmd_str.substring(13);
      if(song.indexOf(":") != -1) {
        // RTTTL player
        // music_player mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6
        return playAudioRTTTLString(song);

      } else if(song.indexOf(".") != -1) {
        // try to open "song" as a file
        // e.g. music_player boot.wav
        if(!song.startsWith("/")) song = "/" + song;  // add "/" if missing
        if(SD.exists(song)) return playAudioFile(&SD, song);
        if(LittleFS.exists(song)) return playAudioFile(&LittleFS, song);
        // else not found
        return false;
      }
    }

    //TODO: webradio
    // https://github.com/earlephilhower/ESP8266Audio/tree/master/examples/WebRadio

    if(cmd_str.startsWith("tts " ) || cmd_str.startsWith("say " )) {
      String text = cmd_str.substring(4);
      return tts(text);
    }
 #endif  // HAS_NS4168_SPKR

  // WIP: record | mic
  // https://github.com/earlephilhower/ESP8266Audio/issues/70
  // https://github.com/earlephilhower/ESP8266Audio/pull/118

#if defined(HAS_SCREEN)
  // backlight brightness adjust (range 0-255) https://docs.flipper.net/development/cli/#XQQAI
  // e.g. "led br 127"
  if(cmd_str.startsWith("led br ")) {
    const char* valueStr = cmd_str.c_str() + strlen("led br ");
    int value = (atoi(valueStr) * 100) / 255;  // convert to 0-100 range
    //Serial.print("value: ");
    //Serial.println(value);
    if(value<=0) value=1;
    if(value>100) value=100;
    setBrightness(value, false);  // false -> do not save
    return true;
  }
  else if(cmd_str.startsWith("led ")) {
    // change UI color
    // e.g. "led 255 255 255"
    const char* rgbString = cmd_str.c_str() + 4;
    int r, g, b;
    if (sscanf(rgbString, "%d %d %d", &r, &g, &b) != 3) {
        Serial.println("invalid color: " + String(rgbString));
        return false;
    }
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        Serial.println("invalid color: " + String(rgbString));
        return false;
    }
    uint16_t hexColor = tft.color565(r, g, b);  // Use the TFT_eSPI function to convert RGB to 16-bit color
    //Serial.print("converted color:");
    //SerialPrintHexString(hexColor);
    bruceConfig.priColor = hexColor;  // change global var, dont save in config
    return true;
  }
  if(cmd_str == "clock" ) {
      //esp_timer_stop(screensaver_timer);  // disable screensaver while the clock is running
      runClockLoop();
      return true;
  }
#endif  // HAS_SCREEN

  // power cmds: off, reboot, sleep
  if(cmd_str == "power off" ) {
    // closest thing https://github.com/esp8266/Arduino/issues/929
    #if defined(STICK_C_PLUS)
      axp192.PowerOff();
    #elif defined(STICK_C_PLUS2)
      digitalWrite(4,LOW);
    //#elif defined(NEW_DEVICE)
    #elif defined(T_EMBED)
      digitalWrite(PIN_POWER_ON,LOW); 
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_6,LOW); 
      esp_deep_sleep_start();

    #else
      //ESP.deepSleep(0);
      esp_deep_sleep_start();  // only wake up via hardware reset
    #endif
    return true;
  }
  if(cmd_str == "power reboot" ) {
    ESP.restart();
    return true;
  }
  if(cmd_str == "power sleep" ) {
    // NOTE: cmd not supported on flipper0
    setSleepMode();
    //turnOffDisplay();
    //esp_timer_stop(screensaver_timer);
    return true;
  }

  if(cmd_str == "webui" ) {
    // start the webui
    if(!wifiConnected) {
      Serial.println("wifiConnect");
      wifiApConnect();  // TODO: read mode from config file
    }
    Serial.println("startWebUi");
    startWebUi(true);  // MEMO: will quit when checkEscPress
    return true;
  }
  //TODO: if(cmd_str == "webui stop" ) {

/*
   // WIP https://github.com/pr3y/Bruce/issues/162#issuecomment-2308788115

   if(cmd_str.startsWith("serial2 write")) {
    setupBruceDaughterboard();
    String args = cmd_str.substring(strlen("serial2 write"));
    Serial2.println(args);  // wakeup
    Serial2.flush();
    return true;
  }

   if(cmd_str.startsWith("serial2 read")) {
    setupBruceDaughterboard();
    String curr_line = "";
    while (Serial2.available()) {
      curr_line = Serial.readStringUntil('\n');
      Serial2.println(curr_line);
    }
    return true;
  }

  if(cmd_str == "power sleep_and_wakeup_from_uart" ) {
    #ifdef HAS_SCREEN
      turnOffDisplay();
    #endif
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/sleep_modes.html#uart-wakeup-light-sleep-only
    // 2FIX: not waking up
    // https://github.com/espressif/arduino-esp32/issues/5107
    // https://github.com/espressif/arduino-esp32/issues/6976
    //Serial.println(TX);
    //Serial.println(RX);
    //Serial.println((gpio_num_t)RX);
    Serial.println("going to sleep...");
    //uart_set_pin(UART_NUM_0, TX, RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    gpio_sleep_set_direction((gpio_num_t)RX, GPIO_MODE_INPUT);
    gpio_sleep_set_pull_mode((gpio_num_t)RX, GPIO_PULLUP_ONLY);
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    uart_set_wakeup_threshold(UART_NUM_0, 3);  // 3 edges on U0RXD to wakeup
    delay(100);
    esp_sleep_enable_uart_wakeup(UART_NUM_0);
    //uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
    delay(100);
    esp_light_sleep_start();
    Serial.begin(115200); // needs reinit?
    Serial.println("woken");
    return true;
  }
*/

  // gpio cmds https://docs.flipper.net/development/cli/#aqA4b
  if(cmd_str.startsWith("gpio mode ")) {
    const char* args = cmd_str.c_str() + strlen("gpio mode ");
    int pin_number=-1;
    int mode=0;
    if (sscanf(args, "%d %d", &pin_number, &mode) == 2) {
      // check usable pins according to the env
      if(mode>=0 && mode<=1 && is_free_gpio_pin(pin_number)) {
        pinMode(pin_number, mode);
        return true;
      }
    }
    // else
    Serial.print("invalid args: ");
    Serial.println(args);
    return false;
  }
  if(cmd_str.startsWith("gpio set ")) {
    const char* args = cmd_str.c_str() + strlen("gpio set ");
    int pin_number=-1;
    int value=0;
    if (sscanf(args, "%d %d", &pin_number, &value) == 2) {
      // check usable pins according to the env
      if(value>=0 && value<=1 && is_free_gpio_pin(pin_number)) {
        digitalWrite(pin_number, value);
        return true;
      }
    }
    // else
    Serial.print("invalid args: ");
    Serial.println(args);
    return false;
  }
  if(cmd_str.startsWith("gpio read ")) {
    const char* args = cmd_str.c_str() + strlen("gpio read ");
    int pin_number=-1;
    if (sscanf(args, "%d", &pin_number) == 1) {
      // check usable pins according to the env
      if(is_free_gpio_pin(pin_number)) {
        Serial.println(digitalRead(pin_number));
        return true;
      }
    }
    // else
    Serial.print("invalid args: ");
    Serial.println(args);
    return false;
  }

  if(cmd_str == "factory_reset") {
      // remove config file and recreate
      if(SD.exists(bruceConfig.filepath)) SD.remove(bruceConfig.filepath);
      if(LittleFS.exists(bruceConfig.filepath)) LittleFS.remove(bruceConfig.filepath);
      bruceConfig.fromFile();  // recreate config file if it does not exists
      return true;
  }

  if(cmd_str.startsWith("settings")) {
    JsonDocument jsonDoc = bruceConfig.toJson();
    JsonObject setting = jsonDoc.as<JsonObject>();

    String args = cmd_str.substring(strlen("settings "));
    args.trim();
    if(args.length()==0) {
      // no args, just prints current config
      serializeJsonPretty(jsonDoc, Serial);
      Serial.println("");
      return true;
    }
    // else
    String setting_name = args.substring(0, args.indexOf(" "));
    setting_name.trim();
    // if(!setting.containsKey(setting_name)) {
    if(setting[setting_name].isNull()) {
      Serial.println("invalid field name: " + setting_name);
      return false;
    }
    String setting_value = args.substring(args.indexOf(" "));
    setting_value.trim();
    if(setting_value.length()==0) {
      // just prints current config
      Serial.println(setting[setting_name].as<String>());
      return true;
    }
    // else change the passed config
    if(setting_name=="priColor") bruceConfig.setTheme(setting_value.toInt());
    if(setting_name=="rot") bruceConfig.setRotation(setting_value.toInt());
    if(setting_name=="dimmerSet") bruceConfig.setDimmer(setting_value.toInt());
    if(setting_name=="bright") bruceConfig.setBright(setting_value.toInt());
    if(setting_name=="tmz") bruceConfig.setTmz(setting_value.toInt());
    if(setting_name=="soundEnabled") bruceConfig.setSoundEnabled(setting_value.toInt());
    if(setting_name=="wifiAtStartup") bruceConfig.setWifiAtStartup(setting_value.toInt());
    if(setting_name=="webUI") {
      bruceConfig.setWebUICreds(
        setting_value.substring(0, setting_value.indexOf(",")),
        setting_value.substring(setting_value.indexOf(",")+1)
      );
    }
    if(setting_name=="wifiAp") {
      bruceConfig.setWifiApCreds(
        setting_value.substring(0, setting_value.indexOf(",")),
        setting_value.substring(setting_value.indexOf(",")+1)
      );
    }
    if(setting_name=="wifi") {
      bruceConfig.addWifiCredential(
        setting_value.substring(0, setting_value.indexOf(",")),
        setting_value.substring(setting_value.indexOf(",")+1)
      );
    }
    if(setting_name=="irTx") bruceConfig.setIrTxPin(setting_value.toInt());
    if(setting_name=="irRx") bruceConfig.setIrRxPin(setting_value.toInt());
    if(setting_name=="rfTx") bruceConfig.setRfTxPin(setting_value.toInt());
    if(setting_name=="rfRx") bruceConfig.setRfRxPin(setting_value.toInt());
    if(setting_name=="rfModule") bruceConfig.setRfModule(static_cast<RFModules>(setting_value.toInt()));
    if(setting_name=="rfFreq" && setting_value.toFloat()) bruceConfig.setRfFreq(setting_value.toFloat());
    if(setting_name=="rfFxdFreq") bruceConfig.setRfFxdFreq(setting_value.toInt());
    if(setting_name=="rfScanRange") bruceConfig.setRfScanRange(setting_value.toInt());
    if(setting_name=="rfidModule") bruceConfig.setRfidModule(static_cast<RFIDModules>(setting_value.toInt()));
    if(setting_name=="wigleBasicToken") bruceConfig.setWigleBasicToken(setting_value);
    if(setting_name=="devMode") bruceConfig.setDevMode(setting_value.toInt());
    return true;
  }

  if(cmd_str == "info device" || cmd_str == "!") {
    Serial.print("Bruce v");
    Serial.println(BRUCE_VERSION);
    Serial.println(GIT_COMMIT_HASH);
    Serial.printf("SDK: %s\n", ESP.getSdkVersion());
    // TODO: read mac addresses https://lastminuteengineers.com/esp32-mac-address-tutorial/

    // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ChipID/GetChipID/GetChipID.ino
    //Serial.printf("Chip is %s (revision v%d)\n", ESP.getChipModel(), ESP.getChipRevision());
    //Serial.printf("Detected flash size: %d\n", ESP.getFlashChipSize());
    //Serial.printf("This chip has %d cores\n", ESP.getChipCores());
    //Serial.printf("CPU Freq is %d\n", ESP.getCpuFreqMHz());
    // Features: WiFi, BLE, Embedded Flash 8MB (GD)
    // Crystal is 40MHz
    // MAC: 24:58:7c:5b:24:5c

    if (wifiConnected) {
      Serial.println("wifi: connected");
      Serial.println("ip: " + wifiIP);  // read global var
    } else {
      Serial.println("wifi: not connected");
    }
    return true;
  }

  if(cmd_str == "free") {
      // report free memory
      Serial.print("Total heap: ");
      Serial.println(ESP.getHeapSize());
      Serial.print("Free heap: ");
      Serial.println(ESP.getFreeHeap());
      if(psramFound()) {
        Serial.print("Total PSRAM: ");
        Serial.println(ESP.getPsramSize());
        Serial.print("Free PSRAM: ");
        Serial.println(ESP.getFreePsram());
      }
      return true;
  }

  if(cmd_str == "i2c") {
    // scan for connected i2c modules
    // derived from https://learn.adafruit.com/scanning-i2c-addresses/arduino
    Wire.begin(GROVE_SDA, GROVE_SCL);
    byte error, address;
    int nDevices;
    Serial.println("Scanning...");
    nDevices = 0;
    for(address = 1; address < 127; address++ )
    {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      if (error == 0)
      {
        Serial.print("I2C device found at address 0x");
        if (address<16)
          Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("  !");
        nDevices++;
      }
      else if (error==4)
      {
        Serial.print("Unknown error at address 0x");
        if (address<16)
          Serial.print("0");
        Serial.println(address,HEX);
      }
    }  // end for
    if (nDevices == 0) {
      Serial.println("No I2C devices found\n");
      return false;
    } else {
      Serial.println("done\n");
      return true;
    }
  }

  // "storage" cmd to manage files  https://docs.flipper.net/development/cli/#Xgais

  if(cmd_str.startsWith("storage read ") || cmd_str.startsWith("storage md5 ") || cmd_str.startsWith("storage crc32 ")) {
    //String filepath = cmd_str.substring(strlen("storage read "));
    String filepath = cmd_str.substring(cmd_str.indexOf(" ", strlen("storage md5")));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    FS* fs = NULL;
    if(SD.exists(filepath)) fs = &SD;
    if(LittleFS.exists(filepath)) fs = &LittleFS;
    if(!fs) return false;
    // else
    if(cmd_str.startsWith("storage read ")) {
      Serial.println( readSmallFile(*fs, filepath) );
      return true;
    }
    else if(cmd_str.startsWith("storage md5 ")) {
      Serial.println( md5File(*fs, filepath) );
      return true;
    }
    else if(cmd_str.startsWith("storage crc32 ")) {
      Serial.println( crc32File(*fs, filepath) );
      return true;
    }
  }

  if(cmd_str.startsWith("storage stat ")) {  // TODO: storage timestamp
    // storage stat /ir/_Flipper-IRDB-main.zip
    String filepath = cmd_str.substring(strlen("storage stat "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    FS* fs = NULL;
    if(SD.exists(filepath)) fs = &SD;
    if(LittleFS.exists(filepath)) fs = &LittleFS;
    if(!fs) return false;  // file not found
    File file = fs->open(filepath, FILE_READ);
    if (!file) return false;
    //Serial.println(filepath);
    //Serial.println(file.size());
    //Serial.println(file.getLastWrite());

    //ALT.: use <sys/stat.h> directly https://github.com/espressif/arduino-esp32/blob/66c9c0b1a6a36b85d27cdac0fb52098368de1a09/libraries/FS/src/vfs_api.cpp#L348#L348
    // missing in Core2? https://github.com/pr3y/Bruce/actions/runs/10469748217/job/28993441958?pr=196
    /*
    #if !defined(M5STACK)
    if(SD.exists(filepath)) filepath = "/sd" + filepath;
    else if(LittleFS.exists(filepath)) filepath = "/littlefs" + filepath;
    else return false;  // not found
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (stat(filepath.c_str(), &st) != 0) return false;
    * */
    // else
    Serial.print("File: ");
    Serial.print(filepath);
    Serial.println("");

    Serial.print("Size: ");
    //Serial.print(st.st_size);
    Serial.print(file.size());
    Serial.print("\t\t");
    //if(S_ISDIR(st.st_mode))
    if(file.isDirectory())
      Serial.print("directory");
    else
      Serial.print("regular file");
    Serial.println("");

    Serial.print("Modify: ");
    //Serial.print(st.st_mtime);    // TODO: parse to localtime
    Serial.print(file.getLastWrite());    // TODO: parse to localtime
    Serial.println("");

    //Serial.println(st.st_mode);
    //Serial.println(st.st_dev);
    //Serial.println(st.st_ctime);
    file.close();
    return true;
  }
  if(cmd_str.startsWith("storage list ")) {
    // storage list BruceRF
    String filepath = cmd_str.substring(strlen("storage list "));
    filepath.trim();
    //if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    FS* fs = NULL;
    if(SD.exists(filepath)) fs = &SD;
    if(LittleFS.exists(filepath)) fs = &LittleFS;
    if(!fs) return false;  // dir not found
    File root = fs->open(filepath);
    if (!root || !root.isDirectory()) return false; // not a dir
    File file2 = root.openNextFile();
    while (file2) {
        Serial.print(file2.name());
        if (file2.isDirectory()) {
          Serial.println("\t<DIR>");
        } else {
          Serial.print("\t");
          Serial.println(file2.size());
        }
        //Serial.println(file2.path());
        //Serial.println(file2.getLastWrite());  // TODO: parse to localtime
        file2 = root.openNextFile();
    }
    file2.close();
    root.close();
    return true;
  }
  if(cmd_str.startsWith("storage remove ")) {
    String filepath = cmd_str.substring(strlen("storage remove "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    if(SD.exists(filepath)) return SD.remove(filepath);
    if(LittleFS.exists(filepath)) return LittleFS.remove(filepath);
    // else
    return false;
  }
  if(cmd_str.startsWith("storage mkdir ")) {
    String filepath = cmd_str.substring(strlen("storage mkdir "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    if(!SD.exists(filepath)) return SD.mkdir(filepath);
    if(!LittleFS.exists(filepath)) return LittleFS.mkdir(filepath);
    // else
    return false;
  }
  if(cmd_str.startsWith("storage write ")) {
    String filepath = cmd_str.substring(strlen("storage write "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    FS* fs = &LittleFS; // default fallback
    if(SD.exists(filepath)) fs = &SD;
    if(LittleFS.exists(filepath)) fs = &LittleFS;
    if(!fs && sdcardMounted) fs = &SD;
    String txt = readSmallFileFromSerial();
    if(txt.length()==0) return false;
    File f = fs->open(filepath, FILE_APPEND, true);  // create if it does not exist, append otherwise
    if(!f) return false;
    f.write((const uint8_t*) txt.c_str(), txt.length());
    f.close();
    Serial.println("file written: " + filepath);
    return true;
  }

  if(cmd_str.startsWith("storage rename ")) {
    // storage rename HelloWorld.txt HelloWorld2.txt
    String args = cmd_str.substring(strlen("storage rename "));
    String filepath = args.substring(0, args.indexOf(" "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    String newName = args.substring(args.indexOf(" "), args.length());
    newName.trim();
    if(newName.length()==0) return false;  // missing arg
    if(!newName.startsWith("/")) newName = "/" + newName;  // add "/" if missing
    Serial.println(filepath);
    Serial.println(newName);
    if(SD.exists(filepath)) return SD.rename(filepath, newName);
    if(LittleFS.exists(filepath)) return LittleFS.rename(filepath, newName);
    // else
    return false;
  }
  if(cmd_str.startsWith("storage copy ")) {
    // storage copy HelloWorld.txt /dest/path
    // storage copy bruce.conf badusb
    String args = cmd_str.substring(strlen("storage copy "));
    String filepath = args.substring(0, args.indexOf(" "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    String newName = args.substring(args.indexOf(" "), args.length());
    newName.trim();
    if(newName.length()==0) return false;  // missing arg
    if(!newName.startsWith("/")) newName = "/" + newName;  // add "/" if missing
    //Serial.println(filepath);
    //Serial.println(newName);
    fileToCopy=filepath;
    if(SD.exists(filepath)) return pasteFile(SD, newName);
    if(LittleFS.exists(filepath)) return pasteFile(LittleFS, newName);
    // else
    fileToCopy="";
    return false;
  }

#if !defined(LITE_VERSION)
  if(cmd_str.startsWith("js run_from_buffer")){
    if(!(setupPsramFs())) return false;
    String txt = readSmallFileFromSerial();
    String tmpfilepath = "/tmpramfile";
    File f = PSRamFS.open(tmpfilepath, FILE_WRITE);
    if(!f) return false;
    f.write((const uint8_t*) txt.c_str(), txt.length());
    f.close();
    bool r = run_bjs_script_headless(PSRamFS, tmpfilepath);
    PSRamFS.remove(tmpfilepath);
    return r;
  }

  if(cmd_str.startsWith("js ")) {
    String filepath = cmd_str.substring(strlen("js "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    //if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    FS* fs = NULL;
    if(SD.exists(filepath)) fs = &SD;
    if(LittleFS.exists(filepath)) fs = &LittleFS;
    if(!fs) {   // dir not found
      // assume filepath is an inline script
      Serial.println(filepath);
      run_bjs_script_headless(filepath);
      return true;
    }
    // else
    run_bjs_script_headless(*fs, filepath);
    // else
    return true;
  }
#endif
  if(cmd_str.startsWith("crypto ")) {
    // crypto decrypt_from_file passwords/github.com.txt.enc 1234
    // crypto encrypt_to_file passwords/github.com.txt.enc 1234
    String args = "";
    if(cmd_str.startsWith("crypto decrypt_from_file "))
      args = cmd_str.substring(strlen("crypto decrypt_from_file "));
    else if(cmd_str.startsWith("crypto type_from_file "))
      args = cmd_str.substring(strlen("crypto type_from_file "));
    else if(cmd_str.startsWith("crypto encrypt_to_file "))
      args = cmd_str.substring(strlen("crypto encrypt_to_file "));
    String filepath = args.substring(0, args.indexOf(" "));
    filepath.trim();
    if(filepath.length()==0) return false;  // missing arg
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    String password = args.substring(args.indexOf(" ")+1, args.length());
    password.trim();
    if(password.length()==0) return false;  // missing arg
    cachedPassword = password;  // avoid interactive prompt
    //Serial.println(filepath);
    //Serial.println(password);


    if(cmd_str.startsWith("crypto decrypt_from_file") || cmd_str.startsWith("crypto type_from_file")) {
      FS* fs = NULL;
      if(SD.exists(filepath)) fs = &SD;
      if(LittleFS.exists(filepath)) fs = &LittleFS;
      if(!fs) return false;  // file not found
      String plaintext = readDecryptedFile(*fs, filepath);
      if(plaintext=="") return false;
      Serial.println(plaintext);
      if(cmd_str.startsWith("crypto decrypt_from_file")) return true;
      #if defined(USB_as_HID)
      else if(cmd_str.startsWith("crypto type_from_file")) {
        key_input_from_string(plaintext);
        return true;
      }
      #endif
      return false;
    }
    else if(cmd_str.startsWith("crypto encrypt_to_file")) {
      String txt = readSmallFileFromSerial();
      if(txt.length()==0) return false;
      FS* fs = &SD;
      if(LittleFS.exists(filepath)) fs = &LittleFS;
      File f = fs->open(filepath, FILE_WRITE);
      if(!f) return false;
      String cyphertxt = encryptString(txt, cachedPassword);
      if(cyphertxt=="") return false;
      f.write((const uint8_t*) cyphertxt.c_str(), cyphertxt.length());
      f.close();
      Serial.println("file written: " + filepath);
      return true;
    }
  }

   if(cmd_str == "uptime") {
      // https://github.com/espressif/arduino-esp32/blob/66c9c0b1a6a36b85d27cdac0fb52098368de1a09/libraries/WebServer/examples/AdvancedWebServer/AdvancedWebServer.ino#L64
      int sec = millis() / 1000;
      int hr = sec / 3600;
      int min = (sec / 60) % 60;
      sec = sec % 60;
      char temp[400];
      snprintf(temp, 400, "Uptime: %02d:%02d:%02d", hr, min, sec);
      Serial.println(temp);
      return true;
  }

   if(cmd_str == "date") {
     if (clock_set) {
        Serial.print("Current time: ");
        #if !defined(HAS_RTC)
          Serial.println(rtc.getDateTime());
          //Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
        #else
          RTC_TimeTypeDef _time;
          RTC_DateTypeDef _date;
          cplus_RTC _rtc;
          _rtc.begin();
          _rtc.GetTime(&_time);
          _rtc.GetDate(&_date);
          char stimeStr[100] = {0};
          snprintf(stimeStr, sizeof(stimeStr), "%02d %02d %04d %02d:%02d:%02d", _date.Month, _date.Date, _date.Year, _time.Hours, _time.Minutes, _time.Seconds);
          Serial.println(stimeStr);
        #endif
        return true;
      } else {
        Serial.println("clock not set");
        return false;
      }
   }

   if(cmd_str == "wifi off") {
     wifiDisconnect();
     return true;
   }


/* WIP
   if(cmd_str.startsWith("wifi scan") || cmd_str.startsWith("scanap") {
   }

   if(cmd_str.startsWith("bt scan")) {
   }

   if(cmd_str.startsWith("rtl433")) {
    // https://github.com/pr3y/Bruce/issues/192
    rtl433_setup();
    return rtl433_loop(10000*3);
  }

  if(cmd_str.startsWith("mass_storage")) {
    // WIP https://github.com/pr3y/Bruce/issues/210
    // https://github.com/espressif/arduino-esp32/blob/master/libraries/SD_MMC/examples/SD2USBMSC/SD2USBMSC.ino
  }

  if(cmd_str.startsWith("radiolib rx")) {
    // https://github.com/jgromes/RadioLib/discussions/973
    setup_cc1101();
    while(loop_rx);
    return true;
  }*/

  //  TODO: help

  //  TODO: more commands https://docs.flipper.net/development/cli#0Z9fs

  Serial.println("unsupported serial command: " + cmd_str);
  return false;
}

