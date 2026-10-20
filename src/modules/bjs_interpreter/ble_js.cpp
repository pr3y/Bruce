#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "ble_js.h"
#include "modules/ble/ble_common.h"
#include <algorithm>
#include <cmath>

static bool rawScanOwned = false;
static bool rawScanPending = false;

JSValue native_bleStopScan(JSContext *, JSValue *, int, JSValue *) {
    if (rawScanOwned && BLEDevice::isInitialized()) {
        NimBLEScan *scan = BLEDevice::getScan();
        scan->stop();
        scan->clearResults();
        BLEDevice::deinit(true);
    }
    rawScanPending = false;
    rawScanOwned = false;
    return JS_UNDEFINED;
}

JSValue native_bleScan(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) {
    // usage: ble.scan(timeout_seconds)
    // returns: array of objects [{name, address, rssi}, ...]
    bool raw = argc > 1 && JS_ToBool(ctx, argv[1]);
    double seconds = 5;
    int timeout = 5; // legacy seconds
    if (argc > 0 && JS_IsNumber(ctx, argv[0])) {
        JS_ToNumber(ctx, &seconds, argv[0]);
        if (!std::isfinite(seconds)) return JS_ThrowRangeError(ctx, "scan duration must be finite");
        seconds = fmax(0.2, fmin(seconds, 30.0));
        timeout = (int)seconds;
    }
    if (timeout < 1) timeout = 1;
    if (timeout > 30) timeout = 30;

    uint32_t scanMs = raw ? (uint32_t)(fmax(0.2, fmin(seconds, 5.0)) * 1000) : timeout * 1000;
    const bool asynchronous = raw && argc > 2 && JS_ToBool(ctx, argv[2]);
    if (!BLEDevice::isInitialized()) BLEDevice::init("");
    if (raw) rawScanOwned = true;
    NimBLEScan *pScan = BLEDevice::getScan();
    BLEScanResults results;
#if __has_include(<NimBLEExtAdvertising.h>)
    if (asynchronous && rawScanPending) {
        if (pScan->isScanning()) return JS_UNDEFINED;
        results = pScan->getResults();
        rawScanPending = false;
    } else {
#endif
        pScan->setActiveScan(!raw);
        pScan->setInterval(100);
        pScan->setWindow(99);
        pScan->clearResults();
        pScan->setMaxResults(raw ? 64 : 255);
#if __has_include(<NimBLEExtAdvertising.h>)
        if (asynchronous) {
            if (!pScan->start(scanMs, false)) return JS_ThrowInternalError(ctx, "BLE scan failed to start");
            rawScanPending = true;
            return JS_UNDEFINED;
        }
        results = pScan->getResults(scanMs, false);
    }
#else
    results = pScan->start(timeout, false);
#endif
    int count = results.getCount();

    JSGCRef arrayRef;
    JS_PushGCRef(ctx, &arrayRef);
    arrayRef.val = JS_NewArray(ctx, 0);
    for (int i = 0; i < count; i++) {
#if __has_include(<NimBLEExtAdvertising.h>)
        const NimBLEAdvertisedDevice *dev = results.getDevice(i);
#else
        NimBLEAdvertisedDevice dev_obj = results.getDevice(i);
        NimBLEAdvertisedDevice *dev = &dev_obj;
#endif
        JSGCRef objectRef;
        JS_PushGCRef(ctx, &objectRef);
        objectRef.val = JS_NewObject(ctx);
        String name = dev->getName().c_str();
        if (name.isEmpty()) name = "<unknown>";
        {
            JSValue value = JS_NewString(ctx, name.c_str());
            JS_SetPropertyStr(ctx, objectRef.val, "name", value);
        }
        {
            JSValue value = JS_NewString(ctx, dev->getAddress().toString().c_str());
            JS_SetPropertyStr(ctx, objectRef.val, "address", value);
        }
        {
            JSValue value = JS_NewInt32(ctx, dev->getRSSI());
            JS_SetPropertyStr(ctx, objectRef.val, "rssi", value);
        }
        if (raw) {
            if (dev->haveManufacturerData()) {
                const auto mfr = dev->getManufacturerData();
                String mh;
                static const char hd[] = "0123456789abcdef";
                for (size_t j = 0; j < mfr.length() && j < 255; j++) {
                    unsigned char c = mfr[j];
                    mh += hd[c >> 4];
                    mh += hd[c & 15];
                }
                JSValue mv = JS_NewString(ctx, mh.c_str());
                JS_SetPropertyStr(ctx, objectRef.val, "manufacturer", mv);
            }
            const auto &payload = dev->getPayload();
            String hex;
            const size_t size = std::min((size_t)payload.size(), (size_t)255);
            hex.reserve(size * 2);
            static const char digits[] = "0123456789abcdef";
            for (size_t k = 0; k < size; ++k) {
                hex += digits[payload[k] >> 4];
                hex += digits[payload[k] & 15];
            }
            {
                JSValue value = JS_NewString(ctx, hex.c_str());
                JS_SetPropertyStr(ctx, objectRef.val, "payload", value);
            }
            {
                JSValue value = JS_NewInt32(ctx, dev->getAddress().getType());
                JS_SetPropertyStr(ctx, objectRef.val, "addressType", value);
            }
            {
                JSValue value = JS_NewBool(payload.size() > size);
                JS_SetPropertyStr(ctx, objectRef.val, "truncated", value);
            }
        }
        JS_SetPropertyUint32(ctx, arrayRef.val, i, objectRef.val);
        JS_PopGCRef(ctx, &objectRef);
    }

    pScan->clearResults();
    // Raw scans retain the stack until stopScan / interpreter cleanup.
    return JS_PopGCRef(ctx, &arrayRef);
}

JSValue native_bleAdvertise(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) {
    // usage: ble.advertise(name_string)
    const char *name = "Bruce-App";
    JSCStringBuf name_buf;
    if (argc > 0 && JS_IsString(ctx, argv[0])) {
        name = JS_ToCString(ctx, argv[0], &name_buf);
    }

    BLEDevice::init(name);
    NimBLEServer *pServer = BLEDevice::createServer();
    pServer->getAdvertising()->start();

    return JS_NewBool(true);
}

JSValue native_bleStopAdvertise(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) {
    NimBLEDevice::getAdvertising()->stop();
    BLEDevice::deinit();
    return JS_UNDEFINED;
}

#endif
