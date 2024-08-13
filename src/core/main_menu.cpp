#include "globals.h"
#include "display.h"
#include "sd_functions.h"
#include "settings.h"
#include "wg.h"
#include "wifi_common.h"
#include "main_menu.h"

#include "modules/ble/ble_spam.h"
#include "modules/ble/ble_common.h"
#include "modules/others/openhaystack.h"
#include "modules/others/tururururu.h"
#include "modules/others/webInterface.h"
#include "modules/others/qrcode_menu.h"
#include "modules/others/mic.h"
#include "modules/ir/TV-B-Gone.h"
#include "modules/ir/ir_read.h"
#include "modules/rf/rf.h"
#include "modules/rfid/tag_o_matic.h"
#include "modules/rfid/mfrc522_i2c.h"
#include "modules/rfid/rfid125.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/dpwo.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wifi_atks.h"
#include "modules/wifi/wardriving.h"

#ifdef USB_as_HID
#include "modules/others/bad_usb.h"
#endif
#ifdef HAS_RGB_LED
#include "modules/others/led_control.h"
#endif


/**********************************************************************
**  Function: wifiOptions
**  Wifi menu options
**********************************************************************/
void wifiOptions() {
  if(!wifiConnected) {
    options = {
      {"Connect Wifi", [=]()  { wifiConnectMenu(); }},    //wifi_common.h
      {"WiFi AP",      [=]()  { wifiConnectMenu(true); }},//wifi_common.h
    };
  } else {
    options = {
      {"Disconnect",   [=]()  { wifiDisconnect(); }},    //wifi_common.h
    };
  }
  options.push_back({"Wifi Atks", [=]()     { wifi_atk_menu(); }});
  options.push_back({"Wardriving", [=]()    { wardriving_setup(); }});
#ifndef LITE_VERSION
  options.push_back({"TelNET", [=]()        { telnet_setup(); }});
  options.push_back({"SSH", [=]()           { ssh_setup(); }});
#endif
  options.push_back({"Raw Sniffer", [=]()   { sniffer_setup(); }});
  options.push_back({"DPWO", [=]()          { dpwo_setup(); }});
  options.push_back({"Evil Portal", [=]()   { startEvilPortal(); }});
  options.push_back({"Scan Hosts", [=]()    { local_scan_setup(); }});
#ifndef LITE_VERSION
  options.push_back({"Wireguard", [=]()     { wg_setup(); }});
#endif
  options.push_back({"Main Menu", [=]()     { backToMenu(); }});
  delay(200);
  loopOptions(options,false,true,"WiFi");
}


/**********************************************************************
**  Function: bleOptions
**  Bluetooth menu options
**********************************************************************/
void bleOptions() {
  options = {
    {"BLE Beacon",   [=]() { ble_test(); }},
    {"BLE Scan",     [=]() { ble_scan(); }},
    {"AppleJuice",   [=]() { aj_adv(0); }},
    {"SwiftPair",    [=]() { aj_adv(1); }},
    {"Samsung Spam", [=]() { aj_adv(2); }},
    {"SourApple",    [=]() { aj_adv(3); }},
    {"Android Spam", [=]() { aj_adv(4); }},
    {"BT Maelstrom", [=]() { aj_adv(5); }},
    {"Main Menu",    [=]() { backToMenu(); }},
  };
  delay(200);
  loopOptions(options,false,true,"Bluetooth");
}


/**********************************************************************
**  Function: rfOptions
**  Radio frequency menu options
**********************************************************************/
void rfOptions(){
  options = {
    {"Scan/copy",     [=]() { RCSwitch_Read_Raw(); }},
    {"Custom SubGhz", [=]() { otherRFcodes(); }},
    {"Spectrum",      [=]() { rf_spectrum(); }}, //@IncursioHack
    {"Jammer Itmt",   [=]() { rf_jammerIntermittent(); }}, //@IncursioHack
    {"Jammer Full",   [=]() { rf_jammerFull(); }}, //@IncursioHack
    {"Config",        [=]() { rfConfigOptions(); }},
    {"Main Menu",     [=]() { backToMenu(); }},
  };
  delay(200);
  loopOptions(options,false,true,"Radio Frequency");
}


/**********************************************************************
**  Function: rfConfigOptions
**  RF config menu options
**********************************************************************/
void rfConfigOptions(){
  options = {
    {"RF TX Pin",     [=]() { gsetRfTxPin(true);     saveConfigs();}},
    {"RF RX Pin",     [=]() { gsetRfRxPin(true);     saveConfigs();}},
    {"RF Module",     [=]() { setRFModuleMenu();     saveConfigs();}},
    {"RF Frequency",  [=]() { setRFFreqMenu();       saveConfigs();}},
    {"Back",          [=]() { rfOptions(); }},
  };

  delay(200);
  loopOptions(options,false,true,"RF Config");
}


/**********************************************************************
**  Function: rfidOptions
**  RFID menu options
**********************************************************************/
void rfidOptions(){
  options = {
    {"Read tag",    [=]()  { TagOMatic(); }}, //@RennanCockles
    {"Read 125kHz", [=]()  { RFID125(); }}, //@RennanCockles
    {"Load file",   [=]()  { TagOMatic(TagOMatic::LOAD_MODE); }}, //@RennanCockles
    {"Erase data",  [=]()  { TagOMatic(TagOMatic::ERASE_MODE); }}, //@RennanCockles
    {"Write NDEF",  [=]()  { TagOMatic(TagOMatic::WRITE_NDEF_MODE); }}, //@RennanCockles
    {"Main Menu",   [=]()  { backToMenu(); }},
  };
  delay(200);
  loopOptions(options,false,true,"RFID");
}


