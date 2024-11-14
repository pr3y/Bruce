#include "core/globals.h"
#include "core/sd_functions.h"
#include "core/main_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "bad_ble.h"

#define DEF_DELAY 100
BleKeyboard Kble(String("Keyboard_" + String((uint8_t)(ESP.getEfuseMac() >> 32), HEX)).c_str(), "BruceNet", 98);
uint8_t Ask_for_restart=0;
/* Example of payload file

REM Author: example
REM Description: open Cmd to type a message
REM Version: 1.0
REM Category: FUN
DELAY 800
GUI r
DELAY 800
STRING cmd
DELAY 800
ENTER
DELAY 800
LEFTARROW
DELAY 800
ENTER
DELAY 500
ALT ENTER
DELAY 500
STRINGLN encho Is this funny??
REPEAT 20

*/

void key_input_ble(FS fs, String bad_script) {
  if (fs.exists(bad_script) && bad_script!="") {
    File payloadFile = fs.open(bad_script, "r");
    if (payloadFile) {
      tft.setCursor(0, 40);
      tft.println("from file!");
      String lineContent = "";
      String Command = "";
      char Cmd[15];
      String Argument = "";
      String RepeatTmp = "";
      char ArgChar;
      bool ArgIsCmd;  // Verifies if the Argument is DELETE, TAB or F1-F12
      int cmdFail;    // Verifies if the command is supported, mus pass through 2 if else statemens and summ 2 to not be supported
      int line;       // Shows 3 commands of the payload on screen to follow the execution


      Kble.releaseAll();
      tft.setTextSize(1);
      tft.setCursor(0, 0);
      tft.fillScreen(bruceConfig.bgColor);
      line = 0;

      while (payloadFile.available()) {
        if(checkSelPress()) {
          while(checkSelPress()); // hold the code in this position until release the btn
          options = {
            {"Continue",  [=](){ yield(); }},
            {"Main Menu", [=](){ returnToMenu=true;}},
          };
          delay(250);
          loopOptions(options);
          delay(250);
          tft.setTextSize(FP);
        }
        if(returnToMenu) break;
        lineContent = payloadFile.readStringUntil('\n');  // O CRLF é uma combinação de dois caracteres de controle: o “Carriage Return” (retorno de carro) representado pelo caractere “\r” e o “Line Feed” (avanço de linha) representado pelo caractere “\n”.
        if (lineContent.endsWith("\r")) lineContent.remove(lineContent.length() - 1);

        ArgIsCmd = false;
        cmdFail = 0;
        RepeatTmp = lineContent.substring(0, lineContent.indexOf(' '));
        RepeatTmp = RepeatTmp.c_str();
        if (RepeatTmp == "REPEAT") {
          if (lineContent.indexOf(' ') > 0) {
            RepeatTmp = lineContent.substring(lineContent.indexOf(' ') + 1);  // how many times it will repeat, using .toInt() conversion;
            if (RepeatTmp.toInt() == 0) {
              RepeatTmp = "1";
              tft.setTextColor(ALCOLOR);
              tft.println("REPEAT argument NaN, repeating once");
            }
          } else {
            RepeatTmp = "1";
            tft.setTextColor(ALCOLOR);
            tft.println("REPEAT without argument, repeating once");
          }
        } else {
          Command = lineContent.substring(0, lineContent.indexOf(' '));    // get the Command
          strcpy(Cmd, Command.c_str());                                    // get the cmd
          if(lineContent.indexOf(' ')>0) Argument = lineContent.substring(lineContent.indexOf(' ') + 1);  // get the argument
          else Argument = "";
          RepeatTmp = "1";
        }
        uint16_t i;
        for (i = 0; i < RepeatTmp.toInt(); i++) {
          char OldCmd[15];
          //Command = char(Command.c_str());
          Argument = Argument.c_str();
          ArgChar = Argument.charAt(0);


          if (Argument == "F1" || Argument == "F2" || Argument == "F3" || Argument == "F4" || Argument == "F5" || Argument == "F6" || Argument == "F7" || Argument == "F8" || Argument == "F9" || Argument == "F10" || Argument == "F11" || Argument == "F12" || Argument == "DELETE" || Argument == "TAB" || Argument == "ENTER"  || Argument == "ESCAPE" || Argument == "ESC") { ArgIsCmd = true; }

          restart: // restart checks

          if (strcmp(Cmd, "REM") == 0)          { Serial.println(" // " + Argument); }                  else { cmdFail++; }
          if (strcmp(Cmd, "DELAY") == 0)        { delay(Argument.toInt()); }                            else { cmdFail++; }
          if (strcmp(Cmd, "DEFAULTDELAY") == 0 || strcmp(Cmd, "DEFAULT_DELAY") == 0) delay(DEF_DELAY);  else { cmdFail++; }  //100ms
          if (strcmp(Cmd, "STRING") == 0)       { Kble.print(Argument);}                                  else { cmdFail++; }
          if (strcmp(Cmd, "STRINGLN") == 0)     { Kble.println(Argument); }                               else { cmdFail++; }
          if (strcmp(Cmd, "SHIFT") == 0)        { Kble.press(KEY_LEFT_SHIFT);                                                         if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}  // Save Cmd into OldCmd and then set Cmd = Argument
          if (strcmp(Cmd, "ALT") == 0)          { Kble.press(KEY_LEFT_ALT);                                                           if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}  // This is made to turn the code faster and to recover
          if (strcmp(Cmd, "CTRL-ALT") == 0)     { Kble.press(KEY_LEFT_ALT); Kble.press(KEY_LEFT_CTRL);                                  if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}  // the Cmd after the if else statements, in order to
          if (strcmp(Cmd, "CTRL-SHIFT") == 0)   { Kble.press(KEY_LEFT_CTRL); Kble.press(KEY_LEFT_SHIFT);                                if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}// the Cmd REPEAT work as intended.
          if (strcmp(Cmd, "CTRL-GUI") == 0)     { Kble.press(KEY_LEFT_CTRL); Kble.press(KEY_LEFT_GUI);                                  if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ALT-SHIFT") == 0)    { Kble.press(KEY_LEFT_ALT); Kble.press(KEY_LEFT_SHIFT);                                 if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ALT-GUI") == 0)      { Kble.press(KEY_LEFT_ALT); Kble.press(KEY_LEFT_GUI);                                   if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "GUI-SHIFT") == 0)    { Kble.press(KEY_LEFT_GUI); Kble.press(KEY_LEFT_SHIFT);                                 if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL-ALT-SHIFT") == 0) { Kble.press(KEY_LEFT_ALT); Kble.press(KEY_LEFT_CTRL); Kble.press(KEY_LEFT_SHIFT);      if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL-ALT-GUI") == 0)   { Kble.press(KEY_LEFT_ALT); Kble.press(KEY_LEFT_CTRL); Kble.press(KEY_LEFT_GUI);        if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ALT-SHIFT-GUI") == 0)  { Kble.press(KEY_LEFT_ALT); Kble.press(KEY_LEFT_SHIFT); Kble.press(KEY_LEFT_GUI);       if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL-SHIFT-GUI") == 0) { Kble.press(KEY_LEFT_CTRL); Kble.press(KEY_LEFT_SHIFT); Kble.press(KEY_LEFT_GUI);      if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "GUI") == 0 || strcmp(Cmd, "WINDOWS") == 0) { Kble.press(KEY_LEFT_GUI);                                     if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL") == 0 || strcmp(Cmd, "CONTROL") == 0) { Kble.press(KEY_LEFT_CTRL);                                   if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ESC") == 0 || strcmp(Cmd, "ESCAPE") == 0) {Kble.press(KEY_ESC);Kble.releaseAll(); } else { cmdFail++;}
          if (strcmp(Cmd, "ENTER") == 0)        { Kble.press(KEY_RETURN); Kble.releaseAll(); }    else { cmdFail++; }
          if (strcmp(Cmd, "DOWNARROW") == 0)    { Kble.press(KEY_DOWN_ARROW); Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "DOWN") == 0)         { Kble.press(KEY_DOWN_ARROW); Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "LEFTARROW") == 0)    { Kble.press(KEY_LEFT_ARROW); Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "LEFT") == 0)         { Kble.press(KEY_LEFT_ARROW); Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "RIGHTARROW") == 0)   { Kble.press(KEY_RIGHT_ARROW);Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "RIGHT") == 0)        { Kble.press(KEY_RIGHT_ARROW);Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "UPARROW") == 0)      { Kble.press(KEY_UP_ARROW);   Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "UP") == 0)           { Kble.press(KEY_UP_ARROW);   Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "BREAK") == 0)        { Kble.press(KEY_PAUSE);      Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "CAPSLOCK") == 0)     { Kble.press(KEY_CAPS_LOCK);  Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PAUSE") == 0)        { Kble.press(KEY_PAUSE);      Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "BACKSPACE") == 0)    { Kble.press(KEYBACKSPACE);   Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "END") == 0)          { Kble.press(KEY_END);        Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "HOME") == 0)         { Kble.press(KEY_HOME);       Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "INSERT") == 0)       { Kble.press(KEY_INSERT);     Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "NUMLOCK") == 0)      { Kble.press(LED_NUMLOCK);    Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PAGEUP") == 0)       { Kble.press(KEY_PAGE_UP);    Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PAGEDOWN") == 0)     { Kble.press(KEY_PAGE_DOWN);  Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PRINTSCREEN") == 0)  { Kble.press(KEY_PRINT_SCREEN);Kble.releaseAll();}else { cmdFail++;}
          if (strcmp(Cmd, "SCROLLOCK") == 0)    { Kble.press(KEY_SCROLL_LOCK);Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "MENU") == 0)         { Kble.press(KEY_MENU);       Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F1") == 0)           { Kble.press(KEY_F1);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F2") == 0)           { Kble.press(KEY_F2);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F3") == 0)           { Kble.press(KEY_F3);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F4") == 0)           { Kble.press(KEY_F4);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F5") == 0)           { Kble.press(KEY_F5);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F6") == 0)           { Kble.press(KEY_F6);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F7") == 0)           { Kble.press(KEY_F7);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F8") == 0)           { Kble.press(KEY_F8);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F9") == 0)           { Kble.press(KEY_F9);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F10") == 0)          { Kble.press(KEY_F10);        Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F11") == 0)          { Kble.press(KEY_F11);        Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F12") == 0)          { Kble.press(KEY_F12);        Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "TAB") == 0)          { Kble.press(KEYTAB);         Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "DELETE") == 0)       { Kble.press(KEY_DELETE);     Kble.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "SPACE") ==0)         { Kble.press(KEY_SPACE);      Kble.releaseAll();} else { cmdFail++;}

          if (ArgIsCmd) strcpy(Cmd, OldCmd);  // Recover the command to run in case of REPEAT

          Kble.releaseAll();

          if (tft.getCursorY()>(HEIGHT-LH)) {
            tft.setCursor(0, 0);
            tft.fillScreen(bruceConfig.bgColor);
          }

          if (cmdFail == 57) {
            tft.setTextColor(ALCOLOR);
            tft.print(Command);
            tft.println(" -> Not Supported, running as STRINGLN");
            if (Argument != "") {
              Kble.print(Command);
              Kble.print(" ");
              Kble.println(Argument);
            } else {
              Kble.println(Command);
            }
          } else {
            tft.setTextColor(bruceConfig.priColor);
            tft.print(Command);
          }
          if(Argument.length()>0) {
            tft.setTextColor(TFT_WHITE);
            tft.println(Argument);
          } else tft.println();
          if (strcmp(Cmd, "REM") != 0) delay(DEF_DELAY);  //if command is not a comment, wait DEF_DELAY until next command (100ms)
        }
      }
      tft.setTextSize(FM);
      payloadFile.close();
      Serial.println("Finished badusb payload execution...");
    }
  } else {
    // rick
    Serial.println("rick");
    tft.setCursor(0, 40);
    tft.println("rick");
    Kble.press(KEY_LEFT_GUI);
    Kble.press('r');
    Kble.releaseAll();
    delay(1000);
    Kble.print("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    Kble.press(KEY_RETURN);
    Kble.releaseAll();
  }

  delay(1000);
  Kble.releaseAll();
}

