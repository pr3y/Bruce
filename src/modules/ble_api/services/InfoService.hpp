#include <NimBLEServer.h>

class InfoService {
private:
    NimBLECharacteristic *info_char = nullptr;
    NimBLEService *info_service;

public:
    InfoService(/* args */);
    ~InfoService();
    void setup(NimBLEServer *pServer);
    void end();
};
