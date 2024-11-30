#include "FileMenu.h"
#include "core/display.h"
#include "core/sd_functions.h"

void FileMenu::optionsMenu() {
    options = {
        {"SD Card",      [=]() { loopSD(SD); }},
        {"LittleFS",     [=]() { loopSD(LittleFS); }},
        {"Main Menu",    [=]() { backToMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"Files");
}

String FileMenu::getName() {
    return _name;
}

void FileMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);

    tft.drawRect(15+iconX, 5+iconY, 50, 70, bruceConfig.priColor);
    tft.fillRect(50+iconX, 5+iconY, 15, 15, bruceConfig.bgColor);
    tft.drawTriangle(50+iconX, 5+iconY, 50+iconX, 19+iconY, 64+iconX, 19+iconY, bruceConfig.priColor);
}