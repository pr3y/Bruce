#ifndef __PN532KILLERTOOLS_H__
#define __PN532KILLERTOOLS_H__
#ifndef LITE_VERSION
#include "PN532Killer.h"
#include <WiFi.h>
#include <WiFiUdp.h>
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
    String _titleName = "PN532Killer";
    bool _isPn532killer = false;
    bool _deviceInitialized = false;
    bool _initializationFailed = false;
    void hardwareProbe();

    void sendCommand(const std::vector<uint8_t> &data);
    void displayBanner();
    void displayInitialScreen();
    void playDeviceDetectedSound();
    void playUidFoundSound();
    void resetDevice(bool showInitialScreen = true);

    void setEmulatorNextSlot(bool reverse = false, bool redrawTypeName = true);
    void setSnifferMode();
    void setSnifferUid();
    void mainMenu();
    void failedInitMenu();
    void netMenu();
    void emulatorMenu();
    void snifferMenu();
    void readerMenu();
    void readTagUid();
    void printUid(const char *protocol, const char *uid);
    void setReaderMode();
    bool enableBleDataTransfer();
    bool disableBleDataTransfer();
    bool enableUdpDataTransfer();
    bool disableUdpDataTransfer();
    bool enableTcpDataTransfer();
    bool disableTcpDataTransfer();

    void drainUartToUdp(bool log = true);
    void udpWifiSelectMenu();
    void sendUdpToPn532(const uint8_t *data, int len);
    void sendTcpToPn532(const uint8_t *data, int len);

    bool _udpEnabled = false;
    WiFiUDP _udp;
    IPAddress _udpRemoteIP;
    uint16_t _udpRemotePort = 0;
    bool _udpHasRemote = false;
    uint32_t _udpLastPacketMs = 0;

    bool _tcpEnabled = false;
    WiFiServer _tcpServer = WiFiServer(18889);
    WiFiClient _tcpClient;
    bool _tcpHasClient = false;
    uint32_t _tcpLastPacketMs = 0;
};

#endif
#endif
