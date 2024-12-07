#ifndef __WIFI_MENU_H__
#define __WIFI_MENU_H__

#include "MenuItemInterface.h"


class WifiMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(float scale);
    String getName(void);

private:
    String _name = "WiFi";
};

#endif
