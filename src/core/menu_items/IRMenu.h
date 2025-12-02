#ifndef __IR_MENU_H__
#define __IR_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class IRMenu : public MenuItemInterface {
public:
    IRMenu() : MenuItemInterface(LANG_IR) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.ir; }

private:
    void configMenu(void);
};

#endif
