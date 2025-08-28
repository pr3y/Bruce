#ifndef LITE_VERSION
#ifndef __WIFI_JS_H__
#define __WIFI_JS_H__

#include <duktape.h>

duk_ret_t native_wifiConnected(duk_context *ctx);
duk_ret_t native_wifiConnectDialog(duk_context *ctx);
duk_ret_t native_wifiConnect(duk_context *ctx);
duk_ret_t native_wifiScan(duk_context *ctx);
duk_ret_t native_wifiDisconnect(duk_context *ctx);
duk_ret_t native_httpFetch(duk_context *ctx);

#endif
#endif
