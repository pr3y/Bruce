/**
 * @file connect.h
 * @author Rennan Cockles (https://github.com/rennancockles)
 * @brief Connection between devices
 * @version 0.1
 * @date 2024-10-02
 */


#ifndef __DEVICE_CONNECTION_H__
#define __DEVICE_CONNECTION_H__

#include <esp_now.h>
#include "core/globals.h"

#define PAGE_BYTES 150


class DeviceConnection {
public:
    enum Status {
        CONNECTING,
        STARTED,
        FAILED,
        SUCCESS,
        ABORTED,
    };

    typedef struct {
        String filename;
        String filepath;
        size_t totalBytes;
        size_t bytesSent;
        char data[PAGE_BYTES];
        size_t dataSize;
        bool done;
    } FileMessage;

    /////////////////////////////////////////////////////////////////////////////////////
    // Constructor
    /////////////////////////////////////////////////////////////////////////////////////
    DeviceConnection();
    ~DeviceConnection();

    /////////////////////////////////////////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////////////////////////////////////////
    void sendFile();
    void receiveFile();

private:
    Status recvStatus;
    Status sendStatus;
    String recvFileName;
    uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    esp_now_peer_info_t peerInfo;

    /////////////////////////////////////////////////////////////////////////////////////
    // Helpers
    /////////////////////////////////////////////////////////////////////////////////////
    bool setupPeer();
    File selectFile();
    FileMessage createFileMessage(File file);
    bool appendToFile(FileMessage fileMessage);
    void createFilename(FS *fs, FileMessage fileMessage);
};

void onDataSent(const uint8_t* mac_addr, esp_now_send_status_t status);
void onDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);

#endif
