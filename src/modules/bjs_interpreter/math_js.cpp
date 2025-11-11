#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "math_js.h"

#include "helpers_js.h"

duk_ret_t putPropMathFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    duk_pop(ctx);
    duk_push_global_object(ctx);
    duk_push_string(ctx, "Math");
    duk_get_prop(ctx, -2);
    duk_idx_t idx_top = duk_get_top_index(ctx);
    bduk_put_prop_c_lightfunc(ctx, idx_top, "acosh", native_math_acosh, 1, 0);
    bduk_put_prop_c_lightfunc(ctx, idx_top, "asinh", native_math_asinh, 1, 0);
    bduk_put_prop_c_lightfunc(ctx, idx_top, "atanh", native_math_atanh, 1, 0);
    bduk_put_prop_c_lightfunc(ctx, idx_top, "is_equal", native_math_is_equal, 3, 0);
    return 0;
}

duk_ret_t registerMath(duk_context *ctx) { return 0; }

duk_ret_t native_math_acosh(duk_context *ctx) {
    duk_double_t x = duk_to_number(ctx, 0);
    duk_push_number(ctx, acosh(x));

    return 1;
}

duk_ret_t native_math_asinh(duk_context *ctx) {
    duk_double_t x = duk_to_number(ctx, 0);
    duk_push_number(ctx, asinh(x));

    return 1;
}

duk_ret_t native_math_atanh(duk_context *ctx) {
    duk_double_t x = duk_to_number(ctx, 0);
    duk_push_number(ctx, atanh(x));

    return 1;
}

duk_ret_t native_math_is_equal(duk_context *ctx) {
    // TODO: Check if the original here had a mistake, corrected version below
    // duk_double_t a = duk_to_number(ctx, 0);
    // duk_double_t b = duk_to_number(ctx, 0);
    // duk_double_t epsilon = duk_to_number(ctx, 0);

    // duk_push_number(ctx, fabs(a - b) < epsilon);

    duk_double_t a = duk_to_number(ctx, 0);
    duk_double_t b = duk_to_number(ctx, 1);
    duk_double_t epsilon = duk_to_number(ctx, 2);

    duk_push_boolean(ctx, fabs(a - b) < epsilon);

    return 1;
}

#endif
