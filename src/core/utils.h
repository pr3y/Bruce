#ifndef __UTILS_H__
#define __UTILS_H__
#include <Arduino.h>
void backToMenu();
void addOptionToMainMenu();
void updateClockTimezone();
#if defined(HAS_RTC)
void updateTimeStr(RTC_TimeTypeDef timeInfo);
#else
void updateTimeStr(struct tm timeInfo);
#endif
void formatTimeStr(int hours, int minutes, int seconds);
void showDeviceInfo();
String getOptionsJSON();
void touchHeatMap(struct TouchPoint t);

#endif
