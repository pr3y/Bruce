/**
 * @file tag_o_matic.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags
 * @version 0.1
 * @date 2024-07-17
 */

#include "tag_o_matic.h"
#include "core/globals.h"
#include "core/mykeyboard.h"
#include "core/display.h"
#include "core/sd_functions.h"

#define NDEF_DATA_SIZE 100
#define RFID_I2C_ADDR 0x28


TagOMatic::TagOMatic() {
    _initial_state = READ_MODE;
    setup();
}

TagOMatic::TagOMatic(RFID_State initial_state) {
    if (initial_state == CLONE_MODE || initial_state == WRITE_MODE || initial_state == SAVE_MODE) {
        initial_state = READ_MODE;
    }
    _initial_state = initial_state;
    setup();
}

void TagOMatic::setup() {
    Wire.begin(GROVE_SDA, GROVE_SCL);

    if (!setup_mfrc522()) {
        displayError("RFID module not found!");
        delay(2000);
        return;
    }

    mfrc522.SetChipAddress(RFID_I2C_ADDR);
    mfrc522.PCD_Init();
    set_state(_initial_state);
    delay(500);
    return loop();
}

bool TagOMatic::setup_mfrc522() {
    Wire.beginTransmission(RFID_I2C_ADDR);
    int error = Wire.endTransmission();
    return (error == 0);
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
                load_file();
                break;
            case CLONE_MODE:
                clone_card();
                break;
            case WRITE_MODE:
                write_data();
                break;
            case WRITE_NDEF_MODE:
                write_ndef_data();
                break;
            case ERASE_MODE:
                erase_card();
                break;
            case SAVE_MODE:
                save_file();
                break;
        }

    }
}

void TagOMatic::select_state() {
    options = {};
    if (_read_uid) {
        options.push_back({"Clone UID",  [=]() { set_state(CLONE_MODE); }});
        options.push_back({"Write data", [=]() { set_state(WRITE_MODE); }});
        options.push_back({"Save file",  [=]() { set_state(SAVE_MODE); }});
    }
    options.push_back({"Read tag",   [=]() { set_state(READ_MODE); }});
    options.push_back({"Load file",  [=]() { set_state(LOAD_MODE); }});
    options.push_back({"Write NDEF", [=]() { set_state(WRITE_NDEF_MODE); }});
    options.push_back({"Erase tag",  [=]() { set_state(ERASE_MODE); }});
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
        case CLONE_MODE:
            padprintln("New UID: " + printableUID.uid);
            padprintln("SAK: " + printableUID.sak);
            padprintln("");
            break;
        case WRITE_MODE:
            if (!pageReadSuccess) padprintln("[!] Data blocks are incomplete");
            padprintln(String(dataPages) + " pages of data to write");
            padprintln("");
            break;
        case WRITE_NDEF_MODE:
            _ndef_created = false;
            break;
        case SAVE_MODE:
        case ERASE_MODE:
            break;
    }
    delay(300);
}

void TagOMatic::cls() {
    drawMainBorder();
    tft.setCursor(10, 28);
    tft.setTextColor(FGCOLOR, BGCOLOR);
}

void TagOMatic::display_banner() {
    cls();
    tft.setTextSize(FM);
    padprintln("TAG-O-MATIC");
    tft.setTextSize(FP);

    switch (current_state) {
        case READ_MODE:
            padprintln("             READ MODE");
            padprintln("             ---------");
            break;
        case LOAD_MODE:
            padprintln("             LOAD MODE");
            padprintln("             ---------");
            break;
        case CLONE_MODE:
            padprintln("            CLONE MODE");
            padprintln("            ----------");
            break;
        case ERASE_MODE:
            padprintln("            ERASE MODE");
            padprintln("            ----------");
            break;
        case WRITE_MODE:
            padprintln("       WRITE DATA MODE");
            padprintln("       ---------------");
            break;
        case WRITE_NDEF_MODE:
            padprintln("       WRITE NDEF MODE");
            padprintln("       ---------------");
            break;
        case SAVE_MODE:
            padprintln("             SAVE MODE");
            padprintln("             ---------");
            break;
    }

    tft.setTextSize(FP);
    padprintln("");
    padprintln("Press [OK] to change mode.");
    padprintln("");
}

