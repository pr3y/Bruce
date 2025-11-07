#ifndef __FM_MENU_H__
#define __FM_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The FMMenu class.
 *
 * This class is responsible for managing the FM menu.
 */
class FMMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new FM Menu object
     *
     */
    FMMenu() : MenuItemInterface("FM") {}

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
    bool getTheme() { return bruceConfig.theme.fm; }
};

#endif
