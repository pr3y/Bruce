#ifndef LITE_VERSION
#include "mood.h"
#include "pwngrid.h"
#include "core/display.h"

void initUi();
void wakeUp();
void drawNearbyMenu();
void drawMood(String face, String phrase, bool broken = false);
void drawTime();
void drawFooterData(uint8_t friends_run = 0, uint8_t friends_tot = 0,
                      String last_friend_name = "", signed int rssi = -1000);
void drawTopCanvas();
void drawBottomCanvas();
void updateUi(bool show_toolbars = false);
#endif
