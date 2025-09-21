#ifndef BRUCE_FILEMANAGERSERVICE_H
#define BRUCE_FILEMANAGERSERVICE_H

#include <NimBLEServer.h>
#include "BruceBLEService.hpp"

class FileManagerService : public BruceBLEService {
    NimBLECharacteristic *fs_char = nullptr;
    NimBLECharacteristic *fs_list_char = nullptr;
public:
    FileManagerService(/* args */);
    ~FileManagerService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
};

#endif // BRUCE_FILEMANAGERSERVICE_H
