

#ifndef __Ethernet_MENU_H__
#define __Ethernet_MENU_H__

#include <MenuItemInterface.h>
#if !defined(LITE_VERSION) && defined(USE_W5500_VIA_SPI)
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
