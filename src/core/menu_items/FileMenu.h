#ifndef __FILE_MENU_H__
#define __FILE_MENU_H__

#include <MenuItemInterface.h>

/**
 * @brief The FileMenu class.
 *
 * This class is responsible for managing the file menu.
 */
class FileMenu : public MenuItemInterface {
public:
    /**
     * @brief Construct a new File Menu object
     *
     */
    FileMenu() : MenuItemInterface("Files") {}

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
    bool getTheme() { return bruceConfig.theme.files; }
};

#endif
