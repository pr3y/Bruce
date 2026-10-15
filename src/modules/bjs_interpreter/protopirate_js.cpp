#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "subghz_js.h"
#include <pp_engine.h>
#include "modules/rf/rf_utils.h"
#include "modules/rf/protocols/rf_presets.h"
#include "modules/rf/protocols/rf_encoder.h"
#include <sstream>
#include <cerrno>

static JSValue pp_result(JSContext *ctx, char *json) {
    if (!json) return JS_NewString(ctx, "{\"ok\":false,\"error\":\"Not enough memory\"}");
    JSValue result = JS_NewString(ctx, json);
    pp_response_free(json);
    return result;
}

JSValue native_subghzProtoInfo(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) {
    return pp_result(ctx, pp_catalog_json());
}

JSValue native_subghzProtoDecode(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) {
    if (argc < 1 || !JS_IsString(ctx, argv[0]))
        return JS_NewString(ctx, "{\"ok\":false,\"error\":\"Expected a SubGhz file string\"}");
    JSCStringBuf buffer;
    const char *input = JS_ToCString(ctx, argv[0], &buffer);
    if (!input) return JS_EXCEPTION;
    return pp_result(ctx, pp_decode_json(input));
}

JSValue native_subghzProtoEncode(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) {
    if (argc < 1 || !JS_IsString(ctx, argv[0]))
        return JS_NewString(ctx, "{\"ok\":false,\"error\":\"Expected a decoded SubGhz file string\"}");
    JSCStringBuf buffer;
    const char *input = JS_ToCString(ctx, argv[0], &buffer);
    if (!input) return JS_EXCEPTION;
    return pp_result(ctx, pp_encode_json(input));
}

// Complete RAW stream in one RMT transaction, including all RAW_Data lines.
// No generic protocol fallback, no per-line reinitialization, no unconditional success.
JSValue native_subghzProtoTransmit(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv) {
    if (argc < 1 || !JS_IsString(ctx, argv[0])) return JS_NewBool(false);
    JSCStringBuf buffer;
    const char *input = JS_ToCString(ctx, argv[0], &buffer);
    if (!input || strlen(input) > 262144) return JS_NewBool(false);
    std::istringstream file(input);
    std::string line, preset, protocol;
    uint32_t frequency = 0;
    std::vector<int> pulses;
    uint64_t total = 0;
    bool gotPreset = false, gotFrequency = false, gotProtocol = false;
    while (std::getline(file, line)) {
        const auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string key = line.substr(0, colon), value = line.substr(colon + 1);
        const auto first = value.find_first_not_of(" \t\r");
        value = first == std::string::npos ? "" : value.substr(first);
        const auto last = value.find_last_not_of(" \t\r");
        if (last != std::string::npos) value.resize(last + 1);
        if (key == "Frequency") {
            if (gotFrequency || value.empty() || value.find_first_not_of("0123456789") != std::string::npos) return JS_NewBool(false);
            errno = 0; char *end;
            unsigned long parsed = strtoul(value.c_str(), &end, 10);
            if (errno || *end || parsed > UINT32_MAX) return JS_NewBool(false);
            frequency = parsed; gotFrequency = true;
        } else if (key == "Preset") {
            if (gotPreset) return JS_NewBool(false);
            preset = value; gotPreset = true;
        } else if (key == "Protocol") {
            if (gotProtocol) return JS_NewBool(false);
            protocol = value; gotProtocol = true;
        } else if (key == "Bruce_Buffer_Full" && value != "0") return JS_NewBool(false);
        else if (key == "RAW_Data" || key == "Data_RAW") {
            std::istringstream values(value); std::string token;
            while (values >> token) {
                errno = 0; char *end; long v = strtol(token.c_str(), &end, 10);
                if (errno || *end || end == token.c_str() || !v || v < -1000000 || v > 1000000 || pulses.size() >= 16384) return JS_NewBool(false);
                total += v > 0 ? v : -v;
                if (total > 10000000) return JS_NewBool(false);
                pulses.push_back(int(v));
            }
        }
    }
    const RfPreset *rp = rf_find_preset(String(preset.c_str()));
    bool validFrequency = (frequency >= 300000000 && frequency <= 348000000) ||
        (frequency >= 387000000 && frequency <= 464000000) || (frequency >= 779000000 && frequency <= 928000000);
    if (!rp || !validFrequency || protocol != "RAW" || pulses.empty() || bruceConfigPins.rfModule != CC1101_SPI_MODULE) return JS_NewBool(false);
    if (!initRfModule("tx", frequency / 1000000.0f)) return JS_NewBool(false);
    ELECHOUSE_cc1101.setSidle();
    ELECHOUSE_cc1101.setModulation(rp->modulation);
    ELECHOUSE_cc1101.setDeviation(rp->deviation ? rp->deviation : 1.58f);
    ELECHOUSE_cc1101.setRxBW(rp->rxBW ? rp->rxBW : 270.83f);
    ELECHOUSE_cc1101.setDRate(rp->dataRate ? rp->dataRate : 10.f);
    ELECHOUSE_cc1101.setPktFormat(3);
    pinMode(bruceConfigPins.CC1101_bus.io0, OUTPUT);
    ELECHOUSE_cc1101.SetTx();
    bool success = rf_tx_durations(pulses);
    deinitRfModule();
    return JS_NewBool(success);
}
#endif
