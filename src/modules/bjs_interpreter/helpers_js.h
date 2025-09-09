#ifndef __HELPERS_JS_H__
#define __HELPERS_JS_H__
#ifndef LITE_VERSION
#include <FS.h>
#include <duktape.h>
#include <string.h>

void bduk_register_c_lightfunc(
    duk_context *ctx, const char *name, duk_c_function func, duk_idx_t nargs, duk_idx_t magic = 0
);

void bduk_register_int(duk_context *ctx, const char *name, duk_int_t val);

void bduk_register_string(duk_context *ctx, const char *name, const char *val);

#define bduk_put_prop(ctx, obj_idx, name, prop_type, prop_value)                                             \
    do {                                                                                                     \
        prop_type((ctx), (prop_value));                                                                      \
        duk_put_prop_string((ctx), (obj_idx), (name));                                                       \
    } while (0)

void bduk_put_prop_c_lightfunc(
    duk_context *ctx, duk_idx_t obj_idx, const char *name, duk_c_function func, duk_idx_t nargs,
    duk_idx_t magic = 0
);

struct FileParamsJS {
    FS *fs;
    String path;
    bool exist;
    u_int8_t paramOffset;
};

FileParamsJS js_get_path_from_params(duk_context *ctx, bool checkIfexist = true, bool legacy = false);

#endif
#endif
