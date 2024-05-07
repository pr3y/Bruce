// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include "globals.h"

void initDisplay(); // Início da função e mostra bootscreen

//Funções para economizar linhas nas outras funções
void resetTftDisplay(int x = 0, int y = 0, uint16_t fc = FGCOLOR, int size = FM, uint16_t bg = BGCOLOR, uint16_t screen = BGCOLOR);
void setTftDisplay(int x = 0, int y = 0, uint16_t fc = tft.textcolor, int size = tft.textsize, uint16_t bg = tft.textbgcolor);

void resetSpriteDisplay(int x = 0, int y = 0, uint16_t fc = FGCOLOR, int size = FM, uint16_t bg = BGCOLOR, uint16_t screen = BGCOLOR);
void setSpriteDisplay(int x = 0, int y = 0, uint16_t fc = sprite.textcolor, int size = sprite.textsize, uint16_t bg = sprite.textbgcolor);

void displayRedStripe(String text, uint16_t fgcolor = TFT_WHITE, uint16_t bfcolor = TFT_RED);

void loopOptions(const std::vector<std::pair<std::string, std::function<void()>>>& options, bool bright = false, bool submenu = false, String subText = "");

void drawOptions(int index,const std::vector<std::pair<std::string, std::function<void()>>>& options, uint16_t fgcolor, uint16_t bgcolor);

void drawSubmenu(int index,const std::vector<std::pair<std::string, std::function<void()>>>& options, String system);

void drawMainMenu(int index = 0);

void listFiles(int index, String fileList[][3]);

void displayScanning();

void drawWireguardStatus();

void progressHandler(int progress, size_t total);

int getBattery();

void drawBatteryStatus();

void drawWifiSmall(int x, int y);

void drawWifi();

void drawBLESmall(int x, int y);

void drawBLE();

void drawRf();

void drawRfid();


void drawOther();

void drawCfg();

#endif
