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
        &scriptsMenu,
    #endif
        &othersMenu,
        &clockMenu,
        &connectMenu,
        &configMenu,
    };

    _totalItems = _menuItems.size();
}

MainMenu::~MainMenu() {}

void MainMenu::begin(void) {
    options = {};

    std::vector<String> l = bruceConfig.disabledMenus;
    for(int i = 0; i < _totalItems; i++) {
        String itemName = _menuItems[i]->getName();
        if( find(l.begin(), l.end(), itemName)==l.end() ) { // If menu item is not disabled
            options.push_back({ // selected lambda
                _menuItems[i]->getName(),
                [=]() { _menuItems[i]->optionsMenu(); },
                false, //selected = false
                nullptr, // hover lambda
                [](void *menuItem) { // render lambda
                    drawMainBorder(false);

                    MenuItemInterface *obj = static_cast<MenuItemInterface *>(menuItem);
                    float scale = float((float)tftWidth / (float)240);
                    if (bruceConfig.rotation & 0b01) scale = float((float)tftHeight / (float)135);
                    obj->draw(scale);
                    #if defined(HAS_TOUCH)
                    TouchFooter();
                    #endif
                },
                _menuItems[i]
            });
        }
    }
    _currentIndex = loopOptions(options, true, "Main Menu");
};
