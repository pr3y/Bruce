#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __KEYBOARD_JS_H__
#define __KEYBOARD_JS_H__

#include <duktape.h>

duk_ret_t putPropKeyboardFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerKeyboard(duk_context *ctx);

duk_ret_t native_keyboard(duk_context *ctx);
duk_ret_t native_getPrevPress(duk_context *ctx);
duk_ret_t native_getSelPress(duk_context *ctx);
duk_ret_t native_getEscPress(duk_context *ctx);
duk_ret_t native_getNextPress(duk_context *ctx);
duk_ret_t native_getAnyPress(duk_context *ctx);
duk_ret_t native_getKeysPressed(duk_context *ctx);
duk_ret_t native_setLongPress(duk_context *ctx);

#endif
#endif
