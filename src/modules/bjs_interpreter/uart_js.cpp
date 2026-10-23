#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "uart_js.h"

#include "globals.h"
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <vector>

namespace {
bool uartReady = false;
bool gpsMode = false;
TinyGPSPlus blindSpotParser;
uint32_t gpsLastByte = 0, gpsLastSentence = 0;
String gpsLine, gpsLastLine;
HardwareSerial blindSpotGPS(2);
HardwareSerial *activeSerial = &Serial1;

bool requireUart(JSContext *ctx) {
    if (uartReady) return true;
    JS_ThrowInternalError(ctx, "uart not initialized: call uart.begin(baud, rxPin, txPin) first");
    return false;
}
} // namespace

JSValue native_uart_begin(JSContext *ctx, JSValue *, int argc, JSValue *argv) {
    if (argc == 0) {
        if (uartReady) activeSerial->end();
        activeSerial = &blindSpotGPS;
        gpsMode = true;
        blindSpotParser = TinyGPSPlus();
        gpsLastByte = gpsLastSentence = 0;
        gpsLine = "";
        gpsLastLine = "";
        ioExpander.turnPinOnOff(IO_EXP_GPS, HIGH);
#ifdef USE_BOOST
        hal_pmic_enable_otg();
#endif
        activeSerial->setRxBufferSize(4096);
        pinMode(bruceConfigPins.gps_bus.rx, INPUT);
        activeSerial->begin(
            bruceConfigPins.gpsBaudrate, SERIAL_8N1, bruceConfigPins.gps_bus.rx, bruceConfigPins.gps_bus.tx
        );
        uartReady = true;
        String info = String("RX ") + bruceConfigPins.gps_bus.rx + " TX " + bruceConfigPins.gps_bus.tx +
                      " @ " + bruceConfigPins.gpsBaudrate;
        return JS_NewString(ctx, info.c_str());
    }
    if (argc < 3 || !JS_IsNumber(ctx, argv[0]) || !JS_IsNumber(ctx, argv[1]) ||
        !JS_IsNumber(ctx, argv[2])) {
        return JS_ThrowTypeError(ctx, "uart.begin(baud:int, rxPin:int, txPin:int) is required");
    }

    uint32_t baud = 0;
    int rxPin = -1;
    int txPin = -1;
    JS_ToUint32(ctx, &baud, argv[0]);
    JS_ToInt32(ctx, &rxPin, argv[1]);
    JS_ToInt32(ctx, &txPin, argv[2]);
    if (baud < 300 || baud > 5000000) return JS_ThrowRangeError(ctx, "uart.begin: baud must be 300..5000000");
    if (rxPin < 0 || txPin < 0) return JS_ThrowRangeError(ctx, "uart.begin: pins must be >= 0");

    if (uartReady) activeSerial->end();
    activeSerial = &Serial1;
    gpsMode = false;
    activeSerial->setRxBufferSize(4096);
    activeSerial->begin(baud, SERIAL_8N1, rxPin, txPin);
    uartReady = true;
    return JS_NewBool(true);
}

JSValue native_uart_gpsRead(JSContext *ctx, JSValue *, int, JSValue *) {
    if (!requireUart(ctx)) return JS_EXCEPTION;
    if (!gpsMode) return JS_ThrowTypeError(ctx, "GPS reader requires uart.begin() using Bruce settings");
    unsigned readCount = 0;
    while (activeSerial->available() && readCount++ < 4096) {
        char c = static_cast<char>(activeSerial->read());
        gpsLastByte = millis();
        if (c == '$') gpsLine = "$";
        else if (c == '\n') {
            gpsLastLine = gpsLine;
            gpsLine = "";
        } else if (c != '\r' && c >= 32 && c <= 126 && gpsLine.length() < 160) gpsLine += c;
        if (blindSpotParser.encode(c)) gpsLastSentence = millis();
    }
    const uint32_t now = millis();
    JsonDocument d;
    d["rx"] = static_cast<int>(bruceConfigPins.gps_bus.rx);
    d["tx"] = static_cast<int>(bruceConfigPins.gps_bus.tx);
    d["baud"] = bruceConfigPins.gpsBaudrate;
    d["bytes"] = blindSpotParser.charsProcessed();
    d["sentences"] = blindSpotParser.passedChecksum();
    d["badChecksum"] = blindSpotParser.failedChecksum();
    d["byteAge"] = blindSpotParser.charsProcessed() ? now - gpsLastByte : 0xffffffffu;
    d["sentenceAge"] = blindSpotParser.passedChecksum() ? now - gpsLastSentence : 0xffffffffu;
    d["satellites"] = blindSpotParser.satellites.isValid() ? blindSpotParser.satellites.value() : 0;
    d["lastLine"] = gpsLastLine;
    bool fix = blindSpotParser.location.isValid() && blindSpotParser.location.age() < 2000;
    d["fix"] = fix;
    if (fix && blindSpotParser.date.isValid() && blindSpotParser.time.isValid() &&
        blindSpotParser.time.age() < 3000 && blindSpotParser.date.year() >= 2020) {
        char utc[25];
        snprintf(
            utc,
            sizeof(utc),
            "%04d-%02d-%02dT%02d:%02d:%02dZ",
            blindSpotParser.date.year(),
            blindSpotParser.date.month(),
            blindSpotParser.date.day(),
            blindSpotParser.time.hour(),
            blindSpotParser.time.minute(),
            blindSpotParser.time.second()
        );
        JsonObject f = d["position"].to<JsonObject>();
        f["lat"] = blindSpotParser.location.lat();
        f["lon"] = blindSpotParser.location.lng();
        f["ts"] = utc;
        if (blindSpotParser.altitude.isValid()) f["alt"] = blindSpotParser.altitude.meters();
    }
    String result;
    serializeJson(d, result);
    return JS_NewString(ctx, result.c_str());
}

