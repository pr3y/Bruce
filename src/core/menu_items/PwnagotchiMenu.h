#ifndef __PWNAGOTCHI_MENU_H__
#define __PWNAGOTCHI_MENU_H__

#include "MenuItemInterface.h"


class PwnagotchiMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    String _name = "Pwnagotchi";

    void configMenu(void);
};

#endif
