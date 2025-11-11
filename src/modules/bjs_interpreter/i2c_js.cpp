#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "i2c_js.h"

#include "helpers_js.h"

duk_ret_t putPropI2CFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "begin", native_i2c_begin, 3, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "scan", native_i2c_scan, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "write", native_i2c_write, 3, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "read", native_i2c_read, 2, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "writeRead", native_i2c_write_read, 4, magic);
    return 0;
}

duk_ret_t registerI2C(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "begin", native_i2c_begin, 3);
    bduk_register_c_lightfunc(ctx, "scan", native_i2c_scan, 0);
    bduk_register_c_lightfunc(ctx, "write", native_i2c_write, 3);
    bduk_register_c_lightfunc(ctx, "read", native_i2c_read, 2);
    bduk_register_c_lightfunc(ctx, "writeRead", native_i2c_write_read, 4);
    return 0;
}

static void i2c_require_ready(duk_context *ctx) {
    duk_push_global_stash(ctx);
    duk_get_prop_string(
        ctx,
        -1,
        "\xff"
        "i2c_ready"
    );
    bool ready = duk_get_boolean_default(ctx, -1, false);
    duk_pop_2(ctx);
    if (!ready) { duk_error(ctx, DUK_ERR_ERROR, "i2c not initialized: call i2c.begin(sda,scl,hz) first"); }
}

duk_ret_t native_i2c_begin(duk_context *ctx) {
    // REQUIRE: begin(sda:int, scl:int, hz:int)
    if (!duk_is_number(ctx, 0) || !duk_is_number(ctx, 1) || !duk_is_number(ctx, 2)) {
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, "i2c.begin(sda:int, scl:int, hz:int) is required");
    }
    int sda = duk_require_int(ctx, 0);
    int scl = duk_require_int(ctx, 1);
    uint32_t hz = (uint32_t)duk_require_uint(ctx, 2);

    if (sda < 0 || scl < 0) return duk_error(ctx, DUK_ERR_RANGE_ERROR, "i2c.begin: pins must be >= 0");
    if (hz < 1000 || hz > 1000000)
        return duk_error(ctx, DUK_ERR_RANGE_ERROR, "i2c.begin: hz must be 1k..1MHz");

    Wire.begin(sda, scl, hz);
    Wire.setClock(hz);
    Wire.setTimeOut(50);

    // mark ready in context stash
    duk_push_global_stash(ctx);
    duk_push_boolean(ctx, true);
    duk_put_prop_string(
        ctx,
        -2,
        "\xff"
        "i2c_ready"
    );
    duk_pop(ctx);

    duk_push_true(ctx);
    return 1;
}

static bool duk_get_bytes_arg(duk_context *ctx, duk_idx_t idx, const uint8_t **p, duk_size_t *n) {
    if (duk_is_buffer_data(ctx, idx)) {
        *p = (const uint8_t *)duk_get_buffer_data(ctx, idx, n);
        return true;
    }
    if (duk_is_string(ctx, idx)) {
        *p = (const uint8_t *)duk_get_lstring(ctx, idx, n);
        return true;
    }
    return false;
}

duk_ret_t native_i2c_scan(duk_context *ctx) {
    i2c_require_ready(ctx);
    duk_idx_t arr = duk_push_array(ctx);
    int idx = 0;
    for (uint8_t a = 1; a < 127; a++) {
        Wire.beginTransmission(a);
        if (Wire.endTransmission(true) == 0) {
            duk_push_int(ctx, a);
            duk_put_prop_index(ctx, arr, idx++);
        }
    }
    return 1;
}

duk_ret_t native_i2c_write(duk_context *ctx) {
    // write(addr:int, data:Uint8Array|string, [sendStop:boolean=true]) -> err(0=ok)
    i2c_require_ready(ctx);
    int addr = duk_require_int(ctx, 0);
    const uint8_t *buf;
    duk_size_t len;
    if (!duk_get_bytes_arg(ctx, 1, &buf, &len))
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, "i2c.write: arg1 must be Uint8Array or string");
    bool sendStop = duk_get_boolean_default(ctx, 2, true);

    Wire.beginTransmission((uint8_t)addr);
    Wire.write(buf, (size_t)len);
    uint8_t err = Wire.endTransmission(sendStop);
    duk_push_int(ctx, err);
    return 1;
}

duk_ret_t native_i2c_read(duk_context *ctx) {
    // read(addr:int, len:int) -> Uint8Array
    i2c_require_ready(ctx);
    int addr = duk_require_int(ctx, 0);
    int len = duk_require_int(ctx, 1);
    size_t got = Wire.requestFrom((uint8_t)addr, (uint8_t)len, (uint8_t)true);
    void *buf = duk_push_fixed_buffer(ctx, got);
    for (size_t i = 0; i < got && Wire.available(); i++) ((uint8_t *)buf)[i] = Wire.read();
    duk_push_buffer_object(ctx, -1, 0, got, DUK_BUFOBJ_UINT8ARRAY);
    return 1;
}

duk_ret_t native_i2c_write_read(duk_context *ctx) {
    // writeRead(addr:int, wbuf:Uint8Array|string, rlen:int, [delayMs:int=0]) -> Uint8Array
    i2c_require_ready(ctx);
    int addr = duk_require_int(ctx, 0);
    const uint8_t *wbuf;
    duk_size_t wlen;
    if (!duk_get_bytes_arg(ctx, 1, &wbuf, &wlen))
        return duk_error(ctx, DUK_ERR_TYPE_ERROR, "i2c.writeRead: arg1 must be Uint8Array or string");
    int rlen = duk_require_int(ctx, 2);
    int wait = duk_get_int_default(ctx, 3, 0);

    Wire.beginTransmission((uint8_t)addr);
    Wire.write(wbuf, (size_t)wlen);
    uint8_t err = Wire.endTransmission(false); // repeated START
    if (err != 0) {
        duk_push_int(ctx, -err);
        return 1;
    }

    if (wait > 0) delay(wait);

    size_t got = Wire.requestFrom((uint8_t)addr, (uint8_t)rlen, (uint8_t)true);
    void *buf = duk_push_fixed_buffer(ctx, got);
    for (size_t i = 0; i < got && Wire.available(); i++) ((uint8_t *)buf)[i] = Wire.read();
    duk_push_buffer_object(ctx, -1, 0, got, DUK_BUFOBJ_UINT8ARRAY);
    return 1;
}

#endif
