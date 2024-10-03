#include "RFIDMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "modules/rfid/tag_o_matic.h"
#include "modules/rfid/rfid125.h"

void RFIDMenu::optionsMenu() {
    options = {
        {"Read tag",    [=]()  { TagOMatic(); }},
        {"Read 125kHz", [=]()  { RFID125(); }},
        {"Load file",   [=]()  { TagOMatic(TagOMatic::LOAD_MODE); }},
        {"Erase data",  [=]()  { TagOMatic(TagOMatic::ERASE_MODE); }},
        {"Write NDEF",  [=]()  { TagOMatic(TagOMatic::WRITE_NDEF_MODE); }},
        {"Config",      [=]()  { configMenu(); }},
        {"Main Menu",   [=]()  { backToMenu(); }},
    };

    delay(200);

    String txt = "RFID";
    if(RfidModule==0)       txt+=" (RFID2)";
    else if(RfidModule==1)  txt+=" (PN532-I2C)";
    else if(RfidModule==2)  txt+=" (PN532-SPI)";
    loopOptions(options,false,true,txt);
}

void RFIDMenu::configMenu() {
    options = {
        {"RFID Module",   [=]() { setRFIDModuleMenu();     saveConfigs();}},
        {"Back",          [=]() { optionsMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"RFID Config");
}

String RFIDMenu::getName() {
    return _name;
}

void RFIDMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);
    tft.drawRoundRect(5+iconX,5+iconY,70,70,10,FGCOLOR);
    tft.fillRect(0+iconX,40+iconY,40,40,BGCOLOR);
    tft.drawCircle(15+iconX,65+iconY,7,FGCOLOR);
    tft.drawArc(15+iconX,65+iconY,18,15,180,270,FGCOLOR,BGCOLOR);
    tft.drawArc(15+iconX,65+iconY,28,25,180,270,FGCOLOR,BGCOLOR);
    tft.drawArc(15+iconX,65+iconY,38,35,180,270,FGCOLOR,BGCOLOR);
}