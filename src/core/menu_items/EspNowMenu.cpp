#include "EspNowMenu.h"
#include "core/display.h"
#include "core/utils.h"

#include "modules/esp_now/espNow.h"

void EspNowMenu::optionsMenu() {
    returnToMenu = false;
    options = {
        {"Message",   [=]() { loopForEspNow(); }},
        {"Pair",      [=]() { startPair(); }    },
        {"Main Menu", [=]() { return; }         },
    };
    // addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Esp Now");
};

void EspNowMenu::drawIconImg() {
    drawImg(
        *bruceConfig.themeFS(),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.espnow),
        0,
        imgCenterY,
        true
    );
}

// chatgpt part (: xd
void EspNowMenu::drawIcon(float scale) {
    clearIconArea();

    int centerX = iconCenterX;
    int centerY = iconCenterY;

    // Sizes based on scale
    int chipSize = scale * 30; // size of central chip (square)
    int chipHalf = chipSize / 2;
    int waveLength = scale * 12;
    int waveGap = scale * 6;
    int waveThickness = 2;

    // --- Draw central chip (square) ---
    tft.fillRect(centerX - chipHalf, centerY - chipHalf, chipSize, chipSize, bruceConfig.priColor);

    // --- Draw "antenna waves" (4 pairs of arcs or lines) ---

    // Left waves (2 vertical lines with gaps)
    tft.drawLine(
        centerX - chipHalf - waveGap,
        centerY - waveLength / 2,
        centerX - chipHalf - waveGap,
        centerY + waveLength / 2,
        bruceConfig.priColor
    );
    tft.drawLine(
        centerX - chipHalf - waveGap * 2,
        centerY - waveLength / 3,
        centerX - chipHalf - waveGap * 2,
        centerY + waveLength / 3,
        bruceConfig.priColor
    );

    // Right waves (2 vertical lines with gaps)
    tft.drawLine(
        centerX + chipHalf + waveGap,
        centerY - waveLength / 2,
        centerX + chipHalf + waveGap,
        centerY + waveLength / 2,
        bruceConfig.priColor
    );
    tft.drawLine(
        centerX + chipHalf + waveGap * 2,
        centerY - waveLength / 3,
        centerX + chipHalf + waveGap * 2,
        centerY + waveLength / 3,
        bruceConfig.priColor
    );

    // --- Optionally draw some details on chip to look like a microcontroller ---
    // Small inner rectangle to simulate chip detail
    int detailSize = chipSize * 0.6;
    int detailHalf = detailSize / 2;
    tft.fillRect(centerX - detailHalf, centerY - detailHalf, detailSize, detailSize, bruceConfig.bgColor);
}
