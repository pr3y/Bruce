#ifndef BRUCE_FILEMANAGERSERVICE_H
#define BRUCE_FILEMANAGERSERVICE_H

#include <NimBLEServer.h>

class FileManagerService {
    NimBLECharacteristic *fs_char = nullptr;
    NimBLEService *fs_service = nullptr;
public:
    FileManagerService(/* args */);
    ~FileManagerService();
    void setup(NimBLEServer *pServer);
    void end();
};

#endif // BRUCE_FILEMANAGERSERVICE_H
