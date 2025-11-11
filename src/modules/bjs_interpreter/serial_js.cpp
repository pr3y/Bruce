#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "serial_js.h"

#include "display_js.h"

#include "helpers_js.h"

duk_ret_t putPropSerialFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "print", native_serialPrint, DUK_VARARGS, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "println", native_serialPrintln, DUK_VARARGS, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "readln", native_serialReadln, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "cmd", native_serialCmd, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "write", native_serialPrint, DUK_VARARGS, magic);
    return 0;
}

duk_ret_t registerSerial(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "serialReadln", native_serialReadln, 1);
    bduk_register_c_lightfunc(ctx, "serialPrintln", native_serialPrintln, DUK_VARARGS);
    bduk_register_c_lightfunc(ctx, "serialCmd", native_serialCmd, 1);
    return 0;
}

void registerConsole(duk_context *ctx) {
    duk_idx_t obj_idx = duk_push_object(ctx);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "log", native_serialPrintln, DUK_VARARGS);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "debug", native_serialPrintln, DUK_VARARGS, 2);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "warn", native_serialPrintln, DUK_VARARGS, 3);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "error", native_serialPrintln, DUK_VARARGS, 4);

    duk_put_global_string(ctx, "console");
}

duk_ret_t native_serialPrint(duk_context *ctx) {
    internal_print(ctx, false, false);
    return 0;
}

duk_ret_t native_serialPrintln(duk_context *ctx) {
    internal_print(ctx, false, true);
    return 0;
}

duk_ret_t native_serialReadln(duk_context *ctx) {
    // usage: serialReadln();   // default to 10s timeout
    // usage: serialReadln(timeout_in_ms : number);
    String line;
    int maxloops = 1000 * 10;
    if (duk_is_number(ctx, 0)) maxloops = duk_to_int(ctx, 0);
    Serial.flush();
    while (maxloops) {
        if (!Serial.available()) {
            maxloops -= 1;
            delay(1);
            continue;
        }
        // data is ready to read
        line = Serial.readStringUntil('\n');
    }
    duk_push_string(ctx, line.c_str());
    return 1;
}

duk_ret_t native_serialCmd(duk_context *ctx) {
    bool r = parseSerialCommand(String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

#endif