bool kbChosen_ble = false;

void chooseKb_ble(const uint8_t *layout) {
  kbChosen_ble = true;
  if(!Kble.isConnected()) Kble.begin(layout);
  else Kble.setLayout(layout); // If people connects to media controller and switch to BadBLE, he can set the layout from here
}
bool ask_restart() {
  if(Ask_for_restart==2) { // it'll be set to 2 if it was 1 and disconnect bluetooth
    displayError("Restart Device");
    returnToMenu=true;
    return true;
  }
  return false;
}


void ble_setup() {
  if(ask_restart()) return;
  FS *fs;
  Serial.println("BadBLE begin");
  bool first_time=true;
  int index=0;
NewScript:
  tft.fillScreen(bruceConfig.bgColor);
  String bad_script = "";
  bad_script = "/badpayload.txt";

  options = { };

  if(setupSdCard()) {
    options.push_back({"SD Card", [&]()  { fs=&SD; }});
  }
  options.push_back({"LittleFS",  [&]()   { fs=&LittleFS; }});
  options.push_back({"Main Menu", [&]()   { fs=nullptr; }});

  delay(250);
  loopOptions(options);
  delay(250);

  if(fs!=nullptr) {
    bad_script = loopSD(*fs,true);
    tft.fillScreen(bruceConfig.bgColor);
    if(first_time) {
      options = {
        {"US Inter",    [=]() { chooseKb_ble(KeyboardLayout_en_US); }},
        {"PT-BR ABNT2", [=]() { chooseKb_ble(KeyboardLayout_pt_BR); }},
        {"PT-Portugal", [=]() { chooseKb_ble(KeyboardLayout_pt_PT); }},
        {"AZERTY FR",   [=]() { chooseKb_ble(KeyboardLayout_fr_FR); }},
        {"es-Espanol",  [=]() { chooseKb_ble(KeyboardLayout_es_ES); }},
        {"it-Italiano", [=]() { chooseKb_ble(KeyboardLayout_it_IT); }},
        {"en-UK",       [=]() { chooseKb_ble(KeyboardLayout_en_UK); }},
        {"de-DE",       [=]() { chooseKb_ble(KeyboardLayout_de_DE); }},
        {"sv-SE",       [=]() { chooseKb_ble(KeyboardLayout_sv_SE); }},
        {"da-DK",       [=]() { chooseKb_ble(KeyboardLayout_da_DK); }},
        {"hu-HU",       [=]() { chooseKb_ble(KeyboardLayout_hu_HU); }},
        {"Main Menu",   [=]() { returnToMenu=true; }},
      };
      delay(250);
      index=loopOptions(options,false,true,"Keyboard Layout",index); // It will ask for the keyboard each time, but will save the last chosen to be faster
      delay(250);
      if(returnToMenu) return;
      if (!kbChosen_ble) Kble.begin(); // starts the KeyboardLayout_en_US as default if nothing had beed chosen (cancel selection)
      Ask_for_restart=1; // arm the flag
      first_time=false;
      displayRedStripe("Waiting Victim",TFT_WHITE, bruceConfig.priColor);
    }
    while (!Kble.isConnected() && !checkEscPress());

    if(Kble.isConnected())  {
      BLEConnected=true;
      displayRedStripe("Preparing",TFT_WHITE, bruceConfig.priColor);
      delay(1000);
      displayWarning(String(BTN_ALIAS) + " to deploy", true);
      delay(200);
      key_input_ble(*fs, bad_script);

      displayRedStripe("Payload Sent",TFT_WHITE, bruceConfig.priColor);
      checkSelPress();
      while (!checkSelPress()) {
          // nothing here, just to hold the screen press Ok of M5.
      }
      if(returnToMenu) goto End; // when cancel the run in the middle, go to End to turn off BLE services
      // Try to run a new script on the same device

      goto NewScript;
    }
    else displayWarning("Canceled", true);
  }
End:

  returnToMenu=true;
}