/**********************************************************************
**  Function: irOptions
**  Infrared menu options
**********************************************************************/
void irOptions(){
  options = {
    {"TV-B-Gone", [=]() { StartTvBGone(); }},
    {"Custom IR", [=]() { otherIRcodes(); }},
    {"IR Read",   [=]() { IrRead(); }},
    {"Config",    [=]() { irConfigOptions(); }},
    {"Main Menu", [=]() { backToMenu(); }}
  };
  delay(200);
  loopOptions(options,false,true,"Infrared");
}


/**********************************************************************
**  Function: irConfigOptions
**  IR config menu options
**********************************************************************/
void irConfigOptions(){
  options = {
    {"Ir TX Pin", [=]() { gsetIrTxPin(true);     saveConfigs();}},
    {"Ir RX Pin", [=]() { gsetIrRxPin(true);     saveConfigs();}},
    {"Back",      [=]() { irOptions(); }},
  };

  delay(200);
  loopOptions(options,false,true,"IR Config");
}


/**********************************************************************
**  Function: otherOptions
**  Other menu options
**********************************************************************/
void otherOptions(){
  options = {
    #ifdef MIC_SPM1423
    {"Mic Spectrum", [=]() { mic_test(); }},
    #endif
    {"QRCodes",      [=]() { qrcode_menu(); }},
    {"SD Card",      [=]() { loopSD(SD); }},
    {"LittleFS",     [=]() { loopSD(LittleFS); }},
    {"WebUI",        [=]() { loopOptionsWebUi(); }},
    {"Megalodon",    [=]() { shark_setup(); }},
    #ifdef USB_as_HID
    {"BadUSB",       [=]()  { usb_setup(); }},
    {"USB Keyboard", [=]()  { usb_keyboard(); }},
    #endif
    #ifdef HAS_RGB_LED
    {"LED Control",  [=]()  { ledrgb_setup(); }}, //IncursioHack
    {"LED FLash",    [=]()  { ledrgb_flash(); }}, // IncursioHack
    #endif
    {"Openhaystack", [=]()  { openhaystack_setup(); }},
    {"Main Menu",    [=]()  { backToMenu(); }},
  };
  delay(200);
  loopOptions(options,false,true,"Others");
}


/**********************************************************************
**  Function: configOptions
**  Config menu options
**********************************************************************/
void configOptions(){
  options = {
    {"Brightness",    [=]() { setBrightnessMenu();   saveConfigs();}},
    {"Dim Time",      [=]() { setDimmerTimeMenu();   saveConfigs();}},
    {"Orientation",   [=]() { gsetRotation(true);    saveConfigs();}},
    {"UI Color",      [=]() { setUIColor();          saveConfigs();}},
    {"Clock",         [=]() { setClock(); }},
    {"Sleep",         [=]() { setSleepMode(); }},
    {"Restart",       [=]() { ESP.restart(); }},
    {"Main Menu",     [=]() { backToMenu(); }},
  };

  delay(200);
  loopOptions(options,false,true,"Config");
}


/**********************************************************************
**  Function:    getMainMenuOptions
**  Description: Get main menu options
**********************************************************************/
void getMainMenuOptions(int index){
  switch(index) {
    case 0:  // Wifi
      wifiOptions();
      break;
    case 1:  // BLE
      bleOptions();
      break;
    case 2: // RF
      rfOptions();
      break;
    case 3: // RFID
      rfidOptions();
      break;
    case 4: // IR
      irOptions();
      break;
    case 5: // Other
      otherOptions();
      break;
    case 6: // Clock
      runClockLoop();
      break;
    case 7: // Config
      configOptions();
      break;
  }
}


/***************************************************************************************
** Function name: drawMainMenu
** Description:   Função para desenhar e mostrar o menu principal
***************************************************************************************/
void drawMainMenu(int index) {
  const char* texts[8] = { "WiFi", "BLE", "RF", "RFID", "IR", "Others", "Clock", "Config" };

  drawMainBorder(false);
  tft.setTextSize(FG);

  switch(index) {
    case 0:
      drawWifi(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
    case 1:
      drawBLE(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
    case 2:
      drawRf(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
    case 3:
      drawRfid(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
    case 4:
      drawIR(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
    case 5:
      drawOther(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
    case 6:
      drawClock(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
    case 7:
      drawCfg(WIDTH/2-40,27+(HEIGHT-134)/2);
      break;
  }

  tft.setTextSize(FM);
  tft.fillRect(10,30+80+(HEIGHT-134)/2, WIDTH-20,LH*FM, BGCOLOR);
  tft.drawCentreString(texts[index],WIDTH/2, 30+80+(HEIGHT-134)/2, SMOOTH_FONT);
  tft.setTextSize(FG);
  tft.drawChar('<',10,HEIGHT/2+10);
  tft.drawChar('>',WIDTH-(LW*FG+10),HEIGHT/2+10);

  #if defined(HAS_TOUCH)
  TouchFooter();
  #endif
}
