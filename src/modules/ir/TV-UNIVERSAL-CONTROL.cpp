#include "TV-UNIVERSAL-CONTROL.h"

IRsend irsend(26);

Command commands[] = {
    {"TV Power", "NEC", "20", "21"},
    {"Volume Up", "NEC", "20", "1D"},
    {"Volume Down", "NEC", "20", "1E"},
    {"Channel Up", "NEC", "20", "1F"},
    {"Channel Down", "NEC", "20", "20"},
    {"Main Menu", "", "", ""}
};

int command_count = sizeof(commands) / sizeof(commands[0]);
int current_selection = 0;

void setupIRRemote() {
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    irsend.begin();
    drawMenu();
}

void loopIRRemote() {
    M5.update();

    if (M5.BtnA.wasPressed()) { 
        current_selection = (current_selection + 1) % command_count;
        drawMenu();
    }

    if (M5.BtnB.wasPressed()) { 
        executeCommand();
    }

    delay(100);
}

void drawMenu() {
    M5.Lcd.fillScreen(BLACK);

    for (int i = 0; i < command_count; i++) {
        if (i == current_selection) {
            M5.Lcd.setTextColor(YELLOW);
        } else {
            M5.Lcd.setTextColor(WHITE);
        }
        M5.Lcd.setCursor(10, 20 + i * 20);
        M5.Lcd.print(commands[i].name);
    }
}

void executeCommand() {
    String protocol = commands[current_selection].protocol;
    String address = commands[current_selection].address;
    String command = commands[current_selection].command;

    if (protocol == "NEC") {
        sendNECCommand(address, command);
    }
    
    drawMenu();
}

void sendNECCommand(String address, String command) {
    uint16_t addressValue = strtoul(address.c_str(), nullptr, 16);
    uint16_t commandValue = strtoul(command.c_str(), nullptr, 16);
    uint64_t data = irsend.encodeNEC(addressValue, commandValue);
    irsend.sendNEC(data, 32, 10);
    Serial.println("Sent NEC Command");
}

void sendRawCommand(uint16_t frequency, String rawData) {

    Serial.println("Sending Raw Command");
}

bool sendDecodedCommand(String protocol, String value, String bits) {
    if (protocol == "NEC") {
        sendNECCommand(value, bits);
        return true;
    }

    return false;
}
