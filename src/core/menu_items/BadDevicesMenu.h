#ifndef __BADDEVICES_MENU_H__
#define __BADDEVICES_MENU_H__

#include "MenuItemInterface.h"


class BadDevicesMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    String _name = "Bad Devices";
};

#endif