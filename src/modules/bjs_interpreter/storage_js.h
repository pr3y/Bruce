#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __STORAGE_JS_H__
#define __STORAGE_JS_H__

#include <SD.h>
#include <duktape.h>

duk_ret_t putPropStorageFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerStorage(duk_context *ctx);

duk_ret_t native_storageReaddir(duk_context *ctx);
duk_ret_t native_storageRead(duk_context *ctx);
duk_ret_t native_storageWrite(duk_context *ctx);
duk_ret_t native_storageRename(duk_context *ctx);
duk_ret_t native_storageRemove(duk_context *ctx);
duk_ret_t native_storageMkdir(duk_context *ctx);
duk_ret_t native_storageRmdir(duk_context *ctx);
duk_ret_t native_storageSpaceLittleFS(duk_context *ctx);
duk_ret_t native_storageSpaceSDCard(duk_context *ctx);

#endif
#endif
