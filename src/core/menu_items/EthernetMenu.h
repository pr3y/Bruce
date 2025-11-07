
#ifndef __Ethernet_MENU_H__
#define __Ethernet_MENU_H__

#include "modules/ethernet/EthernetHelper.h"
#include <MenuItemInterface.h>
#if !defined(LITE_VERSION)
/**
 * @brief The EthernetMenu class.
 *
 * This class is responsible for managing the ethernet menu.
 */
class EthernetMenu : public MenuItemInterface {
private:
    EthernetHelper *eth;
    /**
     * @brief Starts the ethernet connection.
     */
    void start_ethernet();

public:
    /**
     * @brief Construct a new Ethernet Menu object
     *
     */
    EthernetMenu() : MenuItemInterface("Ethernet") {}

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
    bool getTheme() { return bruceConfig.theme.rfid; }
};

#endif
#endif
