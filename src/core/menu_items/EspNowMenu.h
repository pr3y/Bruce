#ifndef __BUZZER_H__
#define __BUZZER_H__

#include <MenuItemInterface.h>

class EspNowMenu : public MenuItemInterface {
public:
    EspNowMenu() : MenuItemInterface("Esp Now") {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.others; }
};

#endif
