#include "ReverseShellMenu.h"
#include "core/display.h"
#include "core/sd_functions.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DNSServer.h>

#include "modules/ReverseShell/ReverseShell.h"

// Developed by Fourier (github.com/9dl)
// Use BruceC2 to interact with the reverse shell server
// BruceC2: https://github.com/9dl/Bruce-C2
// To use BruceC2:
// 1. Start Reverse Shell Mode in Bruce
// 2. Start BruceC2 and wait.
// 3. Visit 192.168.4.1 in your browser to access the web interface for shell executing.

// 32bit: https://github.com/9dl/Bruce-C2/releases/download/v1.0/BruceC2_windows_386.exe
// 64bit: https://github.com/9dl/Bruce-C2/releases/download/v1.0/BruceC2_windows_amd64.exe
void ReverseShellMenu::optionsMenu() {
  options.clear();
    options.push_back({"Information",  [=]() { ReverseShellInfo(); }});
    options.push_back({"Run",  [=]() { ReverseShell(); }});
    delay(200);
    loopOptions(options,false,true,"Bluetooth");
}

String ReverseShellMenu::getName() {
    return _name;
}

void ReverseShellMenu::draw() {
    tft.fillRect(iconX, iconY, 80, 80, bruceConfig.bgColor);

    // Terminal
    tft.drawRoundRect(iconX - 12, iconY + 16, 110, 55, 8, bruceConfig.priColor);
    tft.drawRoundRect(iconX - 13, iconY + 15, 112, 57, 8, bruceConfig.priColor);
    tft.drawRoundRect(iconX - 14, iconY + 14, 114, 59, 8, bruceConfig.priColor);

    // Three Dots in the Top Right Corner (Represents terminal options)
    int dotX = iconX + 83; // Adjust X position so dots are inside the terminal window
    int dotY = iconY + 18; // Keep Y position as is

    // Draw the three dots inside the terminal window
    tft.fillCircle(dotX, dotY, 3, bruceConfig.priColor);  // First dot
    tft.fillCircle(dotX + 6, dotY, 3, bruceConfig.priColor);  // Second dot
    tft.fillCircle(dotX + 12, dotY, 3, bruceConfig.priColor); // Third dot
}