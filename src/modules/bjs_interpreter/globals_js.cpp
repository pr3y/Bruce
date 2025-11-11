#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "globals_js.h"

#include "helpers_js.h"
#include "interpreter.h"

duk_ret_t putPropGlobalsFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) { return 0; }

duk_ret_t registerGlobals(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "now", native_now, 0);
    bduk_register_c_lightfunc(ctx, "delay", native_delay, 1);
    bduk_register_c_lightfunc(ctx, "parse_int", native_parse_int, 1);
    bduk_register_c_lightfunc(ctx, "to_string", native_to_string, 1);
    bduk_register_c_lightfunc(ctx, "to_hex_string", native_to_hex_string, 1);
    bduk_register_c_lightfunc(ctx, "to_lower_case", native_to_lower_case, 1);
    bduk_register_c_lightfunc(ctx, "to_upper_case", native_to_upper_case, 1);
    bduk_register_c_lightfunc(ctx, "random", native_random, 2);

    if (scriptDirpath == NULL || scriptName == NULL) {
        bduk_register_string(ctx, "__filepath", "");
        bduk_register_string(ctx, "__dirpath", "");
    } else {
        bduk_register_string(ctx, "__filepath", (String(scriptDirpath) + String(scriptName)).c_str());
        bduk_register_string(ctx, "__dirpath", scriptDirpath);
    }
    bduk_register_string(ctx, "BRUCE_VERSION", BRUCE_VERSION);
    bduk_register_int(ctx, "BRUCE_PRICOLOR", bruceConfig.priColor);
    bduk_register_int(ctx, "BRUCE_SECCOLOR", bruceConfig.secColor);
    bduk_register_int(ctx, "BRUCE_BGCOLOR", bruceConfig.bgColor);
    return 0;
}

duk_ret_t native_now(duk_context *ctx) {
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = duration_cast<milliseconds>(duration).count();
    duk_push_number(ctx, static_cast<double>(millis));
    return 1; // Return 1 value (the timestamp) to JavaScript
}

duk_ret_t native_delay(duk_context *ctx) {
    delay(duk_to_int(ctx, 0));
    return 0;
}

duk_ret_t native_random(duk_context *ctx) {
    int val;
    if (duk_is_number(ctx, 1)) {
        val = random(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
    } else {
        val = random(duk_to_int(ctx, 0));
    }
    duk_push_int(ctx, val);
    return 1;
}

duk_ret_t native_parse_int(duk_context *ctx) {
    duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);

    if (arg0Type & (DUK_TYPE_MASK_STRING | DUK_TYPE_MASK_NUMBER)) {
        duk_push_number(ctx, duk_to_number(ctx, 0));
    } else {
        duk_push_nan(ctx);
    }

    return 1;
}

duk_ret_t native_to_string(duk_context *ctx) {
    duk_push_string(ctx, duk_to_string(ctx, 0));

    return 1;
}

duk_ret_t native_to_hex_string(duk_context *ctx) {
    duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);

    if (arg0Type & (DUK_TYPE_MASK_STRING | DUK_TYPE_MASK_NUMBER | DUK_TYPE_MASK_BOOLEAN)) {
        duk_push_string(ctx, String(duk_to_int(ctx, 0), HEX).c_str());
    } else {
        duk_push_string(ctx, "");
    }

    return 1;
}

duk_ret_t native_to_lower_case(duk_context *ctx) {
    String text = duk_to_string(ctx, 0);
    text.toLowerCase();
    duk_push_string(ctx, text.c_str());

    return 1;
}

duk_ret_t native_to_upper_case(duk_context *ctx) {
    String text = duk_to_string(ctx, 0);
    text.toUpperCase();
    duk_push_string(ctx, text.c_str());

    return 1;
}

#endif
