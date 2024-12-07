#ifndef __RF_MENU_H__
#define __RF_MENU_H__

#include "MenuItemInterface.h"


class RFMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(float scale);
    String getName(void);

private:
    String _name = "RF";

    void configMenu(void);
};

#endif
