#ifndef __WIFI_MENU_H__
#define __WIFI_MENU_H__

#include <MenuItemInterface.h>

class WifiMenu : public MenuItemInterface {
public:
    WifiMenu() : MenuItemInterface("WiFi") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.wifi; }

private:
    void configMenu(void);
};

#endif
