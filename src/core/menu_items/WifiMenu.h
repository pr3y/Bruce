#ifndef __WIFI_MENU_H__
#define __WIFI_MENU_H__

#include "MenuItemInterface.h"


class WifiMenu : public MenuItemInterface {
public:
    String name = "WiFi";

    void optionsMenu(void);
    void draw(void);
    String getName(void);
};

#endif
