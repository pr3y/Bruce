#ifndef __OTHERS_MENU_H__
#define __OTHERS_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The OthersMenu class.
 *
 * This class is responsible for managing the others menu.
 */
class OthersMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new Others Menu object
     *
     */
    OthersMenu() : MenuItemInterface("Others") {}

    /**
     * @brief The options menu.
     */
    void optionsMenu(void);
    /**
     * @brief Draws the icon.
     *
     * @param scale The scale of the icon.
     */
    void drawIcon(float scale);
    /**
     * @brief Draws the icon image.
     */
    void drawIconImg();
    /**
     * @brief Gets the theme.
     *
     * @return bool True if the theme is enabled, false otherwise.
     */
    bool getTheme() { return bruceConfig.theme.others; }
};

#endif
