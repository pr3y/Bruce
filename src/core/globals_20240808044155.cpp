#include "globals.h"

#define BUZZ_PIN 2

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

/*********************************************************************
**  Function: readFGColorFromEEPROM
**  reads the foreground color from EEPROM
**  if the value is not set, it will use the default value
**********************************************************************/
void readFGCOLORFromEEPROM()
{
    int colorEEPROM;

    EEPROM.begin(EEPROMSIZE);
    EEPROM.get(5, colorEEPROM);

    switch (colorEEPROM)
    {
    case 0:
        FGCOLOR = TFT_PURPLE + 0x3000;
        break;
    case 1:
        FGCOLOR = TFT_WHITE;
        break;
    case 2:
        FGCOLOR = TFT_RED;
        break;
    case 3:
        FGCOLOR = TFT_DARKGREEN;
        break;
    case 4:
        FGCOLOR = TFT_BLUE;
        break;
    case 5:
        FGCOLOR = TFT_YELLOW;
        break;
    case 7:
        FGCOLOR = TFT_ORANGE;
        break;
    default:
        FGCOLOR = TFT_PURPLE + 0x3000;
        EEPROM.put(5, 0);
        EEPROM.commit();
        break;
    }
    EEPROM.end(); // Free EEPROM memory
}

void backToMenu()
{
    returnToMenu = true;
}

void skimmerDetection()
{
    BLEDevice::init("");
    BLEScan *scan = BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(new SkimmerAdvertisedDeviceCallbacks());
    scan->setInterval(1349);
    scan->setWindow(449);

    M5.Display.setTextSize(1.5);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setCursor(0, 0);
    M5.Display.println("Scanning for Skimmers...");

    // Boucle pour gérer l'entrée de l'utilisateur
    while (true)
    {
        M5.update();
        M5Cardputer.update();

        if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER) || M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE))
        {
            waitAndReturnToMenu("Scan Stopped");
            return;
        }
        scan->start(5, false); // Scan pendant 5 secondes
    }
}

void updateTimeStr(struct tm timeInfo)
{
    // Atualiza timeStr com a hora e minuto
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min);
}

void _tone(unsigned int frequency, unsigned long duration = 0UL)
{
    tone(BUZZ_PIN, frequency, duration);
}
