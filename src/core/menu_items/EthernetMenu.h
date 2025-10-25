

#ifndef __Ethernet_MENU_H__
#define __Ethernet_MENU_H__

#include "modules/ethernet/EthernetHelper.h"
#include <MenuItemInterface.h>
#if !defined(LITE_VERSION)
class EthernetMenu : public MenuItemInterface {
private:
    EthernetHelper *eth;
    void start_ethernet();

public:
    EthernetMenu() : MenuItemInterface("Ethernet") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.ethernet; }
};

#endif
#endif
