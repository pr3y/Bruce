#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "device_js.h"
#include <globals.h>

#include "helpers_js.h"

duk_ret_t putPropDeviceFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getName", native_getDeviceName, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getBoard", native_getBoard, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getModel", native_getBoard, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getBruceVersion", native_getBruceVersion, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getBatteryCharge", native_getBattery, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getBatteryDetailed", native_getBatteryDetailed, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getFreeHeapSize", native_getFreeHeapSize, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "getEEPROMSize", native_getEEPROMSize, 0, magic);
    return 0;
}

duk_ret_t registerDevice(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "getBoard", native_getBoard, 0);
    bduk_register_c_lightfunc(ctx, "getName", native_getDeviceName, 0);
    bduk_register_c_lightfunc(ctx, "getBruceVersion", native_getBruceVersion, 0);
    bduk_register_c_lightfunc(ctx, "getBattery", native_getBruceVersion, 0);
    bduk_register_c_lightfunc(ctx, "getBatteryDetailed", native_getBatteryDetailed, 0);
    bduk_register_c_lightfunc(ctx, "getFreeHeapSize", native_getFreeHeapSize, 0);
    bduk_register_c_lightfunc(ctx, "getEEPROMSize", native_getEEPROMSize, 0);
    return 0;
}

duk_ret_t native_getDeviceName(duk_context *ctx) {
    const char *deviceName = bruceConfig.wifiAp.ssid != NULL ? bruceConfig.wifiAp.ssid.c_str() : "Bruce";
    duk_push_string(ctx, deviceName);
    return 1;
}

duk_ret_t native_getBoard(duk_context *ctx) {
#if defined(DEVICE_NAME)
    String board = DEVICE_NAME;
#else
    String board = "Undefined";
#endif
    duk_push_string(ctx, board.c_str());
    return 1;
}

duk_ret_t native_getBruceVersion(duk_context *ctx) {
    duk_push_string(ctx, BRUCE_VERSION);
    return 1;
}

duk_ret_t native_getBattery(duk_context *ctx) {
    int bat = getBattery();
    duk_push_int(ctx, bat);
    return 1;
}

duk_ret_t native_getBatteryDetailed(duk_context *ctx) {
    duk_idx_t obj_idx = duk_push_object(ctx);
    bduk_put_prop(ctx, obj_idx, "battery_percent", duk_push_uint, getBattery());
#ifdef USE_BQ27220_VIA_I2C
    bduk_put_prop(ctx, obj_idx, "remaining_capacity", duk_push_uint, bq.getRemainCap());
    bduk_put_prop(ctx, obj_idx, "full_capacity", duk_push_uint, bq.getFullChargeCap());
    bduk_put_prop(ctx, obj_idx, "design_capacity", duk_push_uint, bq.getDesignCap());
    bduk_put_prop(ctx, obj_idx, "is_charging", duk_push_boolean, bq.getIsCharging());
    bduk_put_prop(
        ctx,
        obj_idx,
        "charging_voltage",
        duk_push_number,
        ((double)bq.getVolt(VOLT_MODE::VOLT_CHARGING) / 1000.0)
    );
    bduk_put_prop(ctx, obj_idx, "charging_current", duk_push_int, bq.getCurr(CURR_MODE::CURR_CHARGING));
    bduk_put_prop(ctx, obj_idx, "time_to_empty", duk_push_uint, bq.getTimeToEmpty());
    bduk_put_prop(ctx, obj_idx, "average_power_use", duk_push_uint, bq.getAvgPower());
    bduk_put_prop(ctx, obj_idx, "voltage", duk_push_number, ((double)bq.getVolt(VOLT_MODE::VOLT) / 1000.0));
    bduk_put_prop(ctx, obj_idx, "voltage_raw", duk_push_uint, bq.getVolt(VOLT_MODE::VOLT_RWA));
    bduk_put_prop(ctx, obj_idx, "current_instant", duk_push_int, bq.getCurr(CURR_INSTANT));
    bduk_put_prop(ctx, obj_idx, "current_average", duk_push_int, (bq.getCurr(CURR_MODE::CURR_AVERAGE)));
    bduk_put_prop(ctx, obj_idx, "current_raw", duk_push_int, bq.getVolt(VOLT_MODE::VOLT_RWA));
#endif

    return 1;
}

duk_ret_t native_getFreeHeapSize(duk_context *ctx) {
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    duk_idx_t obj_idx = duk_push_object(ctx);
    bduk_put_prop(ctx, obj_idx, "ram_free", duk_push_uint, info.total_free_bytes);
    bduk_put_prop(ctx, obj_idx, "ram_min_free", duk_push_uint, info.minimum_free_bytes);
    bduk_put_prop(ctx, obj_idx, "ram_largest_free_block", duk_push_uint, info.largest_free_block);
    bduk_put_prop(ctx, obj_idx, "ram_size", duk_push_uint, ESP.getHeapSize());
    bduk_put_prop(ctx, obj_idx, "psram_free", duk_push_uint, ESP.getFreePsram());
    bduk_put_prop(ctx, obj_idx, "psram_size", duk_push_uint, ESP.getPsramSize());

    return 1;
}

duk_ret_t native_getEEPROMSize(duk_context *ctx) {
    duk_push_int(ctx, EEPROMSIZE);
    return 1;
}

#endif
