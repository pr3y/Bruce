#include "main_menu.h"
#include "globals.h"
#include "display.h"


MainMenu::MainMenu() {
    _menuItems = {
        &wifiMenu,
        &bleMenu,
        &rfMenu,
        &rfidMenu,
        &irMenu,
        &fmMenu,
    #if defined(USE_NRF24_VIA_SPI)
        &nrf24Menu,
    #endif
    #if !defined(LITE_VERSION)
        #if !defined(CORE) && !defined(CORE2)
            &scriptsMenu,
        #endif
    #endif
        &othersMenu,
        &clockMenu,
        &connectMenu,
        &configMenu,
    };

    _totalItems = _menuItems.size();
}

MainMenu::~MainMenu() {}

/***************************************************************************************
** Function name: previous
** Description:   Função para selecionar o menu anterior
***************************************************************************************/
void MainMenu::previous(){
    _currentIndex--;
    if (_currentIndex < 0) _currentIndex = _totalItems - 1;
}

/***************************************************************************************
** Function name: next
** Description:   Função para selecionar o próximo menu
***************************************************************************************/
void MainMenu::next(){
    _currentIndex++;
    if (_currentIndex >= _totalItems) _currentIndex = 0;
}


/**********************************************************************
**  Function:    openMenuOptions
**  Description: Get main menu options
**********************************************************************/
void MainMenu::openMenuOptions(){
    _menuItems[_currentIndex]->optionsMenu();
}


/***************************************************************************************
** Function name: draw
** Description:   Função para desenhar e mostrar o menu principal
***************************************************************************************/
void MainMenu::draw() {
    MenuItemInterface* current_menu = _menuItems[_currentIndex];

    drawMainBorder(false);
    // Fix draw main menu icon remaining lines for those smaller than others
    tft.fillRect(40, 40, WIDTH-70, HEIGHT-70, bruceConfig.bgColor);
    tft.setTextSize(FG);

    current_menu->draw();

    tft.setTextSize(FM);
    tft.fillRect(10,30+80+(HEIGHT-134)/2, WIDTH-20,LH*FM, bruceConfig.bgColor);
    tft.drawCentreString(current_menu->getName(), WIDTH/2, 30+80+(HEIGHT-134)/2, 1);
    tft.setTextSize(FG);
    tft.drawChar('<',10,HEIGHT/2+10);
    tft.drawChar('>',WIDTH-(LW*FG+10),HEIGHT/2+10);

    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif
}
