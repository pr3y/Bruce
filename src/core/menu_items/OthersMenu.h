#ifndef __OTHERS_MENU_H__
#define __OTHERS_MENU_H__

#include "MenuItemInterface.h"


class OthersMenu : public MenuItemInterface {
public:
    String name = "Others";

    void optionsMenu(void);
    void draw(void);
    String getName(void);
};

#endif
