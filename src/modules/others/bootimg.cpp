#include <globals.h>
#include "core/sd_functions.h" // using sd functions called to manage sd files
#include "core/display.h"
#include "core/settings.h"
#include "bootimg.h"

void setBootImage() {
  Serial.println("BadUSB begin");
  tft.fillScreen(bruceConfig.bgColor);

  String bootImage = "/shark.jpg";

  FS *fs;
  bool isSD;

  options = { };

  if(setupSdCard()) {
    options.push_back({"SD Card", [&]()  { fs=&SD;isSD=true; }});
  }
  options.push_back({"LittleFS",  [&]()   { fs=&LittleFS;isSD=false; }});
  options.push_back({"Main Menu", [&]()   { fs=nullptr;isSD=false; }});
  
  FS manipFs = LittleFS;
  if (isSD) {
    FS manipFs = SD;
  }

  loopOptions(options);

  if(fs!=nullptr) {
    bootImage = loopSD(*fs,true,"*.jpg");
    manipFs.remove("/boot.jpg");
    File bootImageFile = manipFs.open("/boot.jpg", "w");
    File chosenImageFile = manipFs.open(bootImage);
    bootImageFile.write(chosenImageFile.read());
  }
}