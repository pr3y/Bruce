#ifndef __IR_MENU_H__
#define __IR_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The IRMenu class.
 *
 * This class is responsible for managing the IR menu.
 */
class IRMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new IR Menu object
     *
     */
    IRMenu() : MenuItemInterface("IR") {}

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
    bool getTheme() { return bruceConfig.theme.ir; }

private:
    /**
     * @brief The config menu.
     */
    void configMenu(void);
};

#endif
