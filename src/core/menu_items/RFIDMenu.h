#ifndef __RFID_MENU_H__
#define __RFID_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The RFIDMenu class.
 *
 * This class is responsible for managing the RFID menu.
 */
class RFIDMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new RFID Menu object
     *
     */
    RFIDMenu() : MenuItemInterface("RFID") {}

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

private:
    /**
     * @brief The config menu.
     */
    void configMenu(void);
};

#endif
