#ifndef __RFID_MENU_H__
#define __RFID_MENU_H__

#include "MenuItemInterface.h"


class RFIDMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    String _name = "RFID";

    void configMenu(void);
};

#endif
