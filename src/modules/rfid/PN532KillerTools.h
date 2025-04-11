#ifndef __PN532KILLERTOOLS_H__
#define __PN532KILLERTOOLS_H__

#include <cstdint>
#include <set>
#include <vector>
#include "PN532Killer.h"

class PN532KillerTools {
public:
    PN532KillerTools();
    ~PN532KillerTools();

    void setup();
    void loop();
    bool connect();

    PN532KillerCmd::WorkMode _workMode = PN532KillerCmd::WorkMode::Reader;
    PN532KillerCmd::TagType _tagType = PN532KillerCmd::TagType::MFC1K;
    PN532KillerCmd::ReaderProtocol _readerProtocol = PN532KillerCmd::ReaderProtocol::HF_ISO14443A;
private:
    PN532Killer _pn532Killer = PN532Killer(Serial1);

    void sendCommand(const std::vector<uint8_t> &data);
    void displayBanner();

    void setEmulatorNextSlot(bool reverse = false);
    void setSnifferMode(uint8_t sniffer_type);
    void mainMenu();
    void emulatorMenu();
    void snifferMenu();
    void readerMenu();
    void readTagUid();
    void printUid(const char *protocol, const char *uid);
    void setReaderMode();
    bool enableBleDataTransfer();
    bool disableBleDataTransfer();
};

#endif
