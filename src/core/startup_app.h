/**
 * @file startup_app.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Bruce startup apps
 * @version 0.1
 * @date 2024-11-20
 */

#ifndef __STARTUP_APP_H__
#define __STARTUP_APP_H__

#include <Arduino.h>
#include <map>
#include <vector>

class StartupApp {
public:
    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    StartupApp();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    bool startApp(const String &appName) const;
    std::vector<String> getAppNames() const;

private:
    std::map<String, std::function<void()>> _startupApps = {};
};

#endif
