#ifndef __RFID_MENU_H__
#define __RFID_MENU_H__

#include "MenuItemInterface.h"


class RFIDMenu : public MenuItemInterface {
public:
    String name = "RFID";

    void optionsMenu(void);
    void draw(void);
    String getName(void);

private:
    void configMenu(void);
};

#endif
