#ifndef __FM_MENU_H__
#define __FM_MENU_H__

#include "MenuItemInterface.h"


class FMMenu : public MenuItemInterface {
public:
    String name = "FM";

    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    void configMenu(void);
};

#endif
