#include "main_menu.h"
#include <globals.h>
#include "display.h"


MainMenu::MainMenu() {
    _menuItems = {
        &wifiMenu,
        &bleMenu,
    #if !defined(REMOVE_RF_MENU)
        &rfMenu,
    #endif
    #if !defined(REMOVE_RFID_MENU)
        &rfidMenu,
    #endif
        &irMenu,
    #if defined(FM_SI4713)
        &fmMenu,
    #endif
        &fileMenu,
        &gpsMenu,
    #if !defined(REMOVE_NRF_MENU)
        &nrf24Menu,
    #endif
    #if !defined(LITE_VERSION)
        #if !defined(ARDUINO_M5STACK_CORE) && !defined(ARDUINO_M5STACK_CORE2)
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
    _checkDisabledMenus(false);
}

/***************************************************************************************
** Function name: next
** Description:   Função para selecionar o próximo menu
***************************************************************************************/
void MainMenu::next(){
    _currentIndex++;
    if (_currentIndex >= _totalItems) _currentIndex = 0;
    _checkDisabledMenus(true);
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
void MainMenu::draw(float scale) {
    MenuItemInterface* current_menu = _menuItems[_currentIndex];

    drawMainBorder(false);
    current_menu->draw(scale);

    #if defined(HAS_TOUCH)
    TouchFooter();
    #endif
}


void MainMenu::_checkDisabledMenus(bool next_button) {
    MenuItemInterface* current_menu = _menuItems[_currentIndex];
    std::vector<String> l = bruceConfig.disabledMenus;

    String currName = current_menu->getName();
    if( find(l.begin(), l.end(), currName)!=l.end() ) {
        // menu disabled, skip to the next/prev one and re-check
        if(next_button)
            next();
        else
            previous();
    }
}