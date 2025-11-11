#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __I2C_JS_H__
#define __I2C_JS_H__

#include <duktape.h>

duk_ret_t putPropI2CFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerI2C(duk_context *ctx);

duk_ret_t native_i2c_begin(duk_context *ctx);
duk_ret_t native_i2c_scan(duk_context *ctx);
duk_ret_t native_i2c_write(duk_context *ctx);
duk_ret_t native_i2c_read(duk_context *ctx);
duk_ret_t native_i2c_write_read(duk_context *ctx);

#endif
#endif
