#ifndef __OTHERS_MENU_H__
#define __OTHERS_MENU_H__

#include "MenuItemInterface.h"


class OthersMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(float scale);
    String getName(void);

private:
    String _name = "Others";
};

#endif
