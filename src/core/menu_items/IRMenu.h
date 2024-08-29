#ifndef __IR_MENU_H__
#define __IR_MENU_H__

#include "MenuItemInterface.h"


class IRMenu : public MenuItemInterface {
public:
    String name = "IR";

    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    void configMenu(void);
};

#endif
