#ifndef __CONNECT_MENU_H__
#define __CONNECT_MENU_H__

#include "MenuItemInterface.h"


class ConnectMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void drawIcon(float scale);
    String getName(void);

private:
    String _name = "Connect";

};

#endif
