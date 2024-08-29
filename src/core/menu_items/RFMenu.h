#ifndef __RF_MENU_H__
#define __RF_MENU_H__

#include "MenuItemInterface.h"


class RFMenu : public MenuItemInterface {
public:
    String name = "RF";

    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    void configMenu(void);
};

#endif