void TagOMatic::dump_card_details() {
    padprintln("Device type: " + printableUID.picc_type);
	padprintln("UID: " + printableUID.uid);
	padprintln("ATQA: " + printableUID.atqa);
	padprintln("SAK: " + printableUID.sak);
    if (!pageReadSuccess) padprintln("[!] Failed to read data blocks");
}

void TagOMatic::dump_ndef_details() {
    if (!_ndef_created) return;

	String payload_type = "";
    switch (ndefMessage.payloadType) {
        case NDEF_URI:
            payload_type = "URI";
            break;
        case NDEF_TEXT:
            payload_type = "Text";
            break;
    }

    padprintln("Payload type: " + payload_type);
    padprintln("Payload size: " + String(ndefMessage.payloadSize) + " bytes");
}

bool TagOMatic::PICC_IsNewCardPresent() {
	byte bufferATQA[2];
	byte bufferSize = sizeof(bufferATQA);
	byte result = mfrc522.PICC_RequestA(bufferATQA, &bufferSize);
	bool bl_result = (result == mfrc522.STATUS_OK || result == mfrc522.STATUS_COLLISION);
    if (bl_result) {
        printableUID.atqa = "";
        for (byte i = 0; i < bufferSize; i++) {
            printableUID.atqa += bufferATQA[i] < 0x10 ? " 0" : " ";
            printableUID.atqa += String(bufferATQA[i], HEX);
        }
        printableUID.atqa.trim();
        printableUID.atqa.toUpperCase();
    }
    return bl_result;
}

void TagOMatic::read_card() {
    if (!PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }
    display_banner();
    pageReadSuccess = read_data_blocks();
    format_data();
    dump_card_details();
    uid = mfrc522.uid;
    _read_uid = true;
    delay(500);
}

void TagOMatic::clone_card() {
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
    delay(1000);
    set_state(READ_MODE);
}

void TagOMatic::erase_card() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    if (erase_data_blocks()) {
        displaySuccess("Tag erased successfully.");
    }
    else {
        displayError("Error erasing data from tag.");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
    set_state(READ_MODE);
}

