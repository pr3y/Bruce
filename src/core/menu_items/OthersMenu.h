#ifndef __OTHERS_MENU_H__
#define __OTHERS_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class OthersMenu : public MenuItemInterface {
public:
    OthersMenu() : MenuItemInterface(LANG_OTHERS) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.others; }
};

#endif
