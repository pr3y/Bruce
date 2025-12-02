#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __MATH_JS_H__
#define __MATH_JS_H__

#include <duktape.h>

duk_ret_t putPropMathFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerMath(duk_context *ctx);

duk_ret_t native_math_acosh(duk_context *ctx);
duk_ret_t native_math_asinh(duk_context *ctx);
duk_ret_t native_math_atanh(duk_context *ctx);
duk_ret_t native_math_is_equal(duk_context *ctx);

#endif
#endif
