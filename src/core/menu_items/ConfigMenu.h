#ifndef __CONFIG_MENU_H__
#define __CONFIG_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The ConfigMenu class.
 *
 * This class is responsible for managing the config menu.
 */
class ConfigMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new Config Menu object
     *
     */
    ConfigMenu() : MenuItemInterface("Config") {}

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
    bool getTheme() { return bruceConfig.theme.config; }

private:
    /**
     * @brief The dev menu.
     */
    void devMenu(void);
};

#endif