bool TagOMatic::erase_data_blocks() {
	byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    bool blockWriteSuccess;

    switch (piccType) {
        case MFRC522::PICC_TYPE_MIFARE_MINI:
        case MFRC522::PICC_TYPE_MIFARE_1K:
        case MFRC522::PICC_TYPE_MIFARE_4K:
            for (byte i = 1; i < 64; i++) {
                if ((i + 1) % 4 == 0) continue;
                blockWriteSuccess = write_mifare_classic_data_block(i, "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
                if (!blockWriteSuccess) return false;
            }
            break;

        case MFRC522::PICC_TYPE_MIFARE_UL:
            // NDEF stardard
            blockWriteSuccess = write_mifare_ultralight_data_block(4, "03 00 FE 00");
            if (!blockWriteSuccess) return false;

            for (byte i = 5; i < 130; i++) {
                blockWriteSuccess = write_mifare_ultralight_data_block(i, "00 00 00 00");
                if (!blockWriteSuccess) return false;
            }
            break;

        default:
            break;
    }

    return true;
}

void TagOMatic::write_data() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    if (mfrc522.uid.sak != uid.sak) {
        displayError("Tag types do not match.");
        delay(1000);
        return set_state(READ_MODE);
    }

    if (write_data_blocks()) {
        displaySuccess("Tag written successfully.");
    }
    else {
        displayError("Error writing data to tag.");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
    set_state(READ_MODE);
}

void TagOMatic::write_ndef_data() {
    if (!_ndef_created) {
        create_ndef_message();
        display_banner();
        dump_ndef_details();
    }

    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    if (write_ndef_blocks()) {
        displaySuccess("Tag written successfully.");
    }
    else {
        displayError("Error writing data to tag.");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
    set_state(READ_MODE);
}

void TagOMatic::create_ndef_message() {
    options = {
        {"Text", [=]() { create_ndef_text(); }},
        {"URL",  [=]() { create_ndef_url(); }},
    };
    delay(200);
    loopOptions(options);
}

void TagOMatic::create_ndef_text() {
    ndefMessage.payloadType = NDEF_TEXT;
    byte uic = 0;
    byte i;

    ndefMessage.payload[0] = 0x02;  // language size
    ndefMessage.payload[1] = 0x65;  // "en" language
    ndefMessage.payload[2] = 0x6E;

    String ndef_data = keyboard("", NDEF_DATA_SIZE, "NDEF data:");

    for (i = 0; i < ndef_data.length(); i++) {
        ndefMessage.payload[i+3] = ndef_data.charAt(i);
    }
    ndefMessage.payloadSize = i+3;
    ndefMessage.messageSize = ndefMessage.payloadSize+4;

    _ndef_created = true;
}

void TagOMatic::create_ndef_url() {
    ndefMessage.payloadType = NDEF_URI;
    byte uic = 0;
    String prefix = "";
    byte i;

    options = {
        {"http://www.",  [&]() { uic=1; prefix="http://www."; }},
        {"https://www.", [&]() { uic=2; prefix="https://www."; }},
        {"http://",      [&]() { uic=3; prefix="http://"; }},
        {"https://",     [&]() { uic=4; prefix="https://"; }},
        {"tel:",         [&]() { uic=5; prefix="tel:"; }},
        {"mailto:",      [&]() { uic=6; prefix="mailto:"; }},
        {"None",         [&]() { uic=0; prefix="None"; }},
    };
    delay(200);
    loopOptions(options);

    ndefMessage.payload[0] = uic;

    String ndef_data = keyboard(prefix, NDEF_DATA_SIZE, "NDEF data:");
    ndef_data = ndef_data.substring(prefix.length());

    for (i = 0; i < ndef_data.length(); i++) {
        ndefMessage.payload[i+1] = ndef_data.charAt(i);
    }
    ndefMessage.payloadSize = i+1;
    ndefMessage.messageSize = ndefMessage.payloadSize+4;

    _ndef_created = true;
}

bool TagOMatic::write_ndef_blocks() {
	byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_UL) return false;

    byte ndef_size = ndefMessage.messageSize + 3;
    byte payload_size = ndef_size % 4 == 0 ? ndef_size : ndef_size + (4 - (ndef_size % 4));
    byte ndef_payload[payload_size];
    byte i;
    bool blockWriteSuccess;

    ndef_payload[0] = ndefMessage.begin;
    ndef_payload[1] = ndefMessage.messageSize;
    ndef_payload[2] = ndefMessage.header;
    ndef_payload[3] = ndefMessage.tnf;
    ndef_payload[4] = ndefMessage.payloadSize;
    ndef_payload[5] = ndefMessage.payloadType;

    for (i = 0; i < ndefMessage.payloadSize; i++) {
        ndef_payload[i+6] = ndefMessage.payload[i];
    }

    ndef_payload[ndef_size-1] = ndefMessage.end;

    if (payload_size > ndef_size) {
        for (i = ndef_size; i < payload_size; i++) {
            ndef_payload[i] = 0;
        }
    }

    for (int i=0; i<payload_size; i+=4) {
        int block = 4 + (i / 4);
        byte status = mfrc522.MIFARE_Ultralight_Write((byte)block, ndef_payload+i, 4);
        if (status != MFRC522::STATUS_OK) return false;
    }

    return true;
}

bool TagOMatic::write_data_blocks() {
	byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    String pageLine = "";
    String strBytes = "";
    int lineBreakIndex;
    int pageIndex;
    bool blockWriteSuccess;

    while (strAllPages.length() > 0) {
        lineBreakIndex = strAllPages.indexOf("\n");
        pageLine = strAllPages.substring(0, lineBreakIndex);
        strAllPages = strAllPages.substring(lineBreakIndex + 1);

        pageIndex = pageLine.substring(5, pageLine.indexOf(":")).toInt();
        strBytes = pageLine.substring(pageLine.indexOf(":") + 1);
        strBytes.trim();

        if (pageIndex == 0) continue;

        switch (piccType) {
            case MFRC522::PICC_TYPE_MIFARE_MINI:
            case MFRC522::PICC_TYPE_MIFARE_1K:
            case MFRC522::PICC_TYPE_MIFARE_4K:
                if (pageIndex == 0 || (pageIndex + 1) % 4 == 0) continue;  // Data blocks for MIFARE Classic
                blockWriteSuccess = write_mifare_classic_data_block(pageIndex, strBytes);
                break;

            case MFRC522::PICC_TYPE_MIFARE_UL:
                if (pageIndex < 4 || pageIndex >= dataPages-5) continue;  // Data blocks for NTAG21X
                blockWriteSuccess = write_mifare_ultralight_data_block(pageIndex, strBytes);
                break;

            default:
                blockWriteSuccess = false;
                break;
        }

        if (!blockWriteSuccess) return false;
    }

    return true;
}

bool TagOMatic::write_mifare_classic_data_block(int block, String data) {
    data.replace(" ", "");
    byte size = data.length() / 2;
    byte buffer[size];

    for (size_t i = 0; i < data.length(); i += 2) {
        buffer[i / 2] = strtoul(data.substring(i, i + 2).c_str(), NULL, 16);
    }

    MFRC522::MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) return false;

    status = mfrc522.MIFARE_Write((byte)block, buffer, size);
    if (status != MFRC522::STATUS_OK) return false;

    return true;
}

