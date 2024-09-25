
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
    // draw the icon
    tft.fillRect(iconX,iconY,80,80,BGCOLOR);
    int i=0;
    for(i=0;i<6;i++) {
        tft.drawArc(40+iconX,40+iconY,30,20,15+60*i,45+60*i,FGCOLOR,BGCOLOR,true);
    }
    tft.drawArc(40+iconX,40+iconY,22,8,0,360,FGCOLOR,BGCOLOR,false);
}