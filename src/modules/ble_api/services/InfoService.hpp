#include <NimBLEServer.h>
#include "BruceBLEService.hpp"

class InfoService : public BruceBLEService {
private:
    NimBLECharacteristic *info_char = nullptr;

public:
    InfoService(/* args */);
    ~InfoService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
};
