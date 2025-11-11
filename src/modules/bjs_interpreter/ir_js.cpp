#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "ir_js.h"

#include "modules/ir/ir_read.h"

#include "helpers_js.h"

duk_ret_t putPropIRFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "read", native_irRead, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "readRaw", native_irRead, 1, magic + 1);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "transmitFile", native_irTransmitFile, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "transmit", native_irTransmit, 3, magic);
    // TODO: transmit(string)
    return 0;
}

duk_ret_t registerIR(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "irRead", native_irRead, 1);
    bduk_register_c_lightfunc(ctx, "irReadRaw", native_irRead, 1, 1);
    bduk_register_c_lightfunc(ctx, "irTransmitFile", native_irTransmitFile, 1);
    bduk_register_c_lightfunc(ctx, "irTransmit", native_irTransmit, 3);
    return 0;
}

duk_ret_t native_irTransmitFile(duk_context *ctx) {
    // usage: irTransmitFile(filename : string, hideDefaultUI : boolean);
    // returns: bool==true on success, false on any error

    // Get the filename (required)
    const char *filename = duk_to_string(ctx, 0);

    // Default for the hideDefaultUI parameter
    bool hideDefaultUI = false;

    // Check if second argument exists and is boolean
    if (duk_get_top(ctx) > 1 && duk_is_boolean(ctx, 1)) { hideDefaultUI = duk_to_boolean(ctx, 1); }

    bool r = parseSerialCommand("ir tx_from_file " + String(filename) + " " + String(hideDefaultUI));
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_irTransmit(duk_context *ctx) {
    // usage: irTransmit(data: string, protocol : string = "NEC", bits: int = 32);
    // returns: bool==true on success, false on any error
    bool r = parseSerialCommand(
        "IRSend {'Data':'" + String(duk_to_string(ctx, 0)) + "','Protocol':'" +
        String(duk_get_string_default(ctx, 1, "NEC")) +
        "','Bits':" + String(duk_get_uint_default(ctx, 2, 32)) + "}"
    );
    // TODO: ALT usage: irTransmit(protocol : string, address: int, command: int);
    // TODO: bool r = parseSerialCommand("ir tx " + String(duk_to_string(ctx, 0)) + " " +
    // String(duk_get_uint_default(ctx, 1)) + " " + String(duk_to_string(ctx, 2)) + " " +
    // String(duk_to_string(ctx, 3)) );
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_irRead(duk_context *ctx) {
    // usage: irRead();
    // usage: irRead(timeout_in_seconds : number);
    // returns a string of the generated ir file, empty string on timeout or other
    // errors
    duk_int_t magic = duk_get_current_magic(ctx);
    IrRead irRead = IrRead(true, magic); // true == headless mode, true==raw mode
    String result = irRead.loop_headless(duk_get_int_default(ctx, 0, 10));
    duk_push_string(ctx, result.c_str());
    return 1;
}

#endif
