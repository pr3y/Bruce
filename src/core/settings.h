#include <EEPROM.h>
#include <NTPClient.h>


void setBrightness(int bright, bool save = true);

void getBrightness();

int gsetRotation(bool set = false);

void setDimmerTime(int dimmerTime);

void getDimmerSet();

void setBrightnessMenu();

void setUIColor();

void setRFModuleMenu();

void setRFFreqMenu();

void setRFIDModuleMenu();

void setSleepMode();

void setDimmerTimeMenu();

void setClock();

void runClockLoop();

int gsetIrTxPin(bool set = false);

int gsetIrRxPin(bool set = false);

int gsetRfTxPin(bool set = false);

int gsetRfRxPin(bool set = false);

void getConfigs();

void saveConfigs();

void runClockLoop();
