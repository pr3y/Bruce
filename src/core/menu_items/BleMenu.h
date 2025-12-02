#ifndef __BLE_MENU_H__
#define __BLE_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class BleMenu : public MenuItemInterface {
public:
    BleMenu() : MenuItemInterface(LANG_BLE) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.ble; }
};

#endif
