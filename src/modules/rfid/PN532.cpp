/**
 * @file PN532.cpp
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Read and Write RFID tags using PN532 module
 * @version 0.1
 * @date 2024-08-19
 */

#include "PN532.h"
#include "core/sd_functions.h"
#include "core/i2c_finder.h"
#include "core/display.h"


PN532::PN532(bool use_i2c) {
    _use_i2c = use_i2c;
    if (use_i2c) nfc.setInterface(GROVE_SDA, GROVE_SCL);
    else nfc.setInterface(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_SS_PIN);
}

bool PN532::begin() {
    bool i2c_check = check_i2c_address(PN532_I2C_ADDRESS);

    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();

    return i2c_check || versiondata;
}

int PN532::read() {
    if (!nfc.startPassiveTargetIDDetection()) return TAG_NOT_PRESENT;
    if (!nfc.readDetectedPassiveTargetID()) return FAILURE;

    displayInfo("Reading data blocks...");
    pageReadSuccess = read_data_blocks();
    format_data();
    set_uid();
    return SUCCESS;
}

int PN532::clone() {
    if (!nfc.startPassiveTargetIDDetection()) return TAG_NOT_PRESENT;
    if (!nfc.readDetectedPassiveTargetID()) return FAILURE;

    if (nfc.targetUid.sak != uid.sak) return TAG_NOT_MATCH;

    uint8_t data[16];
    byte bcc = 0;
    int i;
    for (i = 0; i < uid.size; i++) {
        data[i] = uid.uidByte[i];
        bcc = bcc ^ uid.uidByte[i];
    }
    data[i++] = bcc;
    data[i++] = uid.sak;
    data[i++] = uid.atqaByte[1];
    data[i++] = uid.atqaByte[0];
    byte tmp = 0;
    while (i<16) data[i++] = 0x62+tmp++;

    bool success = nfc.mifareclassic_WriteBlock0(data);
    return success ? SUCCESS : FAILURE;
}

int PN532::erase() {
    if (!nfc.startPassiveTargetIDDetection()) return TAG_NOT_PRESENT;
    if (!nfc.readDetectedPassiveTargetID()) return FAILURE;

    return erase_data_blocks();
}

int PN532::write() {
    if (!nfc.startPassiveTargetIDDetection()) return TAG_NOT_PRESENT;
    if (!nfc.readDetectedPassiveTargetID()) return FAILURE;

    if (nfc.targetUid.sak != uid.sak) return TAG_NOT_MATCH;

    return write_data_blocks();
}

int PN532::write_ndef() {
    if (!nfc.startPassiveTargetIDDetection()) return TAG_NOT_PRESENT;
    if (!nfc.readDetectedPassiveTargetID()) return FAILURE;

    return write_ndef_blocks();
}

