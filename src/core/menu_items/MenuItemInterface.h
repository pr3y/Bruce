#ifndef __MENU_ITEM_INTERFACE_H__
#define __MENU_ITEM_INTERFACE_H__

#include "core/globals.h"


class MenuItemInterface {
public:
    String name;

    virtual ~MenuItemInterface() = default;
    virtual void optionsMenu(void) = 0;
    virtual void draw(void) = 0;
    virtual String getName(void) = 0;

protected:
    int iconX = WIDTH/2 - 40;
    int iconY = 27 + (HEIGHT-134)/2;
};

#endif
