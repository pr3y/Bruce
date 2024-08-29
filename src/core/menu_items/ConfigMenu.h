#ifndef __CONFIG_MENU_H__
#define __CONFIG_MENU_H__

#include "MenuItemInterface.h"


class ConfigMenu : public MenuItemInterface {
public:
    String name = "Config";

    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    void devMenu(void);
};

#endif
