/**
 * @file startup_app.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Bruce startup apps
 * @version 0.1
 * @date 2024-11-20
 */


#include "startup_app.h"

#include "modules/others/gps_tracker.h"
#include "modules/wifi/wardriving.h"


StartupApp::StartupApp() {
    _startupApps["GPS Tracker"] = []() { GPSTracker(); };
    _startupApps["Wardriving"]  = []() { Wardriving(); };
}

bool StartupApp::startApp(const String& appName) const {
    auto it = _startupApps.find(appName);
    if (it == _startupApps.end()) {
        Serial.println("Invalid startup app: " + appName);
        return false;
    }

    it->second();

    delay(200);
    tft.fillScreen(bruceConfig.bgColor);

    return true;
}

std::vector<String> StartupApp::getAppNames() const {
    std::vector<String> keys;
    for (const auto& pair : _startupApps) {
        keys.push_back(pair.first);
    }
    return keys;
}
