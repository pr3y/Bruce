#include "globals.h"


/*********************************************************************
**  Function: backToMenu
**  sets the global var to be be used in the options second parameter
**  and returnToMenu will be user do handle the breaks of all loops

when using loopfunctions with an option to "Back to Menu", use:

add this option:
    options.push_back({"Main Menu", [=]() { backToMenu(); }});

while(1) {
    if(returnToMenu) break; // stop this loop and return to the previous loop

    ...
    loopOptions(options);
    ...
}

/*********************************************************************
**  Function: readFGColorFromEEPROM
**  reads the foreground color from EEPROM
**  if the value is not set, it will use the default value
**********************************************************************/
void readFGCOLORFromEEPROM() {
    int colorEEPROM;

    EEPROM.begin(EEPROMSIZE);
    FGCOLOR = (EEPROM.read(EEPROMSIZE-3)  << 8) | EEPROM.read(EEPROMSIZE-4);

    switch(FGCOLOR){
        case TFT_PURPLE+0x3000: // = 0xA80F;
            break;
        case TFT_WHITE:
            break;
        case TFT_RED:
            break;
        case TFT_DARKGREEN:
            break;
        case TFT_BLUE:
            break;
        case TFT_YELLOW:
            break;
        case TFT_ORANGE:
            break;
        default:
            FGCOLOR = 0xA80F;
            EEPROM.write(EEPROMSIZE-3, 0xA8); 
            EEPROM.write(EEPROMSIZE-4, 0x0F);
            EEPROM.commit();
            break;
    }
    EEPROM.end(); // Free EEPROM memory
}


void backToMenu() {
  returnToMenu=true;
}

void updateTimeStr(struct tm timeInfo) {
  // Atualiza timeStr com a hora e minuto
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min);
}
