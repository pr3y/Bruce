#ifndef __BLE_MENU_H__
#define __BLE_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The BleMenu class.
 *
 * This class is responsible for managing the BLE menu.
 */
class BleMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new Ble Menu object
     *
     */
    BleMenu() : MenuItemInterface("BLE") {}

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
    bool getTheme() { return bruceConfig.theme.ble; }
};

#endif
