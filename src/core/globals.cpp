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
    EEPROM.get(5, colorEEPROM);

    switch(colorEEPROM){
        case 0:
            FGCOLOR = TFT_PURPLE+0x3000;
            break;
        case 1:
            FGCOLOR = TFT_WHITE;
            break;
        case 2:
            FGCOLOR = TFT_RED;
            break;
        case 3:
            FGCOLOR = TFT_DARKGREEN;
            break;
        case 4:
            FGCOLOR = TFT_BLUE;
            break;
        case 5:
            FGCOLOR = TFT_YELLOW;
            break;
        case 7:
            FGCOLOR = TFT_ORANGE;
            break;
        default:
            FGCOLOR = TFT_PURPLE+0x3000;
            EEPROM.put(5, 0);
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