bool TagOMatic::write_mifare_ultralight_data_block(int block, String data) {
    data.replace(" ", "");
    byte size = data.length() / 2;
    byte buffer[size];

    for (size_t i = 0; i < data.length(); i += 2) {
        buffer[i / 2] = strtoul(data.substring(i, i + 2).c_str(), NULL, 16);
    }

    byte status = mfrc522.MIFARE_Ultralight_Write((byte)block, buffer, size);
    if (status != MFRC522::STATUS_OK) return false;

    return true;
}

void TagOMatic::save_file() {
    String uid_str = printableUID.uid;
    uid_str.replace(" ", "");
    String filename = keyboard(uid_str, 30, "File name:");

    display_banner();

    if (write_file(filename)) {
        displaySuccess("File saved.");
    }
    else {
        displayError("Error writing file.");
    }
    delay(1000);
    set_state(READ_MODE);
}

bool TagOMatic::write_file(String filename) {
    FS *fs;
    if(setupSdCard()) fs=&SD;
    else {
        if(!checkLittleFsSize()) fs=&LittleFS;
        else {
            return false;
        }
    }

    if (!(*fs).exists("/BruceRFID")) (*fs).mkdir("/BruceRFID");
    if ((*fs).exists("/BruceRFID/" + filename + ".rfid")) {
        int i = 1;
        filename += "_";
        while((*fs).exists("/BruceRFID/" + filename + String(i) + ".rfid")) i++;
        filename += String(i);
    }
    File file = (*fs).open("/BruceRFID/"+ filename + ".rfid", FILE_WRITE);

    if(!file) {
        return false;
    }

    file.println("Filetype: Bruce RFID File");
    file.println("Version 1");
    file.println("Device type: " + printableUID.picc_type);
    file.println("# UID, ATQA and SAK are common for all formats");
	file.println("UID: " + printableUID.uid);
	file.println("SAK: " + printableUID.sak);
	file.println("ATQA: " + printableUID.atqa);
    file.println("# Memory dump");
	file.println("Pages total: " + String(dataPages));
    if (!pageReadSuccess) file.println("Pages read: " + String(dataPages));
    file.print(strAllPages);

    file.close();
    delay(100);
    return true;
}

void TagOMatic::load_file() {
    display_banner();

    if (load_from_file()) {
        parse_data();
        displaySuccess("File loaded.");
        delay(1000);
        _read_uid = true;

        options = {
            {"Clone UID",  [=]() { set_state(CLONE_MODE); }},
            {"Write data", [=]() { set_state(WRITE_MODE); }},
        };
        delay(200);
        loopOptions(options);
    }
    else {
        displayError("Error loading file.");
        delay(1000);
        set_state(READ_MODE);
    }
}

