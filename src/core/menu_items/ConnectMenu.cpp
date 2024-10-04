
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
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);

    tft.fillCircle(15+iconX, 40+iconY, 7, FGCOLOR);

    tft.fillCircle(55+iconX, 20+iconY, 7, FGCOLOR);
    tft.fillCircle(65+iconX, 40+iconY, 7, FGCOLOR);
    tft.fillCircle(55+iconX, 60+iconY, 7, FGCOLOR);

    tft.drawLine(15+iconX, 40+iconY, 55+iconX, 20+iconY, FGCOLOR);
    tft.drawLine(15+iconX, 40+iconY, 65+iconX, 40+iconY, FGCOLOR);
    tft.drawLine(15+iconX, 40+iconY, 55+iconX, 60+iconY, FGCOLOR);
}