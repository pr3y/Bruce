#include <EEPROM.h>


extern char timeStr[10];

void setBrightness(int bright, bool save = true);

void getBrightness(); 

int gsetRotation(bool set = false);

void setBrightnessMenu();

void setClock();

void runClockLoop();