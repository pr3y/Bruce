#ifndef __DISPLAY_JS_H__
#define __DISPLAY_JS_H__
#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)

#include "core/display.h"
#include <duktape.h>

void clearDisplayModuleData();

inline void internal_print(duk_context *ctx, uint8_t printTft, uint8_t newLine)
    __attribute__((always_inline));

duk_ret_t putPropDisplayFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic = 0);
duk_ret_t registerDisplay(duk_context *ctx);

duk_ret_t native_color(duk_context *ctx);
duk_ret_t native_setTextColor(duk_context *ctx);
duk_ret_t native_setTextSize(duk_context *ctx);
duk_ret_t native_setTextAlign(duk_context *ctx);
duk_ret_t native_drawRect(duk_context *ctx);
duk_ret_t native_drawFillRect(duk_context *ctx);
duk_ret_t native_drawFillRectGradient(duk_context *ctx);
duk_ret_t native_drawRoundRect(duk_context *ctx);
duk_ret_t native_drawFillRoundRect(duk_context *ctx);
duk_ret_t native_drawCircle(duk_context *ctx);
duk_ret_t native_drawFillCircle(duk_context *ctx);
duk_ret_t native_drawLine(duk_context *ctx);
duk_ret_t native_drawPixel(duk_context *ctx);
duk_ret_t native_drawXBitmap(duk_context *ctx);
duk_ret_t native_drawString(duk_context *ctx);
duk_ret_t native_setCursor(duk_context *ctx);
duk_ret_t native_print(duk_context *ctx);
duk_ret_t native_println(duk_context *ctx);
duk_ret_t native_fillScreen(duk_context *ctx);
duk_ret_t native_width(duk_context *ctx);
duk_ret_t native_height(duk_context *ctx);
duk_ret_t native_drawImage(duk_context *ctx);
duk_ret_t native_drawJpg(duk_context *ctx);
duk_ret_t native_drawGif(duk_context *ctx);
duk_ret_t native_gifPlayFrame(duk_context *ctx);
duk_ret_t native_gifDimensions(duk_context *ctx);
duk_ret_t native_gifReset(duk_context *ctx);
duk_ret_t native_gifClose(duk_context *ctx);
duk_ret_t native_gifOpen(duk_context *ctx);
duk_ret_t native_deleteSprite(duk_context *ctx);
duk_ret_t native_pushSprite(duk_context *ctx);
duk_ret_t native_createSprite(duk_context *ctx);

inline void internal_print(duk_context *ctx, uint8_t printTft, uint8_t newLine) {
    duk_int_t magic = duk_get_current_magic(ctx);

    if (magic != 0) {
        // Print if console.debug, console.warn or console.error
        if (magic == 2) {
            Serial.print("[D] ");
        } else if (magic == 3) {
            Serial.print("[W] ");
        } else if (magic == 4) {
            Serial.print("[E] ");
        }
    }

    for (duk_idx_t argIndex = 0; argIndex < 20; argIndex++) {
        duk_uint_t argType = duk_get_type_mask(ctx, argIndex);
        if (argType & DUK_TYPE_MASK_NONE) { break; }
        if (argIndex > 0) {
            if (printTft) tft.print(" ");
            Serial.print(" ");
        }

        if (argType & DUK_TYPE_MASK_UNDEFINED) {
            if (printTft) tft.print("undefined");
            Serial.print("undefined");

        } else if (argType & DUK_TYPE_MASK_NULL) {
            if (printTft) tft.print("null");
            Serial.print("null");

        } else if (argType & DUK_TYPE_MASK_NUMBER) {
            duk_double_t numberValue = duk_to_number(ctx, argIndex);
            if (printTft) tft.printf("%g", numberValue);
            Serial.printf("%g", numberValue);

        } else if (argType & DUK_TYPE_MASK_BOOLEAN) {
            const char *boolValue = duk_to_int(ctx, argIndex) ? "true" : "false";
            if (printTft) tft.print(boolValue);
            Serial.print(boolValue);

        } else {
            const char *stringValue = duk_to_string(ctx, argIndex);
            if (printTft) tft.print(stringValue);
            Serial.print(stringValue);
        }
    }
    if (newLine) {
        if (printTft) tft.println();
        Serial.println();
    }
}

duk_ret_t native_getRotation(duk_context *ctx);
duk_ret_t native_getBrightness(duk_context *ctx);
duk_ret_t native_setBrightness(duk_context *ctx);
duk_ret_t native_restoreBrightness(duk_context *ctx);

#endif
#endif
