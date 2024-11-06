
#include "ScriptsMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "modules/bjs_interpreter/interpreter.h" // for JavaScript interpreter


String getScriptsFolder(FS *&fs) {
    String folder;
    String possibleFolders[] = {"/scripts", "/BruceScripts", "/BruceJS"};
    int listSize = sizeof(possibleFolders) / sizeof(possibleFolders[0]);

    for (int i = 0; i < listSize; i++) {
        if(SD.exists(possibleFolders[i])) {
            fs = &SD;
            return possibleFolders[i];
        }
        if(LittleFS.exists(possibleFolders[i])) {
            fs = &LittleFS;
            return possibleFolders[i];
        }
    }
    return "";
}


std::vector<Option> getScriptsOptionsList() {
    std::vector<Option> opt = {};
    FS* fs;
    String folder = getScriptsFolder(fs);
    if(folder == "") return opt;  // did not find


    File root = fs->open(folder);
    if (!root || !root.isDirectory()) return opt; // not a dir
    File file2 = root.openNextFile();

    while (file2) {
        if (file2.isDirectory()) continue;

        String fileName = String(file2.name());
        if( ! fileName.endsWith(".js") && ! fileName.endsWith(".bjs")) continue;

        String entry_title = String(file2.name());
        entry_title = entry_title.substring(0, entry_title.lastIndexOf("."));  // remove the extension
        opt.push_back(
            {entry_title.c_str(), [=]() { run_bjs_script_headless(*fs, file2.path()); }}
        );

        file2 = root.openNextFile();
    }
    file2.close();
    root.close();

    return opt;
}


void ScriptsMenu::optionsMenu() {
    options = getScriptsOptionsList();

    options.push_back({"Load...",   [=]() { run_bjs_script(); }});
    options.push_back({"Main Menu", [=]() { backToMenu();     }});

    delay(200);
    loopOptions(options,false,true,"Scripts");
}

String ScriptsMenu::getName() {
    return _name;
}

void ScriptsMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,bruceConfig.bgColor);

    tft.drawRect(15+iconX, 5+iconY, 50, 70, bruceConfig.priColor);
    tft.fillRect(50+iconX, 5+iconY, 15, 15, bruceConfig.bgColor);
    tft.drawTriangle(50+iconX, 5+iconY, 50+iconX, 19+iconY, 64+iconX, 19+iconY, bruceConfig.priColor);

    tft.drawLine(25+iconX, 45+iconY, 30+iconX, 50+iconY, bruceConfig.priColor);
    tft.drawLine(25+iconX, 45+iconY, 30+iconX, 40+iconY, bruceConfig.priColor);
    tft.drawLine(35+iconX, 50+iconY, 45+iconX, 40+iconY, bruceConfig.priColor);
    tft.drawLine(55+iconX, 45+iconY, 50+iconX, 50+iconY, bruceConfig.priColor);
    tft.drawLine(55+iconX, 45+iconY, 50+iconX, 40+iconY, bruceConfig.priColor);

}