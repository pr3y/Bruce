
#include "ConnectMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/connect.h"
#include "core/wifi_common.h"


void ConnectMenu::optionsMenu() {
    options = {
        {"Send File",    [=]() { DeviceConnection().sendFile(); }},
        {"Receive File", [=]() { DeviceConnection().receiveFile(); }},
        {"Main Menu",    [=]() { backToMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,getName());
}

String ConnectMenu::getName() {
    return _name;
}

void ConnectMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);

    tft.fillCircle(15+iconX, 40+iconY, 7, bruceConfig.priColor);

    tft.fillCircle(55+iconX, 20+iconY, 7, bruceConfig.priColor);
    tft.fillCircle(65+iconX, 40+iconY, 7, bruceConfig.priColor);
    tft.fillCircle(55+iconX, 60+iconY, 7, bruceConfig.priColor);

    tft.drawLine(15+iconX, 40+iconY, 55+iconX, 20+iconY, bruceConfig.priColor);
    tft.drawLine(15+iconX, 40+iconY, 65+iconX, 40+iconY, bruceConfig.priColor);
    tft.drawLine(15+iconX, 40+iconY, 55+iconX, 60+iconY, bruceConfig.priColor);
}