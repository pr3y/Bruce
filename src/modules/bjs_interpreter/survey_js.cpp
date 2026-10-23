#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "blindspot_wifi_watch.h"
#include "wifi_js.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <cstring>
#include <esp_heap_caps.h>
#include <esp_wifi.h>

namespace {
constexpr unsigned capacity = 48, frameLimit = 512;
struct Frame {
    uint16_t length;
    int8_t rssi;
    uint8_t channel;
    uint8_t data[frameLimit];
};
Frame *frames = nullptr;
unsigned count = 0, dropped = 0, deauth = 0;
BlindSpotWifiWatch watch;
bool pendingSurvey = false;
uint32_t surveyStart = 0;
int surveyDwell = 150;
uint8_t savedChannel = 1;
wifi_second_chan_t savedSecondary = WIFI_SECOND_CHAN_NONE;
wifi_promiscuous_filter_t savedFilter{};
Frame *surveyData = nullptr;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
void capture(void *buffer, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT) return;
    auto *p = static_cast<wifi_promiscuous_pkt_t *>(buffer);
    if (p->rx_ctrl.sig_len < 28) return;
    const unsigned length = p->rx_ctrl.sig_len - 4; // ESP-IDF includes FCS
    const uint8_t subtype = p->payload[0] & 0xfc;
    if (subtype != 0x80 && subtype != 0x50 && subtype != 0x40 && subtype != 0xc0 && subtype != 0xa0) return;
    portENTER_CRITICAL(&mux);
    if (frames) {
        if (subtype == 0xc0 || subtype == 0xa0) {
            ++deauth;
            watch.note(p->payload, length, p->rx_ctrl.channel, p->rx_ctrl.rssi, millis());
        } else {
            unsigned slot = count;
            for (unsigned i = 0; i < count; ++i) {
                if (frames[i].data[0] == p->payload[0] && !memcmp(frames[i].data + 10, p->payload + 10, 6)) {
                    slot = i;
                    break;
                }
            }
            if (slot < capacity) {
                if (slot == count) ++count;
                auto &f = frames[slot];
                f.length = length < frameLimit ? length : frameLimit;
                f.rssi = p->rx_ctrl.rssi;
                f.channel = p->rx_ctrl.channel;
                memcpy(f.data, p->payload, f.length);
            } else ++dropped;
        }
    }
    portEXIT_CRITICAL(&mux);
}
} // namespace

static void endCapture() {
    if (!pendingSurvey) return;
    esp_wifi_set_promiscuous(false);
    portENTER_CRITICAL(&mux);
    frames = nullptr;
    portEXIT_CRITICAL(&mux);
    esp_wifi_set_promiscuous_rx_cb(nullptr);
    esp_wifi_set_promiscuous_filter(&savedFilter);
    esp_wifi_set_channel(savedChannel, savedSecondary);
    pendingSurvey = false;
}

JSValue native_wifiSurveyStop(JSContext *, JSValue *, int, JSValue *) {
    endCapture();
    if (surveyData) {
        free(surveyData);
        surveyData = nullptr;
    }
    return JS_UNDEFINED;
}

