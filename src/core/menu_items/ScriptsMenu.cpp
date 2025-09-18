
#include "ScriptsMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/utils.h"
#include "modules/bjs_interpreter/interpreter.h" // for JavaScript interpreter

String getScriptsFolder(FS *&fs) {
    String folder;
    String possibleFolders[] = {"/scripts", "/BruceScripts", "/BruceJS"};
    int listSize = sizeof(possibleFolders) / sizeof(possibleFolders[0]);

    for (int i = 0; i < listSize; i++) {
        if (SD.exists(possibleFolders[i])) {
            fs = &SD;
            return possibleFolders[i];
        }
        if (LittleFS.exists(possibleFolders[i])) {
            fs = &LittleFS;
            return possibleFolders[i];
        }
    }
    return "";
}

std::vector<Option> getScriptsOptionsList() {
    std::vector<Option> opt = {};
#ifndef LITE_VERSION
    FS *fs;
    String folder = getScriptsFolder(fs);
    if (folder == "") return opt; // did not find

    File root = fs->open(folder);
    if (!root || !root.isDirectory()) return opt; // not a dir
    File file2;

    while (file2 = root.openNextFile()) {
        if (file2.isDirectory()) continue;

        String fileName = String(file2.name());
        if (!fileName.endsWith(".js") && !fileName.endsWith(".bjs")) continue;

        String entry_title = String(file2.name());
        entry_title = entry_title.substring(0, entry_title.lastIndexOf(".")); // remove the extension
        opt.push_back({entry_title.c_str(), [=]() { run_bjs_script_headless(*fs, file2.path()); }});
    }

    file2.close();
    root.close();
#endif
    return opt;
}

void ScriptsMenu::optionsMenu() {
#ifndef LITE_VERSION
    options = getScriptsOptionsList();

    options.push_back({"Load...", run_bjs_script});
    addOptionToMainMenu();

    loopOptions(options, MENU_TYPE_SUBMENU, "Scripts");
#endif
}
void ScriptsMenu::drawIconImg() {
    drawImg(
        *bruceConfig.themeFS(),
        bruceConfig.getThemeItemImg(bruceConfig.theme.paths.interpreter),
        0,
        imgCenterY,
        true
    );
}
void ScriptsMenu::drawIcon(float scale) {
    clearIconArea();

    int iconW = scale * 40;
    int iconH = scale * 60;

    if (iconW % 2 != 0) iconW++;
    if (iconH % 2 != 0) iconH++;

    int foldSize = iconH / 4;
    int arrowSize = iconW / 10;
    int arrowPadX = 2 * arrowSize;
    int arrowPadBottom = 3 * arrowPadX;
    int slashSize = 2 * arrowSize;

    // File
    tft.drawRect(iconCenterX - iconW / 2, iconCenterY - iconH / 2, iconW, iconH, bruceConfig.priColor);
    tft.fillRect(
        iconCenterX + iconW / 2 - foldSize, iconCenterY - iconH / 2, foldSize, foldSize, bruceConfig.bgColor
    );
    tft.drawTriangle(
        (iconCenterX + iconW / 2 - foldSize),
        (iconCenterY - iconH / 2),
        (iconCenterX + iconW / 2 - foldSize),
        (iconCenterY - iconH / 2 + foldSize - 1),
        (iconCenterX + iconW / 2 - 1),
        (iconCenterY - iconH / 2 + foldSize - 1),
        bruceConfig.priColor
    );

    // Left Arrow
    tft.drawLine(
        iconCenterX - iconW / 2 + arrowPadX,
        iconCenterY + iconH / 2 - arrowPadBottom,
        iconCenterX - iconW / 2 + arrowPadX + arrowSize,
        iconCenterY + iconH / 2 - arrowPadBottom + arrowSize,
        bruceConfig.priColor
    );
    tft.drawLine(
        iconCenterX - iconW / 2 + arrowPadX,
        iconCenterY + iconH / 2 - arrowPadBottom,
        iconCenterX - iconW / 2 + arrowPadX + arrowSize,
        iconCenterY + iconH / 2 - arrowPadBottom - arrowSize,
        bruceConfig.priColor
    );

    // Slash
    tft.drawLine(
        iconCenterX - slashSize / 2,
        iconCenterY + iconH / 2 - arrowPadBottom + arrowSize,
        iconCenterX + slashSize / 2,
        iconCenterY + iconH / 2 - arrowPadBottom - arrowSize,
        bruceConfig.priColor
    );

    // Right Arrow
    tft.drawLine(
        iconCenterX + iconW / 2 - arrowPadX,
        iconCenterY + iconH / 2 - arrowPadBottom,
        iconCenterX + iconW / 2 - arrowPadX - arrowSize,
        iconCenterY + iconH / 2 - arrowPadBottom + arrowSize,
        bruceConfig.priColor
    );
    tft.drawLine(
        iconCenterX + iconW / 2 - arrowPadX,
        iconCenterY + iconH / 2 - arrowPadBottom,
        iconCenterX + iconW / 2 - arrowPadX - arrowSize,
        iconCenterY + iconH / 2 - arrowPadBottom - arrowSize,
        bruceConfig.priColor
    );
}
