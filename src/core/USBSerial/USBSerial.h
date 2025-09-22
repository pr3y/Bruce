#ifndef BRUCE_USBSERIAL_H
#define BRUCE_USBSERIAL_H

#include "SerialDevice.h"
#include <Arduino.h>

class USBSerial : public SerialDevice {
public:
    size_t println(const String &s) override { return out->println(s); }
    size_t print(const String &s) override { return out->print(s); }
    size_t print(const int n, int format) override { return out->print(n, format); }
    void vprintf(const char *fmt, va_list args) override { out->printf(fmt, args); }
    size_t println() override { return out->println(); }
    size_t println(size_t n) override { return out->println(n); }
    size_t println(const uint32_t n) override { return out->println(n); }
    size_t println(const int n, int format) override { return out->println(n, format); }
    String readStringUntil(char terminator) override { return out->readStringUntil(terminator); }
    void flush() override { out->flush(); }
    int available() override { return out->available(); }
    void setSerialOutput(Stream *in) { out = in; }
    USBSerial(Stream *in = &Serial) { out = in; }
    ~USBSerial() override = default;

private:
    Stream *out;
};

#endif // BRUCE_USBSERIAL_H
