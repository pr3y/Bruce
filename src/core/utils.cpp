#include "utils.h"
#include "globals.h"

void updateClockTimezone(){
    timeClient.begin();
    timeClient.update();

    timeClient.setTimeOffset(bruceConfig.tmz * 3600);

    localTime = myTZ.toLocal(timeClient.getEpochTime());

#if !defined(HAS_RTC)
    rtc.setTime(timeClient.getEpochTime());
    updateTimeStr(rtc.getTimeStruct());
    clock_set = true;
#endif
}