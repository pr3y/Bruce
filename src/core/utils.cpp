#include "utils.h"
#include <globals.h>
#include "scrollableTextArea.h"

/*********************************************************************
**  Function: backToMenu
**  sets the global var to be be used in the options second parameter
**  and returnToMenu will be user do handle the breaks of all loops

when using loopfunctions with an option to "Back to Menu", use:

add this option:
    options.push_back({"Main Menu", [=]() { backToMenu(); }});

while(1) {
    if(returnToMenu) break; // stop this loop and return to the previous loop

    ...
    loopOptions(options);
    ...
}
*/

void backToMenu() {
  returnToMenu=true;
}

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

void updateTimeStr(struct tm timeInfo) {
  // Atualiza timeStr com a hora e minuto
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}

void showDeviceInfo() {
    ScrollableTextArea area = ScrollableTextArea("DEVICE INFO");

    area.addLine("Bruce Version: " + String(BRUCE_VERSION));
    area.addLine("EEPROM size: " + String(EEPROMSIZE));
    area.addLine("");

#ifdef HAS_SCREEN
    area.addLine("[SCREEN]");
    area.addLine("Rotation: " + String(ROTATION));
    area.addLine("Width: " + String(tftWidth) + "px");
    area.addLine("Height: " + String(tftHeight) + "px");
    area.addLine("Brightness: " + String(bruceConfig.bright) + "%");
    area.addLine("");
#endif

    area.addLine("[GPIO]");
    area.addLine("GROVE_SDA: " + String(GROVE_SDA));
    area.addLine("GROVE_SCL: " + String(GROVE_SCL));
    area.addLine("SERIAL TX: " + String(SERIAL_TX));
    area.addLine("SERIAL RX: " + String(SERIAL_RX));
    area.addLine("SPI_SCK_PIN: " + String(SPI_SCK_PIN));
    area.addLine("SPI_MOSI_PIN: " + String(SPI_MOSI_PIN));
    area.addLine("SPI_MISO_PIN: " + String(SPI_MISO_PIN));
    area.addLine("SPI_SS_PIN: " + String(SPI_SS_PIN));
    area.addLine("IR TX: " + String(LED));
    area.addLine("IR RX: " + String(RXLED));
    area.addLine("");

    area.addLine("[BAT]");
    area.addLine("Charge: " + String(getBattery()) + "%");
    #ifdef USE_BQ27220_VIA_I2C
    area.addLine("BQ27220 ADDR: " + String(BQ27220_I2C_ADDRESS));
    area.addLine("Charging: " + String(bq.getIsCharging()));
    area.addLine("Charging Voltage: " + String(((double)bq.getVolt(VOLT_MODE::VOLT_CHARGING)/1000.0)) + "V");
    area.addLine("Charging Current: " + String(bq.getCurr(CURR_MODE::CURR_CHARGING) + "mA"));
    area.addLine("Time to Empty: " + String((bq.getTimeToEmpty()/1440)) + " days, " + String(((bq.getTimeToEmpty()%1440)/60)) + " hrs," + String(((bq.getTimeToEmpty()%1440)%60)) + " mins");
    area.addLine("Avg Power Use: " + String(bq.getAvgPower()) + "mW");
    area.addLine("Avg Current: " + String(bq.getCurr(CURR_MODE::CURR_AVERAGE)) + "mA");
    area.addLine("Voltage: " + String(((double)bq.getVolt(VOLT_MODE::VOLT)/1000.0)) + "V");
    area.addLine("Raw Voltage: " + String(bq.getVolt(VOLT_MODE::VOLT_RWA)) + "mV");
    area.addLine("Avg Current: " + String(bq.getCurr(CURR_MODE::CURR_AVERAGE)) + "mA");
    area.addLine("Raw Current: " + String(bq.getCurr(CURR_MODE::CURR_RAW)) + "mA");
    #endif
    
    area.show();
}