#ifndef __GUI_JS_H__
#define __GUI_JS_H__
#ifndef LITE_VERSION
#include <duktape.h>

duk_ret_t native_dialogMessage(duk_context *ctx);
duk_ret_t native_dialogNotification(duk_context *ctx);
duk_ret_t native_dialogPickFile(duk_context *ctx);
duk_ret_t native_dialogChoice(duk_context *ctx);
duk_ret_t native_dialogViewFile(duk_context *ctx);
duk_ret_t native_dialogViewText(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewer(duk_context *ctx);
duk_ret_t native_drawStatusBar(duk_context *ctx);

#endif
#endif