bool TagOMatic::load_from_file() {
    cls();

    String filepath;
    File file;
    FS *fs;

    if(setupSdCard()) fs=&SD;
    else fs=&LittleFS;
    filepath = loopSD(*fs, true, "RFID|NFC");
    file = fs->open(filepath, FILE_READ);

    if (!file) {
        return false;
    }

    String line;
    String strData;
    strAllPages = "";
    pageReadSuccess = true;

    while (file.available()) {
        line = file.readStringUntil('\n');
        strData = line.substring(line.indexOf(":") + 1);
        strData.trim();
        if(line.startsWith("Device type:"))  printableUID.picc_type = strData;
        if(line.startsWith("UID:"))          printableUID.uid = strData;
        if(line.startsWith("SAK:"))          printableUID.sak = strData;
        if(line.startsWith("ATQA:"))         printableUID.atqa = strData;
        if(line.startsWith("Pages total:"))  dataPages = strData.toInt();
        if(line.startsWith("Pages read:"))   pageReadSuccess = false;
        if(line.startsWith("Page "))         strAllPages += line + "\n";
    }

    file.close();
    delay(100);
    return true;
}

String TagOMatic::get_tag_type() {
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    String tag_type = mfrc522.PICC_GetTypeName(piccType);

    if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
        switch (totalPages) {
            case 45:
                tag_type = "NTAG213";
                break;
            case 135:
                tag_type = "NTAG215";
                break;
            case 231:
                tag_type = "NTAG216";
                break;
            default:
                break;
        }
    }

    return tag_type;
}

void TagOMatic::format_data() {
	byte bcc = 0;

    printableUID.picc_type = get_tag_type();

	printableUID.sak = mfrc522.uid.sak < 0x10 ? "0" : "";
    printableUID.sak += String(mfrc522.uid.sak, HEX);
    printableUID.sak.toUpperCase();

	// UID
	printableUID.uid = "";
	for (byte i = 0; i < mfrc522.uid.size; i++) {
        printableUID.uid += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
        printableUID.uid += String(mfrc522.uid.uidByte[i], HEX);
		bcc = bcc ^ mfrc522.uid.uidByte[i];
	}
    printableUID.uid.trim();
    printableUID.uid.toUpperCase();

	// BCC
	printableUID.bcc = bcc < 0x10 ? "0" : "";
    printableUID.bcc += String(bcc, HEX);
    printableUID.bcc.toUpperCase();

    // ATQA
    String atqaPart1 = printableUID.atqa.substring(0, 2);
    String atqaPart2 = printableUID.atqa.substring(3, 5);
    printableUID.atqa = atqaPart2 + " " + atqaPart1;
}

void TagOMatic::parse_data() {
    String strUID = printableUID.uid;
    strUID.trim();
    strUID.replace(" ", "");
    uid.size = strUID.length() / 2;
    for (size_t i = 0; i < strUID.length(); i += 2) {
        uid.uidByte[i / 2] = strtoul(strUID.substring(i, i + 2).c_str(), NULL, 16);
    }

    printableUID.sak.trim();
    uid.sak = strtoul(printableUID.sak.c_str(), NULL, 16);
}

bool TagOMatic::read_data_blocks() {
    dataPages = 0;
    totalPages = 0;
    bool readSuccess = false;
	MFRC522::MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    strAllPages = "";

	switch (piccType) {
		case MFRC522::PICC_TYPE_MIFARE_MINI:
		case MFRC522::PICC_TYPE_MIFARE_1K:
		case MFRC522::PICC_TYPE_MIFARE_4K:
			readSuccess = read_mifare_classic_data_blocks(piccType, &key);
			break;

		case MFRC522::PICC_TYPE_MIFARE_UL:
			readSuccess = read_mifare_ultralight_data_blocks();
            dataPages = (readSuccess && dataPages > 0) ? dataPages - 1 : dataPages;
            if (totalPages == 0) totalPages = dataPages;
			break;

		default:
			break;
	}

	mfrc522.PICC_HaltA();
    return readSuccess;
}

bool TagOMatic::read_mifare_classic_data_blocks(byte piccType, MFRC522::MIFARE_Key *key) {
	byte no_of_sectors = 0;
    bool sectorReadSuccess;

	switch (piccType) {
		case MFRC522::PICC_TYPE_MIFARE_MINI:
			no_of_sectors = 5;
            totalPages = 20;  // 320 bytes / 16 bytes per page
			break;

		case MFRC522::PICC_TYPE_MIFARE_1K:
			no_of_sectors = 16;
            totalPages = 64;  // 1024 bytes / 16 bytes per page
			break;

		case MFRC522::PICC_TYPE_MIFARE_4K:
			no_of_sectors = 40;
            totalPages = 256;  // 4096 bytes / 16 bytes per page
			break;

		default: // Should not happen. Ignore.
			break;
	}

	if (no_of_sectors) {
		for (int8_t i = 0; i < no_of_sectors; i++) {
			sectorReadSuccess = read_mifare_classic_data_sector(key, i);
            if (!sectorReadSuccess) break;
		}
	}
	mfrc522.PICC_HaltA();
	mfrc522.PCD_StopCrypto1();
    return sectorReadSuccess;
}

