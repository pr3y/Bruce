#ifndef __GPS_MENU_H__
#define __GPS_MENU_H__

#include <MenuItemInterface.h>


class GpsMenu : public MenuItemInterface {
public:
    GpsMenu() : MenuItemInterface("GPS") {}

    void optionsMenu(void);
    void drawIcon(float scale);

private:
    void configMenu(void);
};

extern GpsMenu gpsMenu;

#endif
