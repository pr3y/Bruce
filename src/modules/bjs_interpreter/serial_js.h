#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __SERIAL_JS_H__
#define __SERIAL_JS_H__

#include <duktape.h>

duk_ret_t putPropSerialFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerSerial(duk_context *ctx);
void registerConsole(duk_context *ctx);

duk_ret_t native_serialPrint(duk_context *ctx);
duk_ret_t native_serialPrintln(duk_context *ctx);
duk_ret_t native_serialReadln(duk_context *ctx);
duk_ret_t native_serialCmd(duk_context *ctx);

#endif
#endif
