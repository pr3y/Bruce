
#include "EthernetMenu.h"
#if !defined(LITE_VERSION)
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/ethernet/ARPScanner.h"
#include "modules/ethernet/EthernetHelper.h"

void EthernetMenu::optionsMenu() {
    options = {
        {"Scan Hosts", [=]() {
             auto eth = EthernetHelper();

             while (!eth.is_connected()) { delay(100); }

             esp_netif_t *esp_netinterface = esp_netif_get_handle_from_ifkey("ETH_SPI_0");
             if (esp_netinterface == nullptr) {
                 Serial.println("Failed to get netif handle");
                 return;
             }
             ARPScanner{esp_netinterface};
             eth.stop();
         }},
    };
    addOptionToMainMenu();

    delay(200);

    loopOptions(options, MENU_TYPE_SUBMENU, "Ethernet");
}

void EthernetMenu::drawIconImg() {
    drawImg(
        *bruceConfig.themeFS(), bruceConfig.getThemeItemImg(bruceConfig.theme.paths.rfid), 0, imgCenterY, true
    );
}
void EthernetMenu::drawIcon(float scale) {
    clearIconArea();

    int iconW = scale * 30;
    int iconH = scale * 40;

    int Y = iconCenterY - 25;

    int smallerH = scale * 16;

    int starterX = iconCenterX - iconW; // X of the first side
    int finalX = iconCenterX + iconW;

    int lineWidth = 2;

    // Draw the main socket structure
    /*
    |-----|
    |     |
    |     |
    */
    tft.drawRect(starterX, Y, lineWidth, iconH, bruceConfig.priColor);

    tft.drawRect(finalX, Y, lineWidth, iconH, bruceConfig.priColor);

    tft.drawRect(starterX, Y, iconW * 2, lineWidth, bruceConfig.priColor);

    // Draw the shorter side to close the first part of socket
    /*
    |-----|
    |     |
    |-   -|
    */
    tft.drawRect(starterX, Y + iconH, smallerH, lineWidth, bruceConfig.priColor);

    tft.drawRect(finalX - smallerH + lineWidth, Y + iconH, smallerH, lineWidth, bruceConfig.priColor);

    // Draw the final enclosure
    /*
    |------|
    |      |
    |-    -|
      |  |
    */
    tft.drawRect(starterX + smallerH, Y + iconH, lineWidth, smallerH, bruceConfig.priColor);
    tft.drawRect(finalX - smallerH + lineWidth, Y + iconH, lineWidth, smallerH, bruceConfig.priColor);

    // Draw the four cable pin at a distance of 15 pixel
    for (size_t i = 0; i < 4; i++) {
        tft.drawRect(starterX + 15 + (i * 15), Y, lineWidth, 16, bruceConfig.priColor);
    }

    // Close the socket calculating width of this side removing from total width, the size of the smaller size
    tft.drawRect(
        starterX + smallerH,
        Y + iconH + smallerH,
        (iconW * 2) - (smallerH * 2) + 4,
        lineWidth,
        bruceConfig.priColor
    );
}
#endif
