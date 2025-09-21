#pragma once

#include <NimBLEServer.h>
#include "BruceBLEService.hpp"

#include <NimBLEAttValue.h>
#include <SerialDevice.h>

#define BUFFER_SIZE 128

class BLESerialService: public BruceBLEService, public SerialDevice {
    NimBLECharacteristic *battery_char = nullptr;
    NimBLEAttValue value;
public:
    BLESerialService();
    ~BLESerialService() override;
    void setup(NimBLEServer *pServer) override;
    void end() override;
    size_t println() override;
    size_t println(size_t n) override;
    size_t println(const String &s) override;
    size_t println(int n, int format) override;
    size_t print(const String &s) override;
    size_t print(int n, int format = DEC) override;
    void vprintf(const char *str, va_list args) override;
    size_t println(uint32_t n) override;
    void flush() override {}
    String readStringUntil(char terminator) override;
    int available() override;
};
