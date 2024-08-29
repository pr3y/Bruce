#include "main_menu.h"
#include "globals.h"
#include "display.h"

#include "menu_items/MenuItemInterface.h"

#include "menu_items/BleMenu.h"
#include "menu_items/ClockMenu.h"
#include "menu_items/ConfigMenu.h"
#include "menu_items/FMMenu.h"
#include "menu_items/IRMenu.h"
#include "menu_items/OthersMenu.h"
#include "menu_items/RFIDMenu.h"
#include "menu_items/RFMenu.h"
#include "menu_items/WifiMenu.h"

// Create instances of menu items
BleMenu bleMenu;
ClockMenu clockMenu;
ConfigMenu configMenu;
FMMenu fmMenu;
IRMenu irMenu;
OthersMenu othersMenu;
RFIDMenu rfidMenu;
RFMenu rfMenu;
WifiMenu wifiMenu;

// Create an array of MenuItemInterface pointers
MenuItemInterface* menuItems[] = {
    &wifiMenu,
    &bleMenu,
    &rfMenu,
    &rfidMenu,
    &irMenu,
    &fmMenu,
    &othersMenu,
    &clockMenu,
    &configMenu,
};

/**********************************************************************
**  Function:    getMainMenuOptions
**  Description: Get main menu options
**********************************************************************/
void getMainMenuOptions(int index){
    menuItems[index]->optionsMenu();
}


/***************************************************************************************
** Function name: drawMainMenu
** Description:   Função para desenhar e mostrar o menu principal
***************************************************************************************/
void drawMainMenu(int index) {
    MenuItemInterface* current_menu = menuItems[index];

    drawMainBorder(false);
    // Fix draw main menu icon remaining lines for those smaller than others
    tft.fillRect(40, 40, WIDTH-70, HEIGHT-70, BGCOLOR);
    tft.setTextSize(FG);

    current_menu->draw();

    tft.setTextSize(FM);
    tft.fillRect(10,30+80+(HEIGHT-134)/2, WIDTH-20,LH*FM, BGCOLOR);
    tft.drawCentreString(current_menu->getName(), WIDTH/2, 30+80+(HEIGHT-134)/2, 1);
    tft.setTextSize(FG);
    tft.drawChar('<',10,HEIGHT/2+10);
    tft.drawChar('>',WIDTH-(LW*FG+10),HEIGHT/2+10);

    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif
}
