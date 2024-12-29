#ifndef __CONFIG_MENU_H__
#define __CONFIG_MENU_H__

#include <MenuItemInterface.h>


class ConfigMenu : public MenuItemInterface {
public:
    ConfigMenu() : MenuItemInterface("Config") {}

    void optionsMenu(void);
    void drawIcon(float scale);

private:
    void devMenu(void);
};

#endif
