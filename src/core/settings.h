#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "config.h"
#include "configPins.h"
#include <NTPClient.h>

void _setBrightness(uint8_t brightval) __attribute__((weak));

void setBrightness(uint8_t brightval, bool save = true);

void getBrightness();

int gsetRotation(bool set = false);

void setBrightnessMenu();

void setUIColor();

bool setCustomUIColorMenu();

void setCustomUIColorChoiceMenu(int colorType);

void setCustomUIColorSettingMenuR(int colorType);

void setCustomUIColorSettingMenuG(int colorType);

void setCustomUIColorSettingMenuB(int colorType);

void setCustomUIColorSettingMenu(
    int colorType, int rgb, std::function<uint16_t(uint16_t, int)> colorGenerator
);

void addEvilWifiMenu();

void removeEvilWifiMenu();

void setRFModuleMenu();

void setRFFreqMenu();

void setRFIDModuleMenu();

void addMifareKeyMenu();

void setSleepMode();

void setDimmerTimeMenu();

void setClock();

void runClockLoop();

int gsetIrTxPin(bool set = false);

void setIrTxRepeats();

int gsetIrRxPin(bool set = false);

int gsetRfTxPin(bool set = false);

int gsetRfRxPin(bool set = false);

void runClockLoop();

void setSoundConfig();

void setSoundVolume();

#ifdef HAS_RGB_LED
void setLedBlinkConfig();
#endif

void setWifiStartupConfig();

void setStartupApp();

void setGpsBaudrateMenu();

void setNetworkCredsMenu();

void setSPIPinsMenu(BruceConfigPins::SPIPins &value);

void setTheme();

void setMacAddressMenu();

#endif
