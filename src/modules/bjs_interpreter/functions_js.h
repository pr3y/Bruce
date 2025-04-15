#ifndef __FUNCTIONS_JS_H__
#define __FUNCTIONS_JS_H__

#include "helpers_js.h"
#include <duktape.h>

duk_ret_t native_noop(duk_context *ctx);

duk_ret_t native_serialPrint(duk_context *ctx);
duk_ret_t native_serialPrintln(duk_context *ctx);

duk_ret_t native_now(duk_context *ctx);
duk_ret_t native_delay(duk_context *ctx);
duk_ret_t native_assert(duk_context *ctx);
duk_ret_t native_random(duk_context *ctx);

// Hardware GPIO interactions
duk_ret_t native_digitalWrite(duk_context *ctx);
duk_ret_t native_analogWrite(duk_context *ctx);
duk_ret_t native_digitalRead(duk_context *ctx);
duk_ret_t native_analogRead(duk_context *ctx);
duk_ret_t native_touchRead(duk_context *ctx);
duk_ret_t native_dacWrite(duk_context *ctx);
duk_ret_t native_ledcSetup(duk_context *ctx);
duk_ret_t native_ledcAttachPin(duk_context *ctx);
duk_ret_t native_ledcWrite(duk_context *ctx);
duk_ret_t native_pinMode(duk_context *ctx);

duk_ret_t native_parse_int(duk_context *ctx);
duk_ret_t native_to_string(duk_context *ctx);
duk_ret_t native_to_hex_string(duk_context *ctx);
duk_ret_t native_to_lower_case(duk_context *ctx);
duk_ret_t native_to_upper_case(duk_context *ctx);

duk_ret_t native_math_acosh(duk_context *ctx);
duk_ret_t native_math_asinh(duk_context *ctx);
duk_ret_t native_math_atanh(duk_context *ctx);
duk_ret_t native_math_is_equal(duk_context *ctx);

/* 2FIX: not working
// terminate the script
duk_ret_t native_exit(duk_context *ctx);
*/

// Get information from the board;
duk_ret_t native_getBattery(duk_context *ctx);
duk_ret_t native_getDeviceName(duk_context *ctx);
duk_ret_t native_getBoard(duk_context *ctx);
duk_ret_t native_getFreeHeapSize(duk_context *ctx);

// Input functions
duk_ret_t native_getPrevPress(duk_context *ctx);
duk_ret_t native_getSelPress(duk_context *ctx);
duk_ret_t native_getEscPress(duk_context *ctx);
duk_ret_t native_getNextPress(duk_context *ctx);
duk_ret_t native_getAnyPress(duk_context *ctx);

duk_ret_t native_getKeysPressed(duk_context *ctx);

// Serial functions

duk_ret_t native_serialReadln(duk_context *ctx);

duk_ret_t native_serialCmd(duk_context *ctx);

duk_ret_t native_playAudioFile(duk_context *ctx);
duk_ret_t native_tone(duk_context *ctx);

duk_ret_t native_irTransmitFile(duk_context *ctx);

duk_ret_t native_subghzTransmitFile(duk_context *ctx);
duk_ret_t native_irTransmit(duk_context *ctx);
duk_ret_t native_subghzTransmit(duk_context *ctx);
duk_ret_t native_badusbRunFile(duk_context *ctx);

// badusb functions

duk_ret_t native_badusbSetup(duk_context *ctx);
// duk_ret_t native_badusbQuit(duk_context *ctx);
duk_ret_t native_badusbPrint(duk_context *ctx);
duk_ret_t native_badusbPrintln(duk_context *ctx);
duk_ret_t native_badusbPress(duk_context *ctx);
duk_ret_t native_badusbHold(duk_context *ctx);
duk_ret_t native_badusbRelease(duk_context *ctx);
duk_ret_t native_badusbReleaseAll(duk_context *ctx);
duk_ret_t native_badusbPressRaw(duk_context *ctx);
// duk_ret_t native_badusbPressSpecial(duk_context *ctx);

// IR functions

duk_ret_t native_irRead(duk_context *ctx);

// Subghz functions
duk_ret_t native_subghzRead(duk_context *ctx);
duk_ret_t native_subghzReadRaw(duk_context *ctx);
duk_ret_t native_subghzSetFrequency(duk_context *ctx);

// Keyboard
duk_ret_t native_keyboard(duk_context *ctx);

// Notification
duk_ret_t native_notifyBlink(duk_context *ctx);

// Storage functions
duk_ret_t native_storageReaddir(duk_context *ctx);
duk_ret_t native_storageRead(duk_context *ctx);
duk_ret_t native_storageWrite(duk_context *ctx);
duk_ret_t native_storageRename(duk_context *ctx);
duk_ret_t native_storageRemove(duk_context *ctx);
duk_ret_t native_storageMkdir(duk_context *ctx);
duk_ret_t native_storageRmdir(duk_context *ctx);

// Require
duk_ret_t native_require(duk_context *ctx);

#endif
