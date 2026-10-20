#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "wifi_js.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <atomic>
#include <globals.h>
namespace {
std::atomic<bool> portalBusy{false}, portalAbort{false};
bool portalReady = false;
char portalSSID[33] = {}, portalResult[1024] = {};
uint8_t portalMAC[6] = {};
int portalChannel = 1;
void portalWorker(void *) {
    JsonDocument result;
    result["status"] = "inconclusive";
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, false);
    WiFi.begin(portalSSID, "", portalChannel, portalMAC);
    uint32_t start = millis();
    while (!portalAbort.load() && !WiFi.isConnected() && millis() - start < 15000)
        vTaskDelay(pdMS_TO_TICKS(50));
    if (portalAbort.load()) result["status"] = "cancelled";
    else if (!WiFi.isConnected()) result["detail"] = "Could not join selected open AP";
    else {
        WiFiClient client;
        HTTPClient http;
        http.setConnectTimeout(2500);
        http.setTimeout(2500);
        http.setReuse(false);
        http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
        const char *headers[] = {"Location", "Content-Type"};
        http.collectHeaders(headers, 2);
        if (http.begin(client, "http://connectivitycheck.gstatic.com/generate_204")) {
            int code = http.GET();
            result["httpStatus"] = code;
            String location = http.header("Location");
            result["location"] = location.substring(0, 192);
            if (code == 204) {
                result["status"] = "no_portal_observed";
                result["detail"] = "Connectivity probe returned expected HTTP 204";
            } else if (code >= 300 && code < 400 && location.length()) {
                result["status"] = "portal";
                result["detail"] = "Connectivity probe redirected; captive portal suspected";
            } else if (code == 200) {
                String sample;
                sample.reserve(2048);
                auto *stream = http.getStreamPtr();
                uint32_t began = millis();
                while (sample.length() < 2048 && !portalAbort.load() && millis() - began < 2000) {
                    if (stream->available()) {
                        int c = stream->read();
                        if (c >= 0) sample += (char)c;
                    } else if (!http.connected()) break;
                    else vTaskDelay(pdMS_TO_TICKS(10));
                }
                sample.toLowerCase();
                bool html = sample.indexOf("<html") >= 0 || sample.indexOf("<form") >= 0 ||
                            sample.indexOf("<!doctype html") >= 0;
                result["status"] = html ? "portal" : "inconclusive";
                result["detail"] = html ? "HTML replaced expected HTTP 204; captive portal suspected"
                                        : "Unexpected HTTP 200; not enough evidence";
                result["passwordForm"] = sample.indexOf("password") >= 0 && sample.indexOf("<form") >= 0;
            } else result["detail"] = "Probe failed or unexpected response; not a safety verdict";
            http.end();
        } else result["detail"] = "Could not start HTTP probe";
    }
    if (portalAbort.load()) result["status"] = "cancelled";
    WiFi.disconnect(false, false);
    wifiConnected = false;
    serializeJson(result, portalResult, sizeof(portalResult));
    portalBusy.store(false); // Publishes the completed result before poll reads it.
    vTaskDelete(nullptr);
}
} // namespace
JSValue native_wifiPortalStart(JSContext *ctx, JSValue *, int argc, JSValue *argv) {
    if (portalBusy.load()) return JS_ThrowInternalError(ctx, "Portal check already running");
    if (argc < 3) return JS_ThrowTypeError(ctx, "portalStart requires SSID, BSSID and channel");
    JSCStringBuf sb;
    const char *ssid = JS_ToCString(ctx, argv[0], &sb);
    if (!ssid || strlen(ssid) > 32) return JS_ThrowTypeError(ctx, "Invalid SSID");
    strlcpy(portalSSID, ssid, sizeof(portalSSID));
    const char *mac = JS_ToCString(ctx, argv[1], &sb);
    unsigned v[6];
    char extra;
    if (!mac ||
        sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x%c", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &extra) != 6)
        return JS_ThrowTypeError(ctx, "Invalid BSSID");
    for (int i = 0; i < 6; i++) portalMAC[i] = v[i];
    JS_ToInt32(ctx, &portalChannel, argv[2]);
    if (portalChannel < 1 || portalChannel > 13) return JS_ThrowRangeError(ctx, "Invalid channel");
    portalAbort = false;
    portalBusy = true;
    portalReady = true;
    portalResult[0] = 0;
    if (xTaskCreate(portalWorker, "portalCheck", 8192, nullptr, 1, nullptr) != pdPASS) {
        portalBusy = false;
        portalReady = false;
        return JS_ThrowInternalError(ctx, "Not enough RAM for portal check");
    }
    return JS_TRUE;
}
JSValue native_wifiPortalPoll(JSContext *ctx, JSValue *, int, JSValue *) {
    if (portalBusy.load() || !portalReady) return JS_UNDEFINED;
    portalReady = false;
    return JS_NewString(ctx, portalResult);
}
JSValue native_wifiPortalCancel(JSContext *, JSValue *, int, JSValue *) {
    portalAbort = true;
    while (portalBusy.load()) vTaskDelay(pdMS_TO_TICKS(20));
    portalReady = false;
    return JS_UNDEFINED;
}
#endif
