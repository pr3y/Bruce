#ifndef __CONFIG_MENU_H__
#define __CONFIG_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class ConfigMenu : public MenuItemInterface {
public:
    ConfigMenu() : MenuItemInterface(LANG_CONFIG) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.config; }

private:
    void devMenu(void);
};

#endif
