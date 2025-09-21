#include "../src/modules/ble_api/services/BruceBLEService.hpp"
#include <NimBLEServer.h>

class InfoService : public BruceBLEService {
private:
    NimBLECharacteristic *info_char = nullptr;

public:
    InfoService(/* args */);
    ~InfoService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
};
