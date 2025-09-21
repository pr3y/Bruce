#include "BLESerialService.h"
#include <NimBLEDevice.h>

BLESerialService::BLESerialService() : BruceBLEService() {}

BLESerialService::~BLESerialService() {}

void BLESerialService::setup(NimBLEServer *pServer) {

    pService = pServer->createService("4371ec0b-3d43-49f9-b731-7c72a4a7bb91");

    battery_char = pService->createCharacteristic(
        "d555ed97-bf2a-4f46-b3eb-d1fcdd7325e9", // Battery Level
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::WRITE
    );

    pService->start();
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
}

void BLESerialService::end() {
}

int BLESerialService::available() {
    NimBLEAttValue new_value = battery_char->getValue();
    if (value == new_value) return 0;
    value = new_value;

    return value.size();
}

size_t BLESerialService::println(const String &s) {
    String toSend = s + "\r\n";
    battery_char->setValue(toSend);
    battery_char->notify();
    return toSend.length();
}

size_t BLESerialService::print(const String &s) {
    battery_char->setValue(s);
    battery_char->notify();
    return s.length();
}

size_t BLESerialService::println(size_t n) {
    String s = String(n);
    return println(s);
}

void BLESerialService::vprintf(const char * fmt, va_list args) {
    int size = sprintf(nullptr, fmt, args);
    char str[BUFFER_SIZE];
    sprintf(str, fmt, args);

    battery_char->setValue(reinterpret_cast<const uint8_t *>(str), size);
    battery_char->notify();
}

String BLESerialService::readStringUntil(char terminator) {
    Serial.println("readStringUntil");
    String result = "";
    std::string value = battery_char->getValue();
    for (char c : value) {
        result += c;
        if (c == terminator) break;
    }
    return result;
}

size_t BLESerialService::println(const uint32_t n) {
    String s = String(n);
    return println(s);
}

size_t BLESerialService::print(const int n, int format) {
    String s = String(n, format);
    return print(s);
}

size_t BLESerialService::println(const int n, int format) {
    String s = String(n, format);
    return println(s);
}

size_t BLESerialService::println() {
    return println("");
}