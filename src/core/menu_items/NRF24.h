#ifndef __NRF24_MENU_H__
#define __NRF24_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The NRF24Menu class.
 *
 * This class is responsible for managing the NRF24 menu.
 */
class NRF24Menu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new NRF24 Menu object
     *
     */
    NRF24Menu() : MenuItemInterface("NRF24") {}

    /**
     * @brief The options menu.
     */
    void optionsMenu(void);
    /**
     * @brief The config menu.
     */
    void configMenu(void);
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
    bool getTheme() { return bruceConfig.theme.nrf; }
};

#endif