void ble_MediaCommands() {
  if(ask_restart()) return;
  Ask_for_restart=1; // arm the flag

  if(!Kble.isConnected()) Kble.begin();

  displayRedStripe("Pairing...",TFT_WHITE, bruceConfig.priColor);

  while (!Kble.isConnected() && !checkEscPress());

  if(Kble.isConnected())  {
    BLEConnected=true;
    drawMainBorder();
    int index=0;

  reMenu:
    options={
      {"ScreenShot",  [=](){ Kble.press(KEY_PRINT_SCREEN); Kble.releaseAll(); }},
      {"Play/Pause",  [=](){ Kble.press(KEY_MEDIA_PLAY_PAUSE); Kble.releaseAll(); }},
      {"Stop",        [=](){ Kble.press(KEY_MEDIA_STOP); Kble.releaseAll(); }},
      {"Next Track",  [=](){ Kble.press(KEY_MEDIA_NEXT_TRACK); Kble.releaseAll(); }},
      {"Prev Track",  [=](){ Kble.press(KEY_MEDIA_PREVIOUS_TRACK); Kble.releaseAll(); }},
      {"Volume +",    [=](){ Kble.press(KEY_MEDIA_VOLUME_UP); Kble.releaseAll(); }},
      {"Volume -",    [=](){ Kble.press(KEY_MEDIA_VOLUME_DOWN); Kble.releaseAll(); }},
      {"Mute",        [=](){ Kble.press(KEY_MEDIA_MUTE); Kble.releaseAll(); }},
      //{"", [=](){ Kble.press(); Kble.releaseAll(); }},
      {"Main Menu", [=](){ returnToMenu=true;}},
    };
    delay(250);
    index=loopOptions(options,index);
    delay(250);
    if(!returnToMenu) goto reMenu;
  }
  returnToMenu=true;

}

