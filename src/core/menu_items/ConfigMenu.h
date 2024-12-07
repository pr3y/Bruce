#ifndef __CONFIG_MENU_H__
#define __CONFIG_MENU_H__

#include "MenuItemInterface.h"


class ConfigMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(float scale);
    String getName(void);

private:
    String _name = "Config";

    void devMenu(void);
};

#endif
