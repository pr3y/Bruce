#ifndef __BLE_MENU_H__
#define __BLE_MENU_H__

#include "MenuItemInterface.h"


class BleMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(float scale);
    String getName(void);

private:
    String _name = "BLE";
};

#endif