#if defined(HAS_KEYBOARD)
//Now cardputer works as a BLE Keyboard!

void ble_keyboard() {
  if(ask_restart()) return;

  drawMainBorder();
  options = {
    {"US Inter",    [=]() { chooseKb_ble(KeyboardLayout_en_US); }},
    {"PT-BR ABNT2", [=]() { chooseKb_ble(KeyboardLayout_pt_BR); }},
    {"PT-Portugal", [=]() { chooseKb_ble(KeyboardLayout_pt_PT); }},
    {"AZERTY FR",   [=]() { chooseKb_ble(KeyboardLayout_fr_FR); }},
    {"es-Espanol",  [=]() { chooseKb_ble(KeyboardLayout_es_ES); }},
    {"it-Italiano", [=]() { chooseKb_ble(KeyboardLayout_it_IT); }},
    {"en-UK",       [=]() { chooseKb_ble(KeyboardLayout_en_UK); }},
    {"de-DE",       [=]() { chooseKb_ble(KeyboardLayout_de_DE); }},
    {"sv-SE",       [=]() { chooseKb_ble(KeyboardLayout_sv_SE); }},
    {"da-DK",       [=]() { chooseKb_ble(KeyboardLayout_da_DK); }},
    {"hu-HU",       [=]() { chooseKb_ble(KeyboardLayout_hu_HU); }},
    {"Main Menu",   [=]() { returnToMenu = true; }},
  };
  delay(200);
  loopOptions(options,false,true,"Keyboard Layout");
  if(returnToMenu) return;
  if (!kbChosen_ble) Kble.begin(); // starts the KeyboardLayout_en_US as default if nothing had beed chosen (cancel selection)
  Ask_for_restart=1;
Reconnect:
  displayRedStripe("Pair to start",TFT_WHITE, bruceConfig.priColor);

  while (!Kble.isConnected() && !checkEscPress()); // loop to wait for the connection callback or ESC

  if(Kble.isConnected())  {
    BLEConnected=true;

    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.setTextSize(FP);
    drawMainBorder();
    tft.setCursor(10,28);
    tft.println("BLE Keyboard:");
    tft.drawCentreString("> " + String(KB_HID_EXIT_MSG) + " <", WIDTH / 2, HEIGHT-20,1);
    tft.setTextSize(FM);
    String _mymsg="";
    keyStroke key;
    while(Kble.isConnected()) {
      key=_getKeyPress();
      if (key.pressed) {
        if(key.enter) Kble.println();
        else {
          for(char k : key.word) {
            Kble.press(k);
          }
        }
        if(key.fn && key.exit_key) break;
        
        Kble.releaseAll();

        // only text for tft
        String keyStr = "";
        for (auto i : key.word) {
          if (keyStr != "") {
            keyStr = keyStr + "+" + i;
          } else {
            keyStr += i;
          }
        }

        if (keyStr.length() > 0) {
          drawMainBorder(false);

          if(_mymsg.length()>keyStr.length()) tft.drawCentreString("                                  ", WIDTH / 2, HEIGHT / 2,1); // clears screen
          tft.drawCentreString("Pressed: " + keyStr, WIDTH / 2, HEIGHT / 2,1);
          _mymsg=keyStr;
        }
        delay(200);
      }
    }
    if(BLEConnected && !Kble.isConnected()) goto Reconnect;
  }

  returnToMenu=true;
}
#endif
