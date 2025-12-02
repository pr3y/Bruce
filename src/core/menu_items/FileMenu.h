#ifndef __FILE_MENU_H__
#define __FILE_MENU_H__

#include <MenuItemInterface.h>
#include <i18n.h>

class FileMenu : public MenuItemInterface {
public:
    FileMenu() : MenuItemInterface(LANG_FILES) {}

    void optionsMenu(void);
    void drawIcon(float scale);
    void drawIconImg();
    bool getTheme() { return bruceConfig.theme.files; }
};

#endif
