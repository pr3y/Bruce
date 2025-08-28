#ifndef LITE_VERSION
#include "helpers_js.h"
#include "core/sd_functions.h"
#include <globals.h>

// Commented because it is not used for now
// void registerFunction(duk_context *ctx, const char *name, duk_c_function
// func, duk_idx_t nargs) { 	duk_push_c_function(ctx, func, nargs);
// 	duk_put_global_string(ctx, name);
// }

void bduk_register_c_lightfunc(
    duk_context *ctx, const char *name, duk_c_function func, duk_idx_t nargs, duk_idx_t magic
) {
    duk_push_c_lightfunc(ctx, func, nargs, nargs == DUK_VARARGS ? 15 : nargs, magic);

    duk_put_global_string(ctx, name);
}

void bduk_register_int(duk_context *ctx, const char *name, duk_int_t val) {
    duk_push_int(ctx, val);
    duk_put_global_string(ctx, name);
}

void bduk_register_string(duk_context *ctx, const char *name, const char *val) {
    duk_push_string(ctx, val);
    duk_put_global_string(ctx, name);
}

void bduk_put_prop_c_lightfunc(
    duk_context *ctx, duk_idx_t obj_idx, const char *name, duk_c_function func, duk_idx_t nargs,
    duk_idx_t magic
) {
    duk_push_c_lightfunc(ctx, func, nargs, nargs == DUK_VARARGS ? 15 : nargs, magic);

    duk_put_prop_string(ctx, obj_idx, name);
}

FileParamsJS js_get_path_from_params(duk_context *ctx, bool checkIfexist, bool legacy) {
    FileParamsJS filePath;
    String fsParam = legacy ? duk_to_string(ctx, 0) : "";
    fsParam.toLowerCase();
    filePath.paramOffset = 1;
    filePath.exist = false;

    // if function(path: {fs: string, path: string})
    if (duk_is_object(ctx, 0)) {
        duk_get_prop_string(ctx, 0, "fs");
        fsParam = duk_to_string(ctx, -1);
        duk_get_prop_string(ctx, 0, "path");
        filePath.path = duk_to_string(ctx, -1);
        duk_pop_2(ctx);
        filePath.paramOffset = 0;
    }

    if (fsParam == "sd") {
        filePath.fs = &SD;
    } else if (fsParam == "littlefs") {
        filePath.fs = &LittleFS;
    } else {
        // if function(path: string)
        filePath.paramOffset = 0;
        filePath.path = duk_to_string(ctx, 0);

        if (sdcardMounted && checkIfexist && SD.exists(filePath.path)) {
            filePath.fs = &SD;
        } else {
            filePath.fs = &LittleFS;
        }
    }

    if (filePath.paramOffset == 1) {
        // if function(fs: string, path: string)
        filePath.path = duk_to_string(ctx, 1);
    }

    if (checkIfexist) {
        if ((filePath.fs)->exists(filePath.path)) {
            filePath.exist = true;
        } else {
            filePath.exist = false;
        }
    }

    return filePath;
}
#endif