JSValue native_wifiSurvey(JSContext *ctx, JSValue *, int argc, JSValue *argv) {
    int channel = 1, dwell = 150;
    if (argc > 0) JS_ToInt32(ctx, &channel, argv[0]);
    if (argc > 1) JS_ToInt32(ctx, &dwell, argv[1]);
    const bool asynchronous = argc > 2 && JS_ToBool(ctx, argv[2]);
    if (!pendingSurvey) {
        wifi_country_t country{};
        WiFi.mode(WIFI_STA);
        esp_wifi_get_country(&country);
        if (channel < country.schan || channel >= country.schan + country.nchan || dwell < 50 || dwell > 500)
            return JS_ThrowRangeError(ctx, "survey: invalid channel or dwell");
        bool active = false;
        esp_wifi_get_promiscuous(&active);
        if (active || WiFi.isConnected())
            return JS_ThrowInternalError(ctx, "survey requires disconnected, idle WiFi");
        esp_wifi_get_channel(&savedChannel, &savedSecondary);
        esp_wifi_get_promiscuous_filter(&savedFilter);
        surveyData = static_cast<Frame *>(
            heap_caps_malloc(sizeof(Frame) * capacity, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)
        );
        if (!surveyData) return JS_ThrowOutOfMemory(ctx);
        portENTER_CRITICAL(&mux);
        count = dropped = deauth = 0;
        frames = surveyData;
        portEXIT_CRITICAL(&mux);
        wifi_promiscuous_filter_t filter{WIFI_PROMIS_FILTER_MASK_MGMT};
        pendingSurvey = true;
        surveyStart = millis();
        surveyDwell = dwell;
        esp_err_t result = esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
        if (result == ESP_OK) result = esp_wifi_set_promiscuous_filter(&filter);
        if (result == ESP_OK) result = esp_wifi_set_promiscuous_rx_cb(capture);
        if (result == ESP_OK) result = esp_wifi_set_promiscuous(true);
        if (result != ESP_OK) {
            native_wifiSurveyStop(ctx, nullptr, 0, nullptr);
            return JS_ThrowInternalError(ctx, "survey setup failed: %d", result);
        }
    }
    if (asynchronous && millis() - surveyStart < (uint32_t)surveyDwell) return JS_UNDEFINED;
    if (!asynchronous) {
        while (millis() - surveyStart < (uint32_t)surveyDwell) delay(10);
    }
    endCapture();
    Frame *data = surveyData;
    surveyData = nullptr;
    unsigned n = count, lost = dropped, burst = deauth;
    JsonDocument security;
    JsonArray senders = security["senders"].to<JsonArray>();
    uint32_t now = millis();
    for (const auto &v : watch.senders) {
        if (!v.used || now - v.start > 3000 || now - v.last > 1000) continue;
        char mac[18];
        snprintf(
            mac,
            sizeof(mac),
            "%02X:%02X:%02X:%02X:%02X:%02X",
            v.mac[0],
            v.mac[1],
            v.mac[2],
            v.mac[3],
            v.mac[4],
            v.mac[5]
        );
        JsonObject item = senders.add<JsonObject>();
        item["mac"] = mac;
        item["channel"] = v.channel;
        item["count"] = v.total;
        item["broadcast"] = v.broadcast;
        item["reason"] = v.reason;
        item["rssi"] = v.rssi;
        item["windowMs"] = now - v.start;
    }
    String securityJson;
    serializeJson(security, securityJson);
    JSGCRef arrayRef;
    JS_PushGCRef(ctx, &arrayRef);
    arrayRef.val = JS_NewArray(ctx, 0);
    static const char digits[] = "0123456789abcdef";
    for (unsigned i = 0; i < n; ++i) {
        const auto &f = data[i];
        String hex;
        hex.reserve(f.length * 2);
        for (unsigned j = 0; j < f.length; ++j) {
            hex += digits[f.data[j] >> 4];
            hex += digits[f.data[j] & 15];
        }
        JSGCRef objectRef;
        JS_PushGCRef(ctx, &objectRef);
        objectRef.val = JS_NewObject(ctx);
        {
            JSValue value = JS_NewString(ctx, hex.c_str());
            JS_SetPropertyStr(ctx, objectRef.val, "payload", value);
        }
        {
            JSValue value = JS_NewInt32(ctx, f.rssi);
            JS_SetPropertyStr(ctx, objectRef.val, "rssi", value);
        }
        {
            JSValue value = JS_NewInt32(ctx, f.channel);
            JS_SetPropertyStr(ctx, objectRef.val, "channel", value);
        }
        JS_SetPropertyUint32(ctx, arrayRef.val, i, objectRef.val);
        JS_PopGCRef(ctx, &objectRef);
    }
    free(data);
    JSGCRef outRef;
    JS_PushGCRef(ctx, &outRef);
    outRef.val = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, outRef.val, "frames", arrayRef.val);
    {
        JSValue value = JS_NewString(ctx, securityJson.c_str());
        JS_SetPropertyStr(ctx, outRef.val, "security", value);
    }
    JS_SetPropertyStr(ctx, outRef.val, "dropped", JS_NewInt32(ctx, lost));
    JS_SetPropertyStr(ctx, outRef.val, "deauth", JS_NewInt32(ctx, burst));
    JSValue out = JS_PopGCRef(ctx, &outRef);
    JS_PopGCRef(ctx, &arrayRef);
    return out;
}
#endif
