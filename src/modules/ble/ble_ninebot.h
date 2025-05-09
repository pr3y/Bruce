#ifndef __BLE_SCOOTER_H__
#define __BLE_SCOOTER_H__

#include <Arduino.h>
#include <NimBLEDevice.h>

#include <globals.h>
#include "core/display.h"

class BLENinebot {
public:
    BLENinebot();
    ~BLENinebot();

    void setup();
    void loop();

private:

    void clientDisconnect(void);
    void redrawMainBorder(void);
};

#endif
