#ifndef __GPS_MENU_H__
#define __GPS_MENU_H__

#include "MenuItemInterface.h"


class GpsMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void drawIcon(float scale);
    String getName(void);

private:
    String _name = "GPS";

    void configMenu(void);
};

#endif
