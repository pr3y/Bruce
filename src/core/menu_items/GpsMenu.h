#ifndef __GPS_MENU_H__
#define __GPS_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The GpsMenu class.
 *
 * This class is responsible for managing the GPS menu.
 */
class GpsMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new Gps Menu object
     *
     */
    GpsMenu() : MenuItemInterface("GPS") {}

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
    bool getTheme() { return bruceConfig.theme.gps; }

private:
    /**
     * @brief The config menu.
     */
    void configMenu(void);
};

#endif
