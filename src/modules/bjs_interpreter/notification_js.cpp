#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "notification_js.h"

#include "helpers_js.h"

duk_ret_t putPropNotificationFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "blink", native_notifyBlink, 2, magic);
    return 0;
}

duk_ret_t registerNotification(duk_context *ctx) { return 0; }

duk_ret_t native_notifyBlink(duk_context *ctx) {
    duk_uint_t arg1Type = duk_get_type_mask(ctx, 1);
    uint32_t delayMs = 500;

    uint8_t delayMsArg = (arg1Type & (DUK_TYPE_NONE | DUK_TYPE_UNDEFINED)) ? 0 : 1;

    if (arg1Type & DUK_TYPE_MASK_NUMBER) {
        delayMs = duk_to_int(ctx, delayMsArg);
    } else if (arg1Type & DUK_TYPE_MASK_STRING) {
        String delayMsString = duk_to_string(ctx, delayMsArg);
        delayMs = ((delayMsString == "long") ? 1000 : 500);
    }

    digitalWrite(19, HIGH);
    delay(delayMs);
    digitalWrite(19, LOW);

    return 0;
}

#endif