JSValue native_uart_write(JSContext *ctx, JSValue *, int argc, JSValue *argv) {
    if (!requireUart(ctx)) return JS_EXCEPTION;
    if (argc < 1) return JS_ThrowTypeError(ctx, "uart.write: byte array, Uint8Array, or string required");

    const uint8_t *data = nullptr;
    size_t length = 0;
    std::vector<uint8_t> bytes;
    JSCStringBuf stringBuffer;

    if (JS_IsString(ctx, argv[0])) {
        const char *stringData = JS_ToCStringLen(ctx, &length, argv[0], &stringBuffer);
        if (!stringData) return JS_ThrowTypeError(ctx, "uart.write: invalid string");
        data = reinterpret_cast<const uint8_t *>(stringData);
    } else if (JS_IsTypedArray(ctx, argv[0])) {
        const char *typedData = JS_GetTypedArrayBuffer(ctx, &length, argv[0]);
        if (!typedData) return JS_ThrowTypeError(ctx, "uart.write: invalid typed array");
        data = reinterpret_cast<const uint8_t *>(typedData);
    } else if (JS_IsObject(ctx, argv[0]) && JS_GetClassID(ctx, argv[0]) == JS_CLASS_ARRAY) {
        JSValue lengthValue = JS_GetPropertyStr(ctx, argv[0], "length");
        uint32_t arrayLength = 0;
        if (!JS_IsNumber(ctx, lengthValue) || JS_ToUint32(ctx, &arrayLength, lengthValue) < 0) {
            return JS_ThrowTypeError(ctx, "uart.write: invalid byte array");
        }
        if (arrayLength > 4096) return JS_ThrowRangeError(ctx, "uart.write: maximum length is 4096 bytes");
        bytes.reserve(arrayLength);
        for (uint32_t i = 0; i < arrayLength; ++i) {
            JSValue item = JS_GetPropertyUint32(ctx, argv[0], i);
            int value = 0;
            if (!JS_IsNumber(ctx, item) || JS_ToInt32(ctx, &value, item) < 0 || value < 0 || value > 255) {
                return JS_ThrowRangeError(ctx, "uart.write: array values must be bytes (0..255)");
            }
            bytes.push_back(static_cast<uint8_t>(value));
        }
        data = bytes.data();
        length = bytes.size();
    } else {
        return JS_ThrowTypeError(ctx, "uart.write: byte array, Uint8Array, or string required");
    }

    return JS_NewInt32(ctx, static_cast<int32_t>(activeSerial->write(data, length)));
}

JSValue native_uart_read(JSContext *ctx, JSValue *, int argc, JSValue *argv) {
    if (!requireUart(ctx)) return JS_EXCEPTION;
    int maximum = 256;
    if (argc > 0) {
        if (!JS_IsNumber(ctx, argv[0]) || JS_ToInt32(ctx, &maximum, argv[0]) < 0) {
            return JS_ThrowTypeError(ctx, "uart.read: maxLength must be an integer");
        }
    }
    if (maximum < 0 || maximum > 4096) return JS_ThrowRangeError(ctx, "uart.read: maxLength must be 0..4096");

    JSGCRef result;
    JS_PushGCRef(ctx, &result);
    result.val = JS_NewArray(ctx, 0);
    uint32_t index = 0;
    while (index < static_cast<uint32_t>(maximum) && activeSerial->available())
        JS_SetPropertyUint32(ctx, result.val, index++, JS_NewInt32(ctx, activeSerial->read()));
    return JS_PopGCRef(ctx, &result);
}

JSValue native_uart_available(JSContext *ctx, JSValue *, int, JSValue *) {
    if (!requireUart(ctx)) return JS_EXCEPTION;
    return JS_NewInt32(ctx, activeSerial->available());
}

JSValue native_uart_flush(JSContext *ctx, JSValue *, int, JSValue *) {
    if (!requireUart(ctx)) return JS_EXCEPTION;
    activeSerial->flush();
    return JS_UNDEFINED;
}

JSValue native_uart_end(JSContext *, JSValue *, int, JSValue *) {
    if (uartReady) activeSerial->end();
    if (gpsMode) {
        // Restore BOTH UART pins: TX can also be the nRF CE pin. Leaving it HIGH
        // after UART shutdown keeps the radio active across an app exit/restart.
        const int pins[] = {bruceConfigPins.gps_bus.rx, bruceConfigPins.gps_bus.tx};
        for (int pin : pins) {
            if (pin == bruceConfigPins.CC1101_bus.cs || pin == bruceConfigPins.NRF24_bus.cs ||
                pin == bruceConfigPins.SDCARD_bus.cs) {
                digitalWrite(pin, HIGH);
                pinMode(pin, OUTPUT);
            } else if (pin == bruceConfigPins.NRF24_bus.io0) {
                digitalWrite(pin, LOW);
                pinMode(pin, OUTPUT);
            }
        }
        ioExpander.turnPinOnOff(IO_EXP_GPS, LOW);
#ifdef USE_BOOST
        hal_pmic_disable_otg();
#endif
    }
    gpsMode = false;
    uartReady = false;
    return JS_UNDEFINED;
}

#endif
