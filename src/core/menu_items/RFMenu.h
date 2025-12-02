#ifndef __RF_MENU_H__
#define __RF_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class RFMenu : public MenuItemInterface {
public:
    RFMenu() : MenuItemInterface(LANG_RF) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.rf; }

private:
    void configMenu(void);
};

#endif
