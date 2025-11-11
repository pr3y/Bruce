#ifndef __DIALOG_JS_H__
#define __DIALOG_JS_H__
#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include <duktape.h>

duk_ret_t putPropDialogFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerDialog(duk_context *ctx);

duk_ret_t native_dialogMessage(duk_context *ctx);
duk_ret_t native_dialogNotification(duk_context *ctx);
duk_ret_t native_dialogPickFile(duk_context *ctx);
duk_ret_t native_dialogChoice(duk_context *ctx);
duk_ret_t native_dialogViewFile(duk_context *ctx);
duk_ret_t native_dialogViewText(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewer(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerDraw(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerScrollUp(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerScrollDown(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerScrollToLine(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerGetLine(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerGetMaxLines(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerGetVisibleText(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerClear(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerFromString(duk_context *ctx);
duk_ret_t native_dialogCreateTextViewerClose(duk_context *ctx);
duk_ret_t native_drawStatusBar(duk_context *ctx);

#endif
#endif
