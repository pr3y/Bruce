#ifndef __RS_MENU_H__
#define __RS_MENU_H__

#include "MenuItemInterface.h"


class ReverseShellMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    String _name = "Reverse Shell";
};

#endif
