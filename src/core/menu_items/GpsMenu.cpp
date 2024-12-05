#include "GpsMenu.h"
#include <math.h>
#include "core/display.h"
#include "core/settings.h"
#include "modules/wifi/wardriving.h"
#include "modules/others/gps_tracker.h"

void GpsMenu::optionsMenu() {
    options = {
        {"Wardriving",  [=]() { Wardriving(); }},
        {"GPS Tracker", [=]() { GPSTracker(); }},
        {"Config",      [=]() { configMenu(); }},
        {"Main Menu",   [=]() { backToMenu(); }}
    };

    delay(200);
    String txt = "GPS (" + String(bruceConfig.gpsBaudrate) + " bps)";
    loopOptions(options,false,true,txt);
}

void GpsMenu::configMenu() {
    options = {
        {"Baudrate", [=]() { setGpsBaudrateMenu(); }},
        {"Back",     [=]() { optionsMenu(); }},
    };

    delay(200);
    loopOptions(options,false,true,"GPS Config");
}

String GpsMenu::getName() {
    return _name;
}

void GpsMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);

    int32_t xi = 40 + iconX;
    int32_t yi = 30 + iconY;

    int r = 18;
    int32_t yt = r/2;
    int32_t xt = sqrt(r*r - (r/2 * r/2));

    tft.fillCircle(xi, yi, r, bruceConfig.priColor);
    tft.fillTriangle(xi-xt, yi+yt, xi+xt, yi+yt, xi, yi+2*r, bruceConfig.priColor);
    tft.fillCircle(xi, yi, r/2, bruceConfig.bgColor);

    tft.drawEllipse(xi, yi+2*r, 1.5*r, r/2, bruceConfig.priColor);
}