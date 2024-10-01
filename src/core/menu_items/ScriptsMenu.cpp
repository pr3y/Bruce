
#include "ScriptsMenu.h"
#include "core/display.h"
#include "core/settings.h"
#include "modules/bjs_interpreter/interpreter.h" // for JavaScript interpreter


void ScriptsMenu::optionsMenu() {

    String Folder = "/scripts";
    FS* fs = NULL;
    if(SD.exists(Folder)) fs = &SD;
    if(LittleFS.exists(Folder)) fs = &LittleFS;
    if(!fs) return;  // dir not found

    //String fileList[MAXFILES][3];
    //readFs(fs, Folder, fileList, "bjs");

    options = { };

    File root = fs->open(Folder);
    if (!root || !root.isDirectory()) return; // not a dir
    File file2 = root.openNextFile();

    while (file2) {
        if (file2.isDirectory()) continue;
        String fileName = String(file2.name());
        if( ! fileName.endsWith(".js") && ! fileName.endsWith(".bjs")) continue;
        // else append to the choices
        String entry_title = String(file2.name()); entry_title = entry_title.substring(0, entry_title.lastIndexOf("."));  // remove the extension
        options.push_back({entry_title.c_str(), [=]() { run_bjs_script_headless(*fs, file2.path()); }});
        file2 = root.openNextFile();
    }
    file2.close();
    root.close();

    options.push_back({"Load...", [=]()   { run_bjs_script(); }});
    options.push_back({"Main Menu", [=]() { backToMenu(); }});

    delay(200);
    loopOptions(options,false,true,"Scripts");
}

String ScriptsMenu::getName() {
    return _name;
}

void ScriptsMenu::draw() {
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);

    tft.drawRect(15+iconX, 5+iconY, 50, 70, FGCOLOR);
    tft.fillRect(50+iconX, 5+iconY, 15, 15, BGCOLOR);
    tft.drawTriangle(50+iconX, 5+iconY, 50+iconX, 19+iconY, 64+iconX, 19+iconY, FGCOLOR);

    tft.drawLine(25+iconX, 45+iconY, 30+iconX, 50+iconY, FGCOLOR);
    tft.drawLine(25+iconX, 45+iconY, 30+iconX, 40+iconY, FGCOLOR);
    tft.drawLine(35+iconX, 50+iconY, 45+iconX, 40+iconY, FGCOLOR);
    tft.drawLine(55+iconX, 45+iconY, 50+iconX, 50+iconY, FGCOLOR);
    tft.drawLine(55+iconX, 45+iconY, 50+iconX, 40+iconY, FGCOLOR);

}