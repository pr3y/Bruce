#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __DEVICE_JS_H__
#define __DEVICE_JS_H__

#include <duktape.h>

duk_ret_t putPropDeviceFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerDevice(duk_context *ctx);

duk_ret_t native_getDeviceName(duk_context *ctx);
duk_ret_t native_getBoard(duk_context *ctx);
duk_ret_t native_getBruceVersion(duk_context *ctx);
duk_ret_t native_getBattery(duk_context *ctx);
duk_ret_t native_getBatteryDetailed(duk_context *ctx);
duk_ret_t native_getFreeHeapSize(duk_context *ctx);
duk_ret_t native_getEEPROMSize(duk_context *ctx);

#endif
#endif
