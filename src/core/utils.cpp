#include "utils.h"
#include "globals.h"
#include "scrollableTextArea.h"

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


void showDeviceInfo() {
    ScrollableTextArea area = ScrollableTextArea("DEVICE INFO");

    area.addLine("Bruce Version: " + String(BRUCE_VERSION));
    area.addLine("EEPROM size: " + String(EEPROMSIZE));
    area.addLine("");

#ifdef HAS_SCREEN
    area.addLine("[SCREEN]");
    area.addLine("Rotation: " + String(ROTATION));
    area.addLine("Width: " + String(WIDTH) + "px");
    area.addLine("Height: " + String(HEIGHT) + "px");
    area.addLine("");
#endif

    area.addLine("[GPIO]");
    area.addLine("GROVE_SDA: " + String(GROVE_SDA));
    area.addLine("GROVE_SCL: " + String(GROVE_SCL));
    area.addLine("SPI_SCK_PIN: " + String(SPI_SCK_PIN));
    area.addLine("SPI_MOSI_PIN: " + String(SPI_MOSI_PIN));
    area.addLine("SPI_MISO_PIN: " + String(SPI_MISO_PIN));
    area.addLine("SPI_SS_PIN: " + String(SPI_SS_PIN));

    area.show();
}
