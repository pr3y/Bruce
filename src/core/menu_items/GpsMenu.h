#ifndef __GPS_MENU_H__
#define __GPS_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class GpsMenu : public MenuItemInterface {
public:
    GpsMenu() : MenuItemInterface(LANG_GPS) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.gps; }

private:
    void configMenu(void);
};

#endif
