/**
 * @file tag_o_matic.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags
 * @version 0.1
 * @date 2024-07-17
 */

#include "tag_o_matic.h"
#include "globals.h"
#include "mykeyboard.h"
#include "display.h"
#include "sd_functions.h"


TagOMatic::TagOMatic() {
    setup();
}

void TagOMatic::setup() {
    Wire.begin(GROVE_SDA, GROVE_SCL);
    mfrc522.PCD_Init();
    set_state(READ_MODE);
    delay(1000);
    return loop();
}

void TagOMatic::loop() {
    while(1) {
        if (checkEscPress()) {
            returnToMenu=true;
            break;
        }

        if (checkSelPress()) {
            select_state();
        }

        switch (current_state) {
            case READ_MODE:
                read_card();
                break;
            case LOAD_MODE:
                load_uid();
                break;
            case WRITE_MODE:
                write_card();
                break;
            case SAVE_MODE:
                save_uid();
                break;
        }

    }
    Serial.println();
}

void TagOMatic::select_state() {
    options = {
        {"Read", [=]() { set_state(READ_MODE); }},
        {"Load", [=]() { set_state(LOAD_MODE); }},
    };
    if (_read_uid) {
        options.push_back({"Write", [=]() { set_state(WRITE_MODE); }});
        options.push_back({"Save",  [=]() { set_state(SAVE_MODE); }});
    }
    delay(200);
    loopOptions(options);
}

void TagOMatic::set_state(RFID_State state) {
    current_state = state;
    display_banner();
    switch (state) {
        case READ_MODE:
        case LOAD_MODE:
            _read_uid = false;
            break;
        case WRITE_MODE:
            tft.println("New UID: " + get_string_uid(&uid));
            tft.println("");
            break;
        case SAVE_MODE:
            break;
    }
    delay(300);
}

void TagOMatic::cls() {
    tft.fillScreen(BGCOLOR);
    tft.setCursor(0, 0);
    tft.setTextColor(FGCOLOR, BGCOLOR);
}

void TagOMatic::display_banner() {
    cls();
    tft.setTextSize(FM);
    tft.println("TAG-O-MATIC");

    tft.setTextSize(FP);
    tft.println("     RFID2 I2C MFRC522");
    tft.println("     -----------------");

    tft.setTextSize(FM);
    switch (current_state) {
        case READ_MODE:
            tft.println("Read Mode");
            break;
        case LOAD_MODE:
            tft.println("Load Mode");
            break;
        case WRITE_MODE:
            tft.println("Write Mode");
            break;
        case SAVE_MODE:
            tft.println("Save Mode");
            break;
    }

    tft.setTextSize(FP);
    tft.println("");
    tft.println("Press 'ENTER' to change mode.");
    tft.println("");
    tft.println("");
}

void TagOMatic::dump_card_details() {
	byte bcc = 0;

    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
	tft.print(mfrc522.PICC_GetTypeName(piccType));

	// SAK
	tft.print(F(" (SAK "));
	if(mfrc522.uid.sak < 0x10)
		tft.print(F("0"));
	tft.print(mfrc522.uid.sak, HEX);
	tft.println(")");

	// UID
	tft.print(F("UID:"));
	for (byte i = 0; i < mfrc522.uid.size; i++) {
        tft.print(mfrc522.uid.uidByte[i] < 0x10 ? F(" 0") : F(" "));
        tft.print(mfrc522.uid.uidByte[i], HEX);
		bcc = bcc ^ mfrc522.uid.uidByte[i];
	}
    tft.println();

	// BCC
	tft.print(F("BCC: "));
	if(bcc < 0x10)
		tft.print(F("0"));
	tft.println(bcc, HEX);
}

void TagOMatic::read_card() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    display_banner();
    dump_card_details();
    uid = mfrc522.uid;
    _read_uid = true;
    delay(1000);
}

void TagOMatic::write_card() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    display_banner();

    if (mfrc522.MIFARE_SetUid(uid.uidByte, uid.size, true)) {
        displaySuccess("UID written successfully.");
    } else {
        displayError("Error writing UID to tag.");
    }

    mfrc522.PICC_HaltA();
    delay(3000);
    set_state(READ_MODE);
}

String TagOMatic::get_string_uid(MFRC522::Uid *_uid) {
    String uid_str = "";
	for (byte i = 0; i < _uid->size; i++) {
        uid_str += String(_uid->uidByte[i], HEX) + " ";
	}
    uid_str.toUpperCase();
    uid_str.trim();

    return uid_str;
}

void TagOMatic::save_uid() {
    String uid_str = get_string_uid(&uid);
    uid_str.replace(" ", "");
    String filename = keyboard(uid_str, 30, "File name:");

    display_banner();

    if (write_file(filename, uid_str)) {
        displaySuccess("UID file saved.");
    }
    else {
        displayError("Error writing UID file.");
    }
    delay(3000);
    set_state(READ_MODE);
}

bool TagOMatic::write_file(String filename, String uid_str) {
    FS *fs;
    if(setupSdCard()) fs=&SD;
    else fs=&LittleFS;
    File file = (*fs).open("/rfid/" + filename + ".rfid", FILE_WRITE);
    if (!file) {
        return false;
    }
    file.write(uid.size);
    file.write(uid.uidByte, uid.size);
    file.close();
    delay(100);
    return true;
}

void TagOMatic::load_uid() {
    display_banner();

    if (load_from_file()) {
        displaySuccess("UID loaded from file.");
        delay(3000);
        _read_uid = true;
        set_state(WRITE_MODE);
    }
    else {
        displayError("Error loading UID from file.");
        delay(3000);
        set_state(READ_MODE);
    }
}

bool TagOMatic::load_from_file() {
    cls();

    String filepath;
    byte size[1];
    File file;
    FS *fs;

    if(setupSdCard()) fs=&SD;
    else fs=&LittleFS;
    filepath = loopSD(*fs, true);
    file = fs->open(filepath, FILE_READ);

    if (!file) {
        return false;
    }

    while (file.available()) {
        file.readBytesUntil('\n', size, 1);
        uid.size = size[0];
        file.readBytesUntil('\n', uid.uidByte, uid.size);
    }
    file.close();
    delay(100);
    return true;
}