bool TagOMatic::read_mifare_classic_data_sector(MFRC522::MIFARE_Key *key, byte sector) {
	byte status;
	byte firstBlock;
	byte no_of_blocks;
	bool isSectorTrailer;
	byte c1, c2, c3;
	byte c1_, c2_, c3_;
	bool invertedError;
	byte g[4];
	byte group;
	bool firstInGroup;

	if (sector < 32) {
		no_of_blocks = 4;
		firstBlock = sector * no_of_blocks;
	}
	else if (sector < 40) {
		no_of_blocks = 16;
		firstBlock = 128 + (sector - 32) * no_of_blocks;
	}
	else {
		return false;
	}

	byte byteCount;
	byte buffer[18];
	byte blockAddr;
	isSectorTrailer = true;
	String strPage;

	for (int8_t blockOffset = 0; blockOffset < no_of_blocks; blockOffset++) {
        strPage = "";
		blockAddr = firstBlock + blockOffset;
		if (isSectorTrailer) {
			status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, &mfrc522.uid);
			if (status != MFRC522::STATUS_OK) {
				return false;
			}
		}
		byteCount = sizeof(buffer);
		status = mfrc522.MIFARE_Read(blockAddr, buffer, &byteCount);
		if (status != MFRC522::STATUS_OK) {
			return false;
		}
		for (byte index = 0; index < 16; index++) {
            strPage += buffer[index] < 0x10 ? F(" 0") : F(" ");
			strPage += String(buffer[index], HEX);
		}
		if (isSectorTrailer) {
			c1  = buffer[7] >> 4;
			c2  = buffer[8] & 0xF;
			c3  = buffer[8] >> 4;
			c1_ = buffer[6] & 0xF;
			c2_ = buffer[6] >> 4;
			c3_ = buffer[7] & 0xF;
			invertedError = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
			g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
			g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
			g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
			g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
			isSectorTrailer = false;
		}

		if (no_of_blocks == 4) {
			group = blockOffset;
			firstInGroup = true;
		}
		else {
			group = blockOffset / 5;
			firstInGroup = (group == 3) || (group != (blockOffset + 1) / 5);
		}

        strPage.trim();
        strPage.toUpperCase();

		strAllPages += "Page " + String(dataPages) + ": " + strPage + "\n";
        dataPages++;
	}

	return true;
}

bool TagOMatic::read_mifare_ultralight_data_blocks() {
	byte status;
	byte byteCount;
	byte buffer[18];
	byte i;
	byte cc;
	String strPage = "";

	for (byte page = 0; page < 256; page +=4) {
		byteCount = sizeof(buffer);
		status = mfrc522.MIFARE_Read(page, buffer, &byteCount);
		if (status != MFRC522::STATUS_OK) {
            return status == MFRC522::STATUS_MIFARE_NACK;
		}
		for (byte offset = 0; offset < 4; offset++) {
            strPage = "";
            if (page + offset == 3) {
                cc = buffer[4 * offset + 2];
                switch (cc) {
                    // NTAG213
                    case 0x12:
                        totalPages = 45;
                        break;
                    // NTAG215
                    case 0x3E:
                        totalPages = 135;
                        break;
                    // NTAG216
                    case 0x6D:
                        totalPages = 231;
                        break;
                    default:
                        break;
                }
            }
			for (byte index = 0; index < 4; index++) {
				i = 4 * offset + index;
	            strPage += buffer[i] < 0x10 ? F(" 0") : F(" ");
				strPage += String(buffer[i], HEX);
			}
            strPage.trim();
            strPage.toUpperCase();

		    strAllPages += "Page " + String(dataPages) + ": " + strPage + "\n";
            dataPages++;
		}
	}

    return true;
}
