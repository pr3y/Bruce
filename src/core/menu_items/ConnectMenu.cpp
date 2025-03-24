
#include "ConnectMenu.h"
#include "core/connect/file_sharing.h"
#include "core/connect/serial_commands.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi_common.h"

void ConnectMenu::optionsMenu() {
  options = {
      {"Send File", [=]() { FileSharing().sendFile(); }},
      {"Recv File", [=]() { FileSharing().receiveFile(); }},
      {"Send Cmds", [=]() { EspSerialCmd().sendCommands(); }},
      {"Recv Cmds", [=]() { EspSerialCmd().receiveCommands(); }},
  };
  addOptionToMainMenu();

  loopOptions(options, false, true, getName().c_str());
}
void ConnectMenu::drawIconImg() {
    if(bruceConfig.theme.connect) {
        FS* fs = nullptr;
        if(bruceConfig.theme.fs == 1) fs=&LittleFS;
        else if (bruceConfig.theme.fs == 2) fs=&SD;
        drawImg(*fs, bruceConfig.getThemeItemImg(bruceConfig.theme.paths.connect), 0, imgCenterY, true);
    }
}
void ConnectMenu::drawIcon(float scale) {
  clearIconArea();

  int iconW = scale * 50;
  int iconH = scale * 40;
  int radius = scale * 7;

  if (iconW % 2 != 0)
    iconW++;
  if (iconH % 2 != 0)
    iconH++;

  tft.fillCircle(iconCenterX - iconW / 2, iconCenterY, radius,
                 bruceConfig.priColor);

  tft.fillCircle(iconCenterX + 0.3 * iconW, iconCenterY - iconH / 2, radius,
                 bruceConfig.priColor);
  tft.fillCircle(iconCenterX + 0.5 * iconW, iconCenterY, radius,
                 bruceConfig.priColor);
  tft.fillCircle(iconCenterX + 0.3 * iconW, iconCenterY + iconH / 2, radius,
                 bruceConfig.priColor);

  tft.drawLine(iconCenterX - iconW / 2, iconCenterY, iconCenterX + 0.3 * iconW,
               iconCenterY - iconH / 2, bruceConfig.priColor);
  tft.drawLine(iconCenterX - iconW / 2, iconCenterY, iconCenterX + 0.5 * iconW,
               iconCenterY, bruceConfig.priColor);
  tft.drawLine(iconCenterX - iconW / 2, iconCenterY, iconCenterX + 0.3 * iconW,
               iconCenterY + iconH / 2, bruceConfig.priColor);
}
