#ifndef __MENU_ITEM_INTERFACE_H__
#define __MENU_ITEM_INTERFACE_H__

#include <globals.h>
#include "core/display.h"


class MenuItemInterface {
public:
    virtual ~MenuItemInterface() = default;
    virtual void optionsMenu(void) = 0;
    virtual void drawIcon(float scale = 1) = 0;

    String getName() const {return _name;}

    void draw(float scale = 1) {
        drawIcon(scale);
        drawArrows(scale);
        drawTitle(scale);
    }

    void drawArrows(float scale = 1) {
        tft.fillRect(arrowAreaX, iconAreaY, arrowAreaW, iconAreaH, bruceConfig.bgColor);
        tft.fillRect(tftWidth - arrowAreaX - arrowAreaW, iconAreaY, arrowAreaW, iconAreaH, bruceConfig.bgColor);

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
            tftWidth - arrowX,
            arrowY,
            tftWidth - arrowX - arrowSize,
            arrowY + arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
        tft.drawWideLine(
            tftWidth - arrowX,
            arrowY,
            tftWidth - arrowX - arrowSize,
            arrowY - arrowSize,
            lineWidth,
            bruceConfig.priColor,
            bruceConfig.bgColor
        );
    }

    void drawTitle(float scale = 1) {
        int titleY = iconCenterY + iconAreaH/2 + FG;

        tft.setTextSize(FM);
        tft.fillRect(
            arrowAreaX, titleY,
            tftWidth - 2*arrowAreaX, LH*FM,
            bruceConfig.bgColor
        );
        tft.drawCentreString(getName(), iconCenterX, titleY, 1);
    }

protected:
    String _name = "";

    int iconAreaH = (
        (tftHeight - 2*BORDER_PAD_Y) % 2 == 0
        ? tftHeight - 2*BORDER_PAD_Y
        : tftHeight - 2*BORDER_PAD_Y + 1
    );
    int iconAreaW = iconAreaH;

    int iconCenterX = tftWidth/2;
    int iconCenterY = tftHeight/2;

    int iconAreaX = iconCenterX - iconAreaW/2;
    int iconAreaY = iconCenterY - iconAreaH/2;

    int arrowAreaX = BORDER_PAD_X;
    int arrowAreaW = iconAreaX - arrowAreaX;

    MenuItemInterface(const String& name) : _name(name) {}

    void clearIconArea(void) {
        tft.fillRect(iconAreaX, iconAreaY, iconAreaW, iconAreaH, bruceConfig.bgColor);
    }
};

#endif
