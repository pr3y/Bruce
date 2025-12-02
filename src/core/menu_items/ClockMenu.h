#ifndef __CLOCK_MENU_H__
#define __CLOCK_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class ClockMenu : public MenuItemInterface {
public:
    ClockMenu() : MenuItemInterface(LANG_CLOCK) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.clock; }
};

#endif
