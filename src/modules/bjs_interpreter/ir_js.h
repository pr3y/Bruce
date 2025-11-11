#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __IR_JS_H__
#define __IR_JS_H__

#include <duktape.h>

duk_ret_t putPropIRFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerIR(duk_context *ctx);

duk_ret_t native_irTransmitFile(duk_context *ctx);
duk_ret_t native_irTransmit(duk_context *ctx);
duk_ret_t native_irRead(duk_context *ctx);

#endif
#endif
