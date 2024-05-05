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

void displayRedStripe(String text);

void loopOptions(const std::vector<std::pair<std::string, std::function<void()>>>& options, bool bright = false, bool submenu = false, String subText = "");

void drawOptions(int index,const std::vector<std::pair<std::string, std::function<void()>>>& options, uint16_t fgcolor, uint16_t bgcolor);

void drawSubmenu(int index,const std::vector<std::pair<std::string, std::function<void()>>>& options, String system);

void drawMainMenu(int index = 0);

void listFiles(int index, String fileList[][3]);

void displayScanning();

void progressHandler(int progress, size_t total);

int getBattery();

void drawBatteryStatus();

void drawWifi();

void drawBLE();

void drawRf();

void drawRfid();


void drawOther();

void drawCfg();

#endif
