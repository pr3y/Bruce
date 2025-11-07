#ifndef __CONNECT_MENU_H__
#define __CONNECT_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The ConnectMenu class.
 *
 * This class is responsible for managing the connect menu.
 */
class ConnectMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new Connect Menu object
     *
     */
    ConnectMenu() : MenuItemInterface("Connect") {}

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
    bool getTheme() { return bruceConfig.theme.connect; }
};

#endif
