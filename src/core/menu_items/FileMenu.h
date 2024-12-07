#ifndef __FILE_MENU_H__
#define __FILE_MENU_H__

#include "MenuItemInterface.h"


class FileMenu : public MenuItemInterface {
public:
    void optionsMenu(void);
    void draw(float scale);
    String getName(void);

private:
    String _name = "Files";
};

#endif
