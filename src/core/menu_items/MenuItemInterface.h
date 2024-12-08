#ifndef __MENU_ITEM_INTERFACE_H__
#define __MENU_ITEM_INTERFACE_H__

#include "core/globals.h"
#include "core/display.h"


class MenuItemInterface {
public:
    virtual ~MenuItemInterface() = default;
    virtual void optionsMenu(void) = 0;
    virtual void drawIcon(float scale = 1) = 0;

    String getName() const {return _name;}

    void drawArrows(float scale = 1) {
        int arrowAreaX = BORDER_PAD_X;
        int arrowAreaW = iconAreaX - arrowAreaX;

        tft.fillRect(arrowAreaX, iconAreaY, arrowAreaW, iconAreaH, bruceConfig.bgColor);
        tft.fillRect(WIDTH - arrowAreaX - arrowAreaW, iconAreaY, arrowAreaW, iconAreaH, bruceConfig.bgColor);

        int arrowSize = scale * 10;
        int lineWidth = scale * 3;

        int arrowX = BORDER_PAD_X + 1.5*arrowSize;
        int arrowY = iconCenterY +  1.5*arrowSize;

        // Left Arrow
        tft.drawWideLine(
            arrowX,
            arrowY,
            arrowX + arrowSize,
            arrowY + arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
        tft.drawWideLine(
            arrowX,
            arrowY,
            arrowX + arrowSize,
            arrowY - arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );

        // Right Arrow
        tft.drawWideLine(
            WIDTH - arrowX,
            arrowY,
            WIDTH - arrowX - arrowSize,
            arrowY + arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
        tft.drawWideLine(
            WIDTH - arrowX,
            arrowY,
            WIDTH - arrowX - arrowSize,
            arrowY - arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
    }

protected:
    String _name = "";

    int iconAreaH = (
        (HEIGHT - 2*BORDER_PAD_Y) % 2 == 0
        ? HEIGHT - 2*BORDER_PAD_Y
        : HEIGHT - 2*BORDER_PAD_Y + 1
    );
    int iconAreaW = iconAreaH;

    int iconCenterX = WIDTH/2;
    int iconCenterY = HEIGHT/2;

    int iconAreaX = iconCenterX - iconAreaW/2;
    int iconAreaY = iconCenterY - iconAreaH/2;

    MenuItemInterface(const String& name) : _name(name) {}

    void clearIconArea(void) {
        tft.fillRect(iconAreaX, iconAreaY, iconAreaW, iconAreaH, bruceConfig.bgColor);
    }
};

#endif
