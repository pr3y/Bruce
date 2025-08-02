#include <BLEServer.h>

class InfoService {
private:
    BLECharacteristic *info_char = NULL;
    BLEService *info_service;

public:
    InfoService(/* args */);
    ~InfoService();
    void setup(BLEServer *pServer);
    void end();
};
