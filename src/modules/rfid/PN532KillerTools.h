#ifndef __PN532KILLERTOOLS_H__
#define __PN532KILLERTOOLS_H__

#include "PN532Killer.h"
#include <cstdint>
#include <set>
#include <vector>

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
    PN532KillerCmd::SnifferType _snifferType = PN532KillerCmd::SnifferType::MFKey32v2;

private:
    PN532Killer _pn532Killer = PN532Killer(Serial1);

    void sendCommand(const std::vector<uint8_t> &data);
    void displayBanner();

    void setEmulatorNextSlot(bool reverse = false, bool redrawTypeName = true);
    void setSnifferMode();
    void setSnifferUid();
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
