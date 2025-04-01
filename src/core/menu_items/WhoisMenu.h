#ifndef __WHOIS_MENU_H__
#define __WHOIS_MENU_H__

#include <MenuItemInterface.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../display.h"
#include "../mykeyboard.h"
#include "../scrollableTextArea.h"

class WhoisMenu : public MenuItemInterface {
public:
    WhoisMenu() : MenuItemInterface("WHOIS") {}

    void optionsMenu(void) override;
    void drawIcon(float scale) override;
    void drawIconImg() override;
    bool getTheme() override { return true; }
    void displayTextScrollable(const char* text);
    void consultarWhois(const String& domain);
    void consultarWhoisDetalhado(const String& domain);

private:
    std::vector<Option> options;
};

#endif