int PN532::load() {
    String filepath;
    File file;
    FS *fs;

    if(!getFsStorage(fs)) return FAILURE;
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

int PN532::save(String filename) {
    FS *fs;
    if(!getFsStorage(fs)) return FAILURE;

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

String PN532::get_tag_type() {
    String tag_type = nfc.PICC_GetTypeName(nfc.targetUid.sak);

    if (nfc.targetUid.sak == PICC_TYPE_MIFARE_UL) {
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

void PN532::set_uid() {
    uid.sak = nfc.targetUid.sak;
    uid.size = nfc.targetUid.size;

    for (byte i = 0; i<2; i++) uid.atqaByte[i] = nfc.targetUid.atqaByte[i];

    for (byte i = 0; i<nfc.targetUid.size; i++) {
        uid.uidByte[i] = nfc.targetUid.uidByte[i];
    }
}

void PN532::format_data() {
    byte bcc = 0;

    printableUID.picc_type = get_tag_type();

    printableUID.sak = nfc.targetUid.sak < 0x10 ? "0" : "";
    printableUID.sak += String(nfc.targetUid.sak, HEX);
    printableUID.sak.toUpperCase();

    // UID
    printableUID.uid = "";
    for (byte i = 0; i < nfc.targetUid.size; i++) {
        printableUID.uid += nfc.targetUid.uidByte[i] < 0x10 ? " 0" : " ";
        printableUID.uid += String(nfc.targetUid.uidByte[i], HEX);
        bcc = bcc ^ nfc.targetUid.uidByte[i];
    }
    printableUID.uid.trim();
    printableUID.uid.toUpperCase();

    // BCC
    printableUID.bcc = bcc < 0x10 ? "0" : "";
    printableUID.bcc += String(bcc, HEX);
    printableUID.bcc.toUpperCase();

    // ATQA
    printableUID.atqa = "";
    for (byte i = 0; i < 2; i++) {
        printableUID.atqa += nfc.targetUid.atqaByte[i] < 0x10 ? " 0" : " ";
        printableUID.atqa += String(nfc.targetUid.atqaByte[i], HEX);
    }
    printableUID.atqa.trim();
    printableUID.atqa.toUpperCase();
}

void PN532::parse_data() {
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

bool PN532::read_data_blocks() {
    dataPages = 0;
    totalPages = 0;
    bool readSuccess = false;
    uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    strAllPages = "";

    switch (uid.sak) {
        case PICC_TYPE_MIFARE_MINI:
        case PICC_TYPE_MIFARE_1K:
        case PICC_TYPE_MIFARE_4K:
            readSuccess = read_mifare_classic_data_blocks(keya);
            break;

        case PICC_TYPE_MIFARE_UL:
            readSuccess = read_mifare_ultralight_data_blocks();
            if (totalPages == 0) totalPages = dataPages;
            break;

        default:
            break;
    }

    return readSuccess;
}

bool PN532::read_mifare_classic_data_blocks(uint8_t *key) {
    byte no_of_sectors = 0;
    bool sectorReadSuccess;

    switch (uid.sak) {
        case PICC_TYPE_MIFARE_MINI:
            no_of_sectors = 5;
            totalPages = 20;  // 320 bytes / 16 bytes per page
            break;

        case PICC_TYPE_MIFARE_1K:
            no_of_sectors = 16;
            totalPages = 64;  // 1024 bytes / 16 bytes per page
            break;

        case PICC_TYPE_MIFARE_4K:
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
    return sectorReadSuccess;
}

bool PN532::read_mifare_classic_data_sector(uint8_t *key, byte sector) {
    uint8_t success;
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

    byte buffer[18];
    byte blockAddr;
    isSectorTrailer = true;
    String strPage;

    for (int8_t blockOffset = 0; blockOffset < no_of_blocks; blockOffset++) {
        strPage = "";
        blockAddr = firstBlock + blockOffset;
        if (isSectorTrailer) {
            success = nfc.mifareclassic_AuthenticateBlock(uid.uidByte, uid.size, firstBlock, 0, key);
            if (!success) {
                return false;
            }
        }
        success = nfc.mifareclassic_ReadDataBlock(blockAddr, buffer);
        if (!success) {
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

bool PN532::read_mifare_ultralight_data_blocks() {
    uint8_t success;
    byte buffer[18];
    byte i;
    String strPage = "";

    uint8_t buf[4];
    nfc.mifareultralight_ReadPage(3, buf);
    switch (buf[2]) {
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
        // MIFARE UL
        default:
            totalPages = 64;
            break;
    }

    for (byte page = 0; page < totalPages; page+=4) {
        success = nfc.ntag2xx_ReadPage(page, buffer);
        if (!success) return false;

        for (byte offset = 0; offset < 4; offset++) {
            strPage = "";
            for (byte index = 0; index < 4; index++) {
                i = 4 * offset + index;
                strPage += buffer[i] < 0x10 ? F(" 0") : F(" ");
                strPage += String(buffer[i], HEX);
            }
            strPage.trim();
            strPage.toUpperCase();

            strAllPages += "Page " + String(dataPages) + ": " + strPage + "\n";
            dataPages++;
            if (dataPages >= totalPages) break;
        }
    }

    return true;
}

int PN532::write_data_blocks() {
    String pageLine = "";
    String strBytes = "";
    int lineBreakIndex;
    int pageIndex;
    bool blockWriteSuccess;
    int totalSize = strAllPages.length();

    while (strAllPages.length() > 0) {
        lineBreakIndex = strAllPages.indexOf("\n");
        pageLine = strAllPages.substring(0, lineBreakIndex);
        strAllPages = strAllPages.substring(lineBreakIndex + 1);

        pageIndex = pageLine.substring(5, pageLine.indexOf(":")).toInt();
        strBytes = pageLine.substring(pageLine.indexOf(":") + 1);
        strBytes.trim();

        if (pageIndex == 0) continue;

        switch (uid.sak) {
            case PICC_TYPE_MIFARE_MINI:
            case PICC_TYPE_MIFARE_1K:
            case PICC_TYPE_MIFARE_4K:
                if (pageIndex == 0 || (pageIndex + 1) % 4 == 0) continue;  // Data blocks for MIFARE Classic
                blockWriteSuccess = write_mifare_classic_data_block(pageIndex, strBytes);
                break;

            case PICC_TYPE_MIFARE_UL:
                if (pageIndex < 4 || pageIndex >= dataPages-5) continue;  // Data blocks for NTAG21X
                blockWriteSuccess = write_mifare_ultralight_data_block(pageIndex, strBytes);
                break;

            default:
                blockWriteSuccess = false;
                break;
        }

        if (!blockWriteSuccess) return FAILURE;

        progressHandler(totalSize-strAllPages.length(), totalSize, "Writing data blocks...");
    }

    return SUCCESS;
}

bool PN532::write_mifare_classic_data_block(int block, String data) {
    data.replace(" ", "");
    byte size = data.length() / 2;
    byte buffer[size];

    if (size != 16) return false;

    for (size_t i = 0; i < data.length(); i += 2) {
        buffer[i / 2] = strtoul(data.substring(i, i + 2).c_str(), NULL, 16);
    }

    uint8_t key[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


    uint8_t success = nfc.mifareclassic_AuthenticateBlock(uid.uidByte, uid.size, block, 0, key);
    if (!success) return false;

    return nfc.mifareclassic_WriteDataBlock(block, buffer);
}

bool PN532::write_mifare_ultralight_data_block(int block, String data) {
    data.replace(" ", "");
    byte size = data.length() / 2;
    byte buffer[size];

    if (size != 4) return false;

    for (size_t i = 0; i < data.length(); i += 2) {
        buffer[i / 2] = strtoul(data.substring(i, i + 2).c_str(), NULL, 16);
    }

    return nfc.ntag2xx_WritePage(block, buffer);
}

int PN532::erase_data_blocks() {
    bool blockWriteSuccess;

    switch (uid.sak) {
        case PICC_TYPE_MIFARE_MINI:
        case PICC_TYPE_MIFARE_1K:
        case PICC_TYPE_MIFARE_4K:
            for (byte i = 1; i < 64; i++) {
                if ((i + 1) % 4 == 0) continue;
                blockWriteSuccess = write_mifare_classic_data_block(i, "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00");
                if (!blockWriteSuccess) return FAILURE;
            }
            break;

        case PICC_TYPE_MIFARE_UL:
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

int PN532::write_ndef_blocks() {
    if (uid.sak != PICC_TYPE_MIFARE_UL) return TAG_NOT_MATCH;

    byte ndef_size = ndefMessage.messageSize + 3;
    byte payload_size = ndef_size % 4 == 0 ? ndef_size : ndef_size + (4 - (ndef_size % 4));
    byte ndef_payload[payload_size];
    byte i;
    bool blockWriteSuccess;
    uint8_t success;

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
        success = nfc.ntag2xx_WritePage(block, ndef_payload+i);
        if (!success) return FAILURE;
    }

    return SUCCESS;
}