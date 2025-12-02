#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __GPIO_JS_H__
#define __GPIO_JS_H__

#include <duktape.h>

duk_ret_t putPropGPIOFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerGPIO(duk_context *ctx);

duk_ret_t native_digitalWrite(duk_context *ctx);
duk_ret_t native_analogWrite(duk_context *ctx);
duk_ret_t native_digitalRead(duk_context *ctx);
duk_ret_t native_analogRead(duk_context *ctx);
duk_ret_t native_touchRead(duk_context *ctx);
duk_ret_t native_dacWrite(duk_context *ctx);
duk_ret_t native_ledcSetup(duk_context *ctx);
duk_ret_t native_ledcAttachPin(duk_context *ctx);
duk_ret_t native_ledcWrite(duk_context *ctx);
duk_ret_t native_pinMode(duk_context *ctx);
duk_ret_t native_pins(duk_context *ctx);

#endif
#endif
