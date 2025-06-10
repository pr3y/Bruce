

#ifndef __Ethernet_MENU_H__
#define __Ethernet_MENU_H__

#include <MenuItemInterface.h>
#if !defined(LITE_VERSION)
class EthernetMenu : public MenuItemInterface {
public:
    EthernetMenu() : MenuItemInterface("Ethernet") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.rfid; }
};

#endif
#endif
