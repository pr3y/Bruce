#include <BLEServer.h>

class BatteryService {
private:
    BLECharacteristic *battery_char = NULL;
    void battery_handler_task();
    TaskHandle_t battery_task_handle = NULL;
    BLEService *pBatSvc;

public:
    BatteryService(/* args */);
    ~BatteryService();
    void setup(BLEServer *pServer);
    void end();
};
