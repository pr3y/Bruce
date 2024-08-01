#include "../lib/TFT_eSPI_QRcode/src/qrcode.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/mykeyboard.h"


struct QRCODESTR {
  char name[19];
  String url;
};

void qrcode_display(String qrcodeUrl) {
    // Fetch current brightness
    EEPROM.begin(EEPROMSIZE);
    int oldBrightness = EEPROM.read(2);
    EEPROM.end(); // Free EEPROM memory
    QRcode qrcode (&tft);
    qrcode.init();

    tft.fillScreen(TFT_WHITE);
    // Set max brightness for better readability
    setBrightness(100);

    qrcode.create(qrcodeUrl);
    while(!checkEscPress() and !checkSelPress()) {
        delay(100);
    }
    tft.fillScreen(BGCOLOR);

    // Restore user brightness config
    setBrightness(oldBrightness);
}

void qrcode_menu() {
    options = {};
    QRCODESTR qrcodes[] = {
        { "Bruce AP", "WIFI:T:WPA;S:BruceNet;P:brucenet;;"},
        { "Rickroll", "https://youtu.be/dQw4w9WgXcQ" },
        { "HackerTyper", "https://hackertyper.net/" },
        { "ZomboCom", "https://html5zombo.com/" },
    };

    for(int i=0; i<4; i++) {
        options.push_back({qrcodes[i].name, [&]() { qrcode_display(qrcodes[i].url); }});
    }

    options.push_back({"Main menu", [=]() { backToMenu(); }});

    delay(200);
    loopOptions(options);
    delay(200);
}

