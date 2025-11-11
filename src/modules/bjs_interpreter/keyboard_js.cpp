#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "keyboard_js.h"

#include "core/mykeyboard.h"

#include "helpers_js.h"

duk_ret_t putPropKeyboardFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "keyboard", native_keyboard, 3, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getKeysPressed", native_getKeysPressed, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getPrevPress", native_getPrevPress, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getSelPress", native_getSelPress, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getEscPress", native_getEscPress, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getNextPress", native_getNextPress, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getAnyPress", native_getAnyPress, 1, magic);
    return 0;
}

duk_ret_t registerKeyboard(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "getKeysPressed", native_getKeysPressed, 0);
    bduk_register_c_lightfunc(ctx, "getPrevPress", native_getPrevPress, 0);
    bduk_register_c_lightfunc(ctx, "getSelPress", native_getSelPress, 0);
    bduk_register_c_lightfunc(ctx, "getEscPress", native_getEscPress, 0);
    bduk_register_c_lightfunc(ctx, "getNextPress", native_getNextPress, 0);
    bduk_register_c_lightfunc(ctx, "getAnyPress", native_getAnyPress, 0);
    return 0;
}

duk_ret_t native_keyboard(duk_context *ctx) {
    // usage: keyboard() : string
    // usage: keyboard(title: string) : string
    // usage: keyboard(title: string, maxlen: int) : string
    // usage: keyboard(title: string, maxlen: int, initval : string) : string
    // usage: keyboard(maxlen: int, initval: string) : string
    // returns: text typed by the user
    String result = "";
    if (!duk_is_string(ctx, 0)) {
        result = keyboard("");
    } else if (!duk_is_number(ctx, 1)) {
        result = keyboard(duk_to_string(ctx, 0));
    } else if (!duk_is_string(ctx, 2)) {
        result = keyboard(duk_to_string(ctx, 0), duk_to_int(ctx, 1));
    } else if (duk_is_number(ctx, 0) && duk_is_string(ctx, 1)) {
        result = keyboard("", duk_to_int(ctx, 1), duk_to_string(ctx, 2));
    } else {
        result = keyboard(duk_to_string(ctx, 0), duk_to_int(ctx, 1), duk_to_string(ctx, 2));
    }
    duk_push_string(ctx, result.c_str());
    return 1;
}

duk_ret_t native_getPrevPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(PrevPress)) || PrevPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getSelPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(SelPress)) || SelPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getEscPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(EscPress)) || EscPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getNextPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(NextPress)) || NextPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getAnyPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(AnyKeyPress)) || AnyKeyPress)
        duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}

duk_ret_t native_getKeysPressed(duk_context *ctx) {
    duk_push_array(ctx);
#ifdef HAS_KEYBOARD
    // Create a new array on the stack
    keyStroke key = _getKeyPress();
    if (!key.pressed) return 1; // if nothing has beed pressed, return 1
    int arrayIndex = 0;
    for (auto i : key.word) {
        char str[2] = {i, '\0'};
        duk_push_string(ctx, str);
        duk_put_prop_index(ctx, -2, arrayIndex);
        arrayIndex++;
    }
    if (key.del) {
        duk_push_string(ctx, "Delete");
        duk_put_prop_index(ctx, -2, arrayIndex);
        arrayIndex++;
    }
    if (key.enter) {
        duk_push_string(ctx, "Enter");
        duk_put_prop_index(ctx, -2, arrayIndex);
        arrayIndex++;
    }
    if (key.fn) {
        duk_push_string(ctx, "Function");
        duk_put_prop_index(ctx, -2, arrayIndex);
        arrayIndex++;
    }
    for (auto i : key.modifier_keys) {
        if (i == 0x82) {
            duk_push_string(ctx, "Alt");
            duk_put_prop_index(ctx, -2, arrayIndex);
            arrayIndex++;
        } else if (i == 0x2B) {
            duk_push_string(ctx, "Tab");
            duk_put_prop_index(ctx, -2, arrayIndex);
            arrayIndex++;
        } else if (i == 0x00) {
            duk_push_string(ctx, "Option");
            duk_put_prop_index(ctx, -2, arrayIndex);
            arrayIndex++;
        }
    }
#endif
    return 1;
}
#endif
