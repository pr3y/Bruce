#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __NOTIFICATION_JS_H__
#define __NOTIFICATION_JS_H__

#include <duktape.h>

duk_ret_t putPropNotificationFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerNotification(duk_context *ctx);

duk_ret_t native_notifyBlink(duk_context *ctx);

#endif
#endif
