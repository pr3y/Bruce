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


**********************************************************************/
void backToMenu() {
  returnToMenu=true;
}

void updateTimeStr(struct tm timeInfo) {
  // Atualiza timeStr com a hora e minuto
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min);
}