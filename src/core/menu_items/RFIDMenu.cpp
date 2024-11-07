#include "RFIDMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "modules/rfid/tag_o_matic.h"
#include "modules/rfid/rfid125.h"
#include "modules/rfid/chameleon.h"
#include "modules/rfid/amiibo.h"
#include "modules/rfid/pn532ble.h"

void RFIDMenu::optionsMenu() {
    options = {
        {"Read tag",    [=]()  { TagOMatic(); }},
        {"Read 125kHz", [=]()  { RFID125(); }},
        {"Scan tags",   [=]()  { TagOMatic(TagOMatic::SCAN_MODE); }},
        {"Load file",   [=]()  { TagOMatic(TagOMatic::LOAD_MODE); }},
        {"Erase data",  [=]()  { TagOMatic(TagOMatic::ERASE_MODE); }},
        {"Write NDEF",  [=]()  { TagOMatic(TagOMatic::WRITE_NDEF_MODE); }},
        {"Amiibolink",  [=]()  { Amiibo(); }},
        {"Chameleon",   [=]()  { Chameleon(); }},
        {"PN532 BLE",   [=]()  { Pn532ble(); }},
        {"Config",      [=]()  { configMenu(); }},
        {"Main Menu",   [=]()  { backToMenu(); }},
    };

    delay(200);

    String txt = "RFID";
    if(bruceConfig.rfidModule==M5_RFID2_MODULE)        txt+=" (RFID2)";
    else if(bruceConfig.rfidModule==PN532_I2C_MODULE)  txt+=" (PN532-I2C)";
    else if(bruceConfig.rfidModule==PN532_SPI_MODULE)  txt+=" (PN532-SPI)";
    loopOptions(options,false,true,txt);
}

void RFIDMenu::configMenu() {
    options = {
        {"RFID Module",   [=]() { setRFIDModuleMenu(); }},
        {"Back",          [=]() { optionsMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"RFID Config");
}

String RFIDMenu::getName() {
    return _name;
}

void RFIDMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);
    tft.drawRoundRect(5+iconX,5+iconY,70,70,10,bruceConfig.priColor);
    tft.fillRect(0+iconX,40+iconY,40,40,bruceConfig.bgColor);
    tft.drawCircle(15+iconX,65+iconY,7,bruceConfig.priColor);
    tft.drawArc(15+iconX,65+iconY,18,15,180,270,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawArc(15+iconX,65+iconY,28,25,180,270,bruceConfig.priColor,bruceConfig.bgColor);
    tft.drawArc(15+iconX,65+iconY,38,35,180,270,bruceConfig.priColor,bruceConfig.bgColor);
}