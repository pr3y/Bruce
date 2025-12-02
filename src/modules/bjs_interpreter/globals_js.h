#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __GLOBALS_JS_H__
#define __GLOBALS_JS_H__

#include <chrono>
#include <duktape.h>

duk_ret_t putPropGlobalsFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerGlobals(duk_context *ctx);

duk_ret_t native_now(duk_context *ctx);
duk_ret_t native_delay(duk_context *ctx);
duk_ret_t native_random(duk_context *ctx);
duk_ret_t native_parse_int(duk_context *ctx);
duk_ret_t native_to_string(duk_context *ctx);
duk_ret_t native_to_hex_string(duk_context *ctx);
duk_ret_t native_to_lower_case(duk_context *ctx);
duk_ret_t native_to_upper_case(duk_context *ctx);

#endif
#endif
