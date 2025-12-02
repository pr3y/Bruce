#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __BADUSB_JS_H__
#define __BADUSB_JS_H__

#include <duktape.h>

duk_ret_t putPropBadUSBFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerBadUSB(duk_context *ctx);

duk_ret_t native_badusbRunFile(duk_context *ctx);
duk_ret_t native_badusbSetup(duk_context *ctx);
duk_ret_t native_badusbPrint(duk_context *ctx);
duk_ret_t native_badusbPrintln(duk_context *ctx);
duk_ret_t native_badusbPress(duk_context *ctx);
duk_ret_t native_badusbHold(duk_context *ctx);
duk_ret_t native_badusbRelease(duk_context *ctx);
duk_ret_t native_badusbReleaseAll(duk_context *ctx);
duk_ret_t native_badusbPressRaw(duk_context *ctx);

#endif
#endif
