#include "FileMenu.h"
#include "core/display.h"
#include "core/sd_functions.h"
#include "modules/others/webInterface.h"

void FileMenu::optionsMenu() {
    options = {
        {"SD Card",      [=]() { loopSD(SD); }},
        {"LittleFS",     [=]() { loopSD(LittleFS); }},
        {"WebUI",        [=]() { loopOptionsWebUi(); }},
        {"Main Menu",    [=]() { backToMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"Files");
}

void FileMenu::drawIcon(float scale) {
    clearIconArea();

    int iconW = scale * 50;
    int iconH = scale * 70;
    int foldSize = scale * 15;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 2 != 0) iconH++;

    // File
    tft.drawRect(
        iconCenterX - iconW/2,
        iconCenterY - iconH/2,
        iconW,
        iconH,
        bruceConfig.priColor
    );
    tft.fillRect(
        iconCenterX + iconW/2 - foldSize,
        iconCenterY - iconH/2,
        foldSize,
        foldSize,
        bruceConfig.bgColor
    );
    tft.drawTriangle(
        (iconCenterX + iconW/2 - foldSize), (iconCenterY - iconH/2),
        (iconCenterX + iconW/2 - foldSize), (iconCenterY - iconH/2 + foldSize - 1),
        (iconCenterX + iconW/2 - 1), (iconCenterY - iconH/2 + foldSize - 1),
        bruceConfig.priColor
    );
}