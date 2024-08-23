/**
 * @file RFID2.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags using RFID2 module from M5Stack
 * @version 0.1
 * @date 2024-08-19
 */

#include "RFID2.h"
#include <Wire.h>
#include "core/sd_functions.h"
#include "core/i2c_finder.h"

#define RFID2_I2C_ADDRESS 0x28


RFID2::RFID2() {}

bool RFID2::begin() {
    if (!check_i2c_address(RFID2_I2C_ADDRESS)) return false;

    mfrc522.SetChipAddress(RFID2_I2C_ADDRESS);
    mfrc522.PCD_Init();
    return true;
}

bool RFID2::PICC_IsNewCardPresent() {
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

int RFID2::read() {
    if (!PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return TAG_NOT_PRESENT;
    }
    pageReadSuccess = read_data_blocks();
    format_data();
    set_uid();
    return SUCCESS;
}

int RFID2::clone() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return TAG_NOT_PRESENT;
    }

    if (mfrc522.uid.sak != uid.sak) return TAG_NOT_MATCH;

    bool success = mfrc522.MIFARE_SetUid(uid.uidByte, uid.size, true);
    mfrc522.PICC_HaltA();
    return success ? SUCCESS : FAILURE;
}

int RFID2::erase() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return TAG_NOT_PRESENT;
    }

    int result = erase_data_blocks();
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return result;
}

int RFID2::write() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return TAG_NOT_PRESENT;
    }

    if (mfrc522.uid.sak != uid.sak) return TAG_NOT_MATCH;

    int result = write_data_blocks();

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return result;
}

int RFID2::write_ndef() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return TAG_NOT_PRESENT;
    }

    int result = write_ndef_blocks();

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return result;
}

int RFID2::load() {
    String filepath;
    File file;
    FS *fs;

    if(setupSdCard()) fs=&SD;
    else fs=&LittleFS;
    filepath = loopSD(*fs, true, "RFID|NFC");
    file = fs->open(filepath, FILE_READ);

    if (!file) {
        return FAILURE;
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
    parse_data();

    return SUCCESS;
}

int RFID2::save(String filename) {
    FS *fs;
    if(setupSdCard()) fs=&SD;
    else {
        if(!checkLittleFsSize()) fs=&LittleFS;
        else {
            return FAILURE;
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
        return FAILURE;
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
    return SUCCESS;
}

String RFID2::get_tag_type() {
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

void RFID2::set_uid() {
    uid.sak = mfrc522.uid.sak;
    uid.size = mfrc522.uid.size;

    for (byte i = 0; i<mfrc522.uid.size; i++) {
        uid.uidByte[i] = mfrc522.uid.uidByte[i];
    }
}

void RFID2::format_data() {
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

void RFID2::parse_data() {
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

bool RFID2::read_data_blocks() {
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

bool RFID2::read_mifare_classic_data_blocks(byte piccType, MFRC522::MIFARE_Key *key) {
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

bool RFID2::read_mifare_classic_data_sector(MFRC522::MIFARE_Key *key, byte sector) {
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

bool RFID2::read_mifare_ultralight_data_blocks() {
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

int RFID2::write_data_blocks() {
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

        if (!blockWriteSuccess) return FAILURE;
    }

    return SUCCESS;
}

bool RFID2::write_mifare_classic_data_block(int block, String data) {
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

bool RFID2::write_mifare_ultralight_data_block(int block, String data) {
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

int RFID2::erase_data_blocks() {
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    bool blockWriteSuccess;

    switch (piccType) {
        case MFRC522::PICC_TYPE_MIFARE_MINI:
        case MFRC522::PICC_TYPE_MIFARE_1K:
        case MFRC522::PICC_TYPE_MIFARE_4K:
            for (byte i = 1; i < 64; i++) {
                if ((i + 1) % 4 == 0) continue;
                blockWriteSuccess = write_mifare_classic_data_block(i, "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
                if (!blockWriteSuccess) return FAILURE;
            }
            break;

        case MFRC522::PICC_TYPE_MIFARE_UL:
            // NDEF stardard
            blockWriteSuccess = write_mifare_ultralight_data_block(4, "03 00 FE 00");
            if (!blockWriteSuccess) return FAILURE;

            for (byte i = 5; i < 130; i++) {
                blockWriteSuccess = write_mifare_ultralight_data_block(i, "00 00 00 00");
                if (!blockWriteSuccess) return FAILURE;
            }
            break;

        default:
            break;
    }

    return SUCCESS;
}

int RFID2::write_ndef_blocks() {
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_UL) return TAG_NOT_MATCH;

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
        if (status != MFRC522::STATUS_OK) return FAILURE;
    }

    return SUCCESS;
}
