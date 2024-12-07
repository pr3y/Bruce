#ifndef __IR_MENU_H__
#define __IR_MENU_H__

#include "MenuItemInterface.h"


class IRMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(float scale);
    String getName(void);

private:
    String _name = "IR";

    void configMenu(void);
};

#endif
