#ifndef __CLOCK_MENU_H__
#define __CLOCK_MENU_H__

#include "MenuItemInterface.h"


class ClockMenu : public MenuItemInterface {
public:
    String name = "Clock";

    void optionsMenu(void);
    void draw(void);
    String getName(void);
};

#endif
