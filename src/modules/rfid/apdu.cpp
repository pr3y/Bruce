#include "apdu.h"
#include <vector>
#include <string>

const uint8_t ApduCommand::C_APDU_CLA = 0;
const uint8_t ApduCommand::C_APDU_INS = 1;
const uint8_t ApduCommand::C_APDU_P1 = 2;
const uint8_t ApduCommand::C_APDU_P2 = 3;
const uint8_t ApduCommand::C_APDU_LC = 4;
const uint8_t ApduCommand::C_APDU_DATA = 5;
const uint8_t ApduCommand::C_APDU_P1_SELECT_BY_ID = 0x00;
const uint8_t ApduCommand::C_APDU_P1_SELECT_BY_NAME = 0x04;

const uint8_t ApduCommand::R_APDU_SW1_COMMAND_COMPLETE = 0x90;
const uint8_t ApduCommand::R_APDU_SW2_COMMAND_COMPLETE = 0x00;
const uint8_t ApduCommand::R_APDU_SW1_NDEF_TAG_NOT_FOUND = 0x6A;
const uint8_t ApduCommand::R_APDU_SW2_NDEF_TAG_NOT_FOUND = 0x82;
const uint8_t ApduCommand::R_APDU_SW1_FUNCTION_NOT_SUPPORTED = 0x6A;
const uint8_t ApduCommand::R_APDU_SW2_FUNCTION_NOT_SUPPORTED = 0x81;
const uint8_t ApduCommand::R_APDU_SW1_MEMORY_FAILURE = 0x65;
const uint8_t ApduCommand::R_APDU_SW2_MEMORY_FAILURE = 0x81;
const uint8_t ApduCommand::R_APDU_SW1_END_OF_FILE_BEFORE_REACHED_LE_BYTES = 0x62;
const uint8_t ApduCommand::R_APDU_SW2_END_OF_FILE_BEFORE_REACHED_LE_BYTES = 0x82;

const uint8_t ApduCommand::ISO7816_SELECT_FILE = 0xA4;
const uint8_t ApduCommand::ISO7816_READ_BINARY = 0xB0;
const uint8_t ApduCommand::ISO7816_UPDATE_BINARY = 0xD6;

const std::vector<uint8_t> NdefCommand::APPLICATION_NAME_V2 = {0, 0x07, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
const uint8_t NdefCommand::NDEF_MAX_LENGTH = 0x64;

const uint8_t Ndef::TNF_WELL_KNOWN = 0x01;
const uint8_t Ndef::RTD_URI = 0x55;

std::vector<uint8_t> Ndef::urlNdefAbbrv(std::string url) {
    std::vector<std::string> abbrv_table = {
        "http://www.",
        "https://www.",
        "http://",
        "https://",
        "tel:",
        "mailto:",
        "ftp://anonymous:anonymous@",
        "ftp://ftp.",
        "ftps://",
        "sftp://",
        "smb://",
        "nfs://",
        "ftp://",
        "dav://",
        "news:",
        "telnet://",
        "imap:",
        "rtsp://",
        "urn:",
        "pop:",
        "sip:",
        "sips:",
        "tftp:",
        "btspp://",
        "btl2cap://",
        "btgoep://",
        "tcpobex://",
        "irdaobex://",
        "file://",
        "urn:epc:id:",
        "urn:epc:tag:",
        "urn:epc:pat:",
        "urn:epc:raw:",
        "urn:epc:",
        "urn:nfc:"
    };

    std::vector<uint8_t> ndefMessage;

    for (size_t i = 0; i < abbrv_table.size(); ++i) {
        if (url.find(abbrv_table[i]) == 0) {
            ndefMessage.push_back(static_cast<uint8_t>(i + 1));
            url = url.substr(abbrv_table[i].length());
            break;
        }
    }

    ndefMessage.insert(ndefMessage.end(), url.begin(), url.end());
    return ndefMessage;
}

std::vector<uint8_t> Ndef::newMessage(std::vector<uint8_t> ndef) {
    std::vector<uint8_t> message;
    message.push_back(0xD1); // NDEF record header
    message.push_back(TNF_WELL_KNOWN);
    message.push_back(ndef.size());
    message.push_back(RTD_URI);
    message.insert(message.end(), ndef.begin(), ndef.end());
    return message;
}
