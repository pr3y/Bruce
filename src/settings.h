#include <EEPROM.h>
#include <NTPClient.h>


void setBrightness(int bright, bool save = true);

void getBrightness(); 

int gsetRotation(bool set = false);

void setBrightnessMenu();

void setClock();

void runClockLoop();