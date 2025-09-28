#ifndef BRUCE_USBSERIAL_H
#define BRUCE_USBSERIAL_H

#include "SerialDevice.h"
#include <Arduino.h>

class USBSerial : public SerialDevice {
public:
    size_t println(const String &s) override {
        return Serial.println(s);
    }
    size_t print(const String &s) override {
        return Serial.print(s);
    }
    size_t print(const int n, int format) override {
        return Serial.print(n, format);
    }
    void vprintf(const char * fmt, va_list args) override {
        Serial.printf(fmt, args);
    }
    size_t println() override {
        return Serial.println();
    }
    size_t println(size_t n) override {
        return Serial.println(n);
    }
    size_t println(const uint32_t n) override {
        return Serial.println(n);
    }
    size_t println(const int n, int format) override {
        return Serial.println(n, format);
    }
    String readStringUntil(char terminator) override {
        return Serial.readStringUntil(terminator);
    }
    size_t write(uint8_t *str, size_t size) override {
        return Serial.write(str, size);
    }
    void flush() override {
        Serial.flush();
    }
    int available() override {
        return Serial.available();
    }
    ~USBSerial() override = default;
};

#endif // BRUCE_USBSERIAL_H
