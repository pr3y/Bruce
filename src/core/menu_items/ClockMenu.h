#ifndef __CLOCK_MENU_H__
#define __CLOCK_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The ClockMenu class.
 *
 * This class is responsible for managing the clock menu.
 */
class ClockMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new Clock Menu object
     *
     */
    ClockMenu() : MenuItemInterface("Clock") {}

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
    bool getTheme() { return bruceConfig.theme.clock; }
};

#endif
