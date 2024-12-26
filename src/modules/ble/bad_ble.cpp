#include "core/globals.h"
#include "core/sd_functions.h"
#include "core/main_menu.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "bad_ble.h"

#define DEF_DELAY 100

String apName = "";
BleKeyboard Kble;
bool kbChosen_ble = false;

void initializeBleKeyboard() {
    if (apName == "") {
        apName = keyboard("BadBLE", 30, "BadBLE name :");
    }
    Kble = BleKeyboard(String("Keyboard_" + apName + "_" + String((uint8_t)(ESP.getEfuseMac() >> 32), HEX)).c_str(), "BruceNet", 98);
}

void chooseKb_ble(const uint8_t *layout) {
    kbChosen_ble = true;
    if (!Kble.isConnected()) {
        Kble.begin(layout);  // Initialisation avec la disposition du clavier
    } else {
        Kble.setLayout(layout);  // Changer la disposition si déjà connecté
    }
}

uint8_t Ask_for_restart = 0;

bool ask_restart() {
    if (Ask_for_restart == 2) {  // Il sera défini sur 2 si c'était 1 et que Bluetooth est déconnecté
        displayError("Restart Device");
        returnToMenu = true;
        return true;
    }
    return false;
}

void key_input_ble(FS fs, String bad_script) {
    if (fs.exists(bad_script) && bad_script != "") {
        File payloadFile = fs.open(bad_script, "r");
        if (payloadFile) {
            tft.setCursor(0, 40);
            tft.println("from file!");
            String lineContent = "";
            String Command = "";
            char Cmd[15];
            String Argument = "";
            String RepeatTmp = "";
            char ArgChar;
            bool ArgIsCmd;
            int cmdFail;
            int line;

            Kble.releaseAll();
            tft.setTextSize(1);
            tft.setCursor(0, 0);
            tft.fillScreen(bruceConfig.bgColor);
            line = 0;

            while (payloadFile.available()) {
                if (checkSelPress()) {
                    while (checkSelPress());
                    options = {
                        {"Continue", [=]() { yield(); }},
                        {"Main Menu", [=]() { returnToMenu = true; }},
                    };
                    delay(250);
                    loopOptions(options);
                    delay(250);
                    tft.setTextSize(FP);
                }
                if (returnToMenu) break;
                lineContent = payloadFile.readStringUntil('\n');
                if (lineContent.endsWith("\r")) lineContent.remove(lineContent.length() - 1);

                ArgIsCmd = false;
                cmdFail = 0;
                RepeatTmp = lineContent.substring(0, lineContent.indexOf(' '));
                RepeatTmp = RepeatTmp.c_str();
                if (RepeatTmp == "REPEAT") {
                    if (lineContent.indexOf(' ') > 0) {
                        RepeatTmp = lineContent.substring(lineContent.indexOf(' ') + 1);
                        if (RepeatTmp.toInt() == 0) {
                            RepeatTmp = "1";
                            tft.setTextColor(ALCOLOR);
                            tft.println("REPEAT argument NaN, repeating once");
                        }
                    } else {
                        RepeatTmp = "1";
                        tft.setTextColor(ALCOLOR);
                        tft.println("REPEAT without argument, repeating once");
                    }
                } else {
                    Command = lineContent.substring(0, lineContent.indexOf(' '));
                    strcpy(Cmd, Command.c_str());
                    if (lineContent.indexOf(' ') > 0) Argument = lineContent.substring(lineContent.indexOf(' ') + 1);
                    else Argument = "";
                    RepeatTmp = "1";
                }
                uint16_t i;
                for (i = 0; i < RepeatTmp.toInt(); i++) {
                    char OldCmd[15];
                    Argument = Argument.c_str();
                    ArgChar = Argument.charAt(0);

                    restart:
                    if (strcmp(Cmd, "REM") == 0) { Serial.println(" // " + Argument); }
                    else { cmdFail++; }
                    if (strcmp(Cmd, "DELAY") == 0) { delay(Argument.toInt()); }
                    else { cmdFail++; }
                    if (strcmp(Cmd, "DEFAULTDELAY") == 0 || strcmp(Cmd, "DEFAULT_DELAY") == 0) delay(DEF_DELAY);
                    else { cmdFail++; }
                    if (strcmp(Cmd, "STRING") == 0) { Kble.print(Argument); }
                    else { cmdFail++; }
                    if (strcmp(Cmd, "STRINGLN") == 0) { Kble.println(Argument); }
                    else { cmdFail++; }
                    if (strcmp(Cmd, "SHIFT") == 0) { Kble.press(KEY_LEFT_SHIFT); if (!ArgIsCmd) { Kble.press(ArgChar); Kble.releaseAll(); } else { strcpy(OldCmd, Cmd); strcpy(Cmd, Argument.c_str()); goto restart; } }
                    else { cmdFail++; }

                    if (ArgIsCmd) strcpy(Cmd, OldCmd);

                    Kble.releaseAll();

                    if (tft.getCursorY() > (HEIGHT - LH)) {
                        tft.setCursor(0, 0);
                        tft.fillScreen(bruceConfig.bgColor);
                    }

                    if (cmdFail == 57) {
                        tft.setTextColor(ALCOLOR);
                        tft.print(Command);
                        tft.println(" -> Not Supported, running as STRINGLN");
                        if (Argument != "") {
                            Kble.print(Command);
                            Kble.print(" ");
                            Kble.println(Argument);
                        } else {
                            Kble.println(Command);
                        }
                    } else {
                        tft.setTextColor(bruceConfig.priColor);
                        tft.print(Command);
                    }
                    if (Argument.length() > 0) {
                        tft.setTextColor(TFT_WHITE);
                        tft.println(Argument);
                    } else tft.println();
                    if (strcmp(Cmd, "REM") != 0) delay(DEF_DELAY);
                }
            }
            tft.setTextSize(FM);
            payloadFile.close();
            Serial.println("Finished badusb payload execution...");
        }
    } else {
        Serial.println("rick");
        tft.setCursor(0, 40);
        tft.println("rick");
        Kble.press(KEY_LEFT_GUI);
        Kble.press('r');
        Kble.releaseAll();
        delay(1000);
        Kble.print("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
        Kble.press(KEY_RETURN);
        Kble.releaseAll();
    }

    delay(1000);
    Kble.releaseAll();
}

void ble_setup() {
    initializeBleKeyboard();
    if (ask_restart()) return;
    FS *fs;
    Serial.println("BadBLE begin");
    bool first_time = true;
    int index = 0;
NewScript:
    tft.fillScreen(bruceConfig.bgColor);
    String bad_script = "";
    bad_script = "/badpayload.txt";

    options = { };

    if (setupSdCard()) {
        options.push_back({"SD Card", [&]() { fs = &SD; }});
    }
    options.push_back({"LittleFS", [&]() { fs = &LittleFS; }});
    options.push_back({"Main Menu", [&]() { fs = nullptr; }});

    delay(250);
    loopOptions(options);
    delay(250);

    if (fs != nullptr) {
        bad_script = loopSD(*fs, true);
        tft.fillScreen(bruceConfig.bgColor);
        if (first_time) {
            options = {
                {"US Inter", [=]() { chooseKb_ble(KeyboardLayout_en_US); }},
                {"PT-BR ABNT2", [=]() { chooseKb_ble(KeyboardLayout_pt_BR); }},
                {"PT-Portugal", [=]() { chooseKb_ble(KeyboardLayout_pt_PT); }},
                {"AZERTY FR", [=]() { chooseKb_ble(KeyboardLayout_fr_FR); }},
                {"es-Espanol", [=]() { chooseKb_ble(KeyboardLayout_es_ES); }},
                {"it-Italiano", [=]() { chooseKb_ble(KeyboardLayout_it_IT); }},
                {"en-UK", [=]() { chooseKb_ble(KeyboardLayout_en_UK); }},
                {"de-DE", [=]() { chooseKb_ble(KeyboardLayout_de_DE); }},
                {"sv-SE", [=]() { chooseKb_ble(KeyboardLayout_sv_SE); }},
                {"da-DK", [=]() { chooseKb_ble(KeyboardLayout_da_DK); }},
                {"hu-HU", [=]() { chooseKb_ble(KeyboardLayout_hu_HU); }},
                {"tr-TR", [=]() { chooseKb_ble(KeyboardLayout_tr_TR); }},
                {"Main Menu", [=]() { returnToMenu = true; }},
            };
            delay(250);
            index = loopOptions(options, false, true, "Keyboard Layout", index);
            delay(250);
            if (returnToMenu) return;
            if (!kbChosen_ble) Kble.begin();
            Ask_for_restart = 1;
            first_time = false;
            displaySomething("Waiting Victim");
        }
        while (!Kble.isConnected() && !checkEscPress());

        if (Kble.isConnected()) {
            BLEConnected = true;
            displaySomething("Preparing");
            delay(1000);
            displayWarning(String(BTN_ALIAS) + " to deploy", true);
            delay(200);
            key_input_ble(*fs, bad_script);

            displaySomething("Payload Sent", true);
            if (returnToMenu) goto End;

            goto NewScript;
        } else displayWarning("Canceled", true);
    }
End:

    returnToMenu = true;
}
