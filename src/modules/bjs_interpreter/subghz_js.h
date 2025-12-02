#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __SUBGHZ_JS_H__
#define __SUBGHZ_JS_H__

#include <duktape.h>

duk_ret_t putPropSubGHzFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerSubGHz(duk_context *ctx);

duk_ret_t native_subghzTransmitFile(duk_context *ctx);
duk_ret_t native_subghzTransmit(duk_context *ctx);
duk_ret_t native_subghzRead(duk_context *ctx);
duk_ret_t native_subghzReadRaw(duk_context *ctx);
duk_ret_t native_subghzSetFrequency(duk_context *ctx);

#endif
#endif
