#include "interpreter.h"
#include "core/mykeyboard.h"
#include "core/sd_functions.h"
#include "core/serialcmds.h"
#include "modules/badusb_ble/bad_usb.h"
#include "modules/ir/ir_read.h"
#include "modules/rf/rf.h"

#include <quickjs.h>

// #include "dialog_js.h"
// #include "display_js.h"
// #include "helpers_js.h"
// #include "wifi_js.h"

// #define DUK_USE_DEBUG
// #define DUK_USE_DEBUG_LEVEL 2
// #define DUK_USE_DEBUG_WRITE

// #include <USBHIDConsumerControl.h>  // used for badusbPressSpecial
// USBHIDConsumerControl cc;

static char *script = NULL;
static char *scriptDirpath = NULL;
static char *scriptName = NULL;

// static JSValue native_noop(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) { return 0; }
/*
static duk_ret_t native_load(duk_context *ctx) {
    free((char *)script);
    free((char *)scriptDirpath);
    free((char *)scriptName);
    script = strdup(duk_to_string(ctx, 0));
    scriptDirpath = NULL;
    scriptName = NULL;
    return 0;
}

static duk_ret_t native_serialPrint(duk_context *ctx) {
    internal_print(ctx, false, false);
    return 0;
}

static duk_ret_t native_serialPrintln(duk_context *ctx) {
    internal_print(ctx, false, true);
    return 0;
}
*/
static JSValue native_now(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = duration_cast<milliseconds>(duration).count();
    return JS_NewInt64(ctx, millis);
}

static JSValue native_delay(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    if (argc < 1) { return JS_ThrowTypeError(ctx, "%s: requires 1 argument (ms)", "delay"); }

    int32_t delay_ms = 0;
    if (JS_ToInt32(ctx, &delay_ms, argv[0])) {
        return JS_ThrowTypeError(ctx, "%s: argument must be a number", "delay");
    }
    delay(delay_ms);
    return JS_UNDEFINED;
}

static JSValue native_assert(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    int condition = 0;
    if (argc > 0) { condition = JS_ToBool(ctx, argv[0]); }

    if (condition) { return JS_TRUE; }

    const char *message = "assert";
    if (argc > 1) {
        message = JS_ToCString(ctx, argv[1]);
        if (!message) message = "assert";
    }

    return JS_ThrowInternalError(ctx, "Assertion failed: %s", message);
}

static JSValue native_random(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    int32_t val;

    if (argc >= 2) {
        // Two arguments: random(min, max)
        int32_t min, max;
        JS_ToInt32(ctx, &min, argv[0]);
        JS_ToInt32(ctx, &max, argv[1]);
        val = random(min, max);
    } else if (argc == 1) {
        // One argument: random(max)
        int32_t max;
        JS_ToInt32(ctx, &max, argv[0]);
        val = random(max);
    } else {
        // No arguments: random()
        val = random();
    }

    return JS_NewInt32(ctx, val);
}

// Hardware GPIO interactions
static JSValue native_digitalWrite(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    uint32_t pin = 0;
    uint32_t val = 0;
    JS_ToUint32(ctx, &pin, argv[0]);
    JS_ToUint32(ctx, &val, argv[1]);
    digitalWrite(pin, !!val);
    return JS_UNDEFINED;
}

static JSValue native_analogWrite(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    uint32_t pin = 0;
    uint32_t val = 0;
    JS_ToUint32(ctx, &pin, argv[0]);
    JS_ToUint32(ctx, &val, argv[1]);
    analogWrite(pin, val);
    return JS_UNDEFINED;
}

static JSValue native_digitalRead(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    uint32_t pin = 0;
    JS_ToUint32(ctx, &pin, argv[0]);

    int val = digitalRead(pin);
    return JS_NewInt32(ctx, val);
}

static JSValue native_analogRead(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    uint32_t pin = 0;
    JS_ToUint32(ctx, &pin, argv[0]);

    int val = analogRead(pin);
    return JS_NewInt32(ctx, val);
}

static JSValue native_touchRead(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
    uint32_t pin = 0;
    JS_ToUint32(ctx, &pin, argv[0]);

    int val = touchRead(pin);
    return JS_NewInt32(ctx, val);
}

static JSValue native_dacWrite(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
#if defined(SOC_DAC_SUPPORTED)
    uint32_t pin = 0;
    uint32_t val = 0;
    JS_ToUint32(ctx, &pin, argv[0]);
    JS_ToUint32(ctx, &val, argv[1]);
    dacWrite(pin, val);
    return JS_UNDEFINED;
#else
    return duk_error(ctx, DUK_ERR_TYPE_ERROR, "%s function not supported on this device", "gpio.dacWrite()");
    return JS_ThrowTypeError(ctx, "%s function not supported on this device", "gpio.dacWrite()");
#endif
    return 0;
}

// static JSValue native_ledcSetup(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
//     uint32_t channel;
//     uint32_t freq;
//     uint32_t resolution_bits;
//     JS_ToUint32(ctx, &channel, argv[0]);
//     JS_ToUint32(ctx, &freq, argv[1]);
//     JS_ToUint32(ctx, &resolution_bits, argv[2]);

//     int val = ledcSetup(channel, freq, resolution_bits);

//     return JS_NewInt32(ctx, val);
// }

// static JSValue native_ledcAttachPin(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
//     uint32_t pin;
//     uint32_t channel;
//     JS_ToUint32(ctx, &pin, argv[0]);
//     JS_ToUint32(ctx, &channel, argv[1]);
//     ledcAttachPin(pin, channel);
//     return JS_UNDEFINED;
// }

// static JSValue native_ledcWrite(JSContext *ctx, JSValueConst jsThis, int argc, JSValueConst *argv) {
//     uint32_t channel;
//     uint32_t duty;
//     JS_ToUint32(ctx, &channel, argv[0]);
//     JS_ToUint32(ctx, &duty, argv[1]);
//     ledcWrite(channel, duty);
//     return JS_UNDEFINED;
// }

static JSValue native_pinMode(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    uint8_t pin = 255;
    uint8_t mode = INPUT;

    // Parse pin number
    if (JS_IsNumber(argv[0])) {
        JS_ToUint32(ctx, (uint32_t *)&pin, argv[0]);
    } else if (JS_IsString(argv[0])) {
        const char *pinString = JS_ToCString(ctx, argv[0]);
        if (pinString && pinString[0] == 'G') { pin = atoi(&pinString[1]); }
        JS_FreeCString(ctx, pinString);
    }

    if (pin == 255) { return JS_ThrowTypeError(ctx, "gpio.init() invalid argument"); }

    // Parse mode
    if (argc >= 2) {
        if (JS_IsNumber(argv[1])) {
            JS_ToUint32(ctx, (uint32_t *)&mode, argv[1]);
        } else if (JS_IsString(argv[1])) {
            const char *modeString = JS_ToCString(ctx, argv[1]);
            const char *pullModeString = argc >= 3 ? JS_ToCString(ctx, argv[2]) : NULL;

            if (strcmp(modeString, "input") == 0 || strcmp(modeString, "analog") == 0) {
                if (pullModeString && strcmp(pullModeString, "up") == 0) {
                    mode = INPUT_PULLUP;
                } else if (pullModeString && strcmp(pullModeString, "down") == 0) {
                    mode = INPUT_PULLDOWN;
                } else {
                    mode = INPUT;
                }
            } else if (strcmp(modeString, "output") == 0) {
                mode = OUTPUT;
            }

            JS_FreeCString(ctx, modeString);
            if (pullModeString) JS_FreeCString(ctx, pullModeString);
        }
    }

    pinMode(pin, mode);
    return JS_UNDEFINED;
}

static JSValue native_parse_int(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc < 1) return JS_NAN;

    if (JS_IsString(argv[0])) {
        const char *str = JS_ToCString(ctx, argv[0]);
        int32_t val = atoi(str);
        JS_FreeCString(ctx, str);
        return JS_NewInt32(ctx, val);
    } else if (JS_IsNumber(argv[0])) {
        int32_t val;
        JS_ToInt32(ctx, &val, argv[0]);
        return JS_NewInt32(ctx, val);
    }

    return JS_NAN;
}

static JSValue native_to_string(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc < 1) { return JS_NewString(ctx, "undefined"); }

    return JS_ToString(ctx, argv[0]);
}

static JSValue native_to_hex_string(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (JS_IsString(argv[0]) || JS_IsNumber(argv[0])) {
        return JS_NewString(ctx, String(JS_ToString(ctx, argv[0]), HEX).c_str());
    } else {
        return JS_NewString(ctx, "");
    }
}

static JSValue native_to_lower_case(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc < 1) { return JS_UNDEFINED; }

    const char *str = JS_ToCString(ctx, argv[0]);
    if (!str) return JS_UNDEFINED;

    String text(str);
    text.toLowerCase();
    JSValue result = JS_NewString(ctx, text.c_str());
    JS_FreeCString(ctx, str);

    return result;
}

static JSValue native_to_upper_case(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    if (argc < 1) { return JS_UNDEFINED; }

    const char *str = JS_ToCString(ctx, argv[0]);
    if (!str) return JS_UNDEFINED;

    String text(str);
    text.toUpperCase();
    JSValue result = JS_NewString(ctx, text.c_str());
    JS_FreeCString(ctx, str);

    return result;
}
/*TODOTODO
static duk_ret_t native_math_acosh(duk_context *ctx) {
    duk_double_t x = duk_to_number(ctx, 0);
    duk_push_number(ctx, acosh(x));

    return 1;
}

static duk_ret_t native_math_asinh(duk_context *ctx) {
    duk_double_t x = duk_to_number(ctx, 0);
    duk_push_number(ctx, asinh(x));

    return 1;
}

static duk_ret_t native_math_atanh(duk_context *ctx) {
    duk_double_t x = duk_to_number(ctx, 0);
    duk_push_number(ctx, atanh(x));

    return 1;
}

static duk_ret_t native_math_is_equal(duk_context *ctx) {
    duk_double_t a = duk_to_number(ctx, 0);
    duk_double_t b = duk_to_number(ctx, 0);
    duk_double_t epsilon = duk_to_number(ctx, 0);

    duk_push_number(ctx, fabs(a - b) < epsilon);

    return 1;
}

/* 2FIX: not working
// terminate the script
static duk_ret_t native_exit(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_ERROR, "Script exited");
  interpreter_start=false;
  return 0;
}
*/
/*TODOTODO
// Get information from the board;
static duk_ret_t native_getBattery(duk_context *ctx) {
    int bat = getBattery();
    duk_push_int(ctx, bat);
    return 1;
}

static duk_ret_t native_getDeviceName(duk_context *ctx) {
    const char *deviceName = bruceConfig.wifiAp.ssid != NULL ? bruceConfig.wifiAp.ssid.c_str() : "Bruce";
    duk_push_string(ctx, deviceName);
    return 1;
}

static duk_ret_t native_getBoard(duk_context *ctx) {
    String board = "Undefined";
#if defined(ARDUINO_M5STICK_C_PLUS)
    board = "StickCPlus";
#endif
#if defined(ARDUINO_M5STICK_C_PLUS2)
    board = "StickCPlus2";
#endif
#if defined(ARDUINO_M5STACK_CARDPUTER)
    board = "Cardputer";
#endif
#if defined(ARDUINO_M5STACK_CORE2)
    board = "Core2";
#endif
#if defined(ARDUINO_M5STACK_CORE)
    board = "Core";
#endif
#if defined(ARDUINO_M5STACK_CORES3)
    board = "CoreS3/SE";
#endif
    duk_push_string(ctx, board.c_str());
    return 1;
}

static duk_ret_t native_getFreeHeapSize(duk_context *ctx) {
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

// Input functions
static duk_ret_t native_getPrevPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(PrevPress)) || PrevPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getSelPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(SelPress)) || SelPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getEscPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(EscPress)) || EscPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getNextPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(NextPress)) || NextPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getAnyPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(AnyKeyPress)) || AnyKeyPress)
        duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}

static duk_ret_t native_getKeysPressed(duk_context *ctx) {
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

// Serial functions

static duk_ret_t native_serialReadln(duk_context *ctx) {
    // usage: serialReadln();   // default to 10s timeout
    // usage: serialReadln(timeout_in_ms : number);
    String line;
    int maxloops = 1000 * 10;
    if (duk_is_number(ctx, 0)) maxloops = duk_to_int(ctx, 0);
    Serial.flush();
    while (maxloops) {
        if (!Serial.available()) {
            maxloops -= 1;
            delay(1);
            continue;
        }
        // data is ready to read
        line = Serial.readStringUntil('\n');
    }
    duk_push_string(ctx, line.c_str());
    return 1;
}

static duk_ret_t native_serialCmd(duk_context *ctx) {
    bool r = serialCli.parse(String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_playAudioFile(duk_context *ctx) {
    // usage: playAudioFile(filename : string);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in serialCli.parse)
    bool r = serialCli.parse("music_player " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_tone(duk_context *ctx) {
    // usage: tone(frequency: number);
    // usage: tone(frequency: number, duration: number, nonBlocking: boolean);
    if (!bruceConfig.soundEnabled) return 0;

#if defined(BUZZ_PIN)
    tone(BUZZ_PIN, duk_get_uint_default(ctx, 0, 500), duk_get_uint_default(ctx, 1, 1000));
#elif defined(HAS_NS4168_SPKR)
    //  alt. implementation using the speaker
    if (!duk_get_int_default(ctx, 2, 0)) {
        serialCli.parse("tone " + String(duk_to_int(ctx, 0)) + " " + String(duk_to_int(ctx, 1)));
    }
#endif
    return 0;
}

static duk_ret_t native_irTransmitFile(duk_context *ctx) {
    // usage: irTransmitFile(filename : string);
    // returns: bool==true on success, false on any error
    bool r = serialCli.parse("ir tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_subghzTransmitFile(duk_context *ctx) {
    // usage: subghzTransmitFile(filename : string);
    // returns: bool==true on success, false on any error
    bool r = serialCli.parse("subghz tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_badusbRunFile(duk_context *ctx) {
    // usage: badusbRunFile(filename : string);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in serialCli.parse)
    bool r = serialCli.parse("badusb tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

// badusb functions

static duk_ret_t native_badusbSetup(duk_context *ctx) {
// usage: badusbSetup();
// returns: bool==true on success, false on any error
#if defined(USB_as_HID)
    Kb.begin();
    // cc.begin();
    USB.begin();
    duk_push_boolean(ctx, true);
#else
    duk_push_boolean(ctx, false);
#endif
    return 1;
}
*/
/*
static duk_ret_t native_badusbQuit(duk_context *ctx) {
  // usage: badusbQuit();
  // returns: quit keyboard mode, reinit serial port
  #if defined(USB_as_HID)
    Kb.end();
    //cc.begin();
    USB.~ESPUSB(); // Explicit call to destructor
    Serial.begin(115200);  // need to reinit serial when finished
    duk_push_boolean(ctx, true);
  #else
    duk_push_boolean(ctx, false);
  #endif
  return 1;
}
* */
/*TODOTODO
static duk_ret_t native_badusbPrint(duk_context *ctx) {
// usage: badusbPrint(msg : string);
#if defined(USB_as_HID)
    Kb.print(duk_to_string(ctx, 0));
#endif
    return 0;
}

static duk_ret_t native_badusbPrintln(duk_context *ctx) {
// usage: badusbPrintln(msg : string);
#if defined(USB_as_HID)
    Kb.println(duk_to_string(ctx, 0));
#endif
    return 0;
}

static duk_ret_t native_badusbPress(duk_context *ctx) {
// usage: badusbPress(keycode_number);
// keycodes list:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/src/USBHIDKeyboard.h
#if defined(USB_as_HID)
    Kb.press(duk_to_int(ctx, 0));
    delay(1);
    Kb.release(duk_to_int(ctx, 0));
#endif
    return 0;
}

static duk_ret_t native_badusbHold(duk_context *ctx) {
// usage: badusbHold(keycode : number);
#if defined(USB_as_HID)
    Kb.press(duk_to_int(ctx, 0));
#endif
    return 0;
}

static duk_ret_t native_badusbRelease(duk_context *ctx) {
// usage: badusbHold(keycode : number);
#if defined(USB_as_HID)
    Kb.release(duk_to_int(ctx, 0));
#endif
    return 0;
}

static duk_ret_t native_badusbReleaseAll(duk_context *ctx) {
#if defined(USB_as_HID)
    Kb.releaseAll();
#endif
    return 0;
}

static duk_ret_t native_badusbPressRaw(duk_context *ctx) {
// usage: badusbPressRaw(keycode_number);
// keycodes list: TinyUSB's HID_KEY_* macros
// https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
#if defined(USB_as_HID)
    Kb.pressRaw(duk_to_int(ctx, 0));
    delay(1);
    Kb.releaseRaw(duk_to_int(ctx, 0));
#endif
    return 0;
}

/*
static duk_ret_t native_badusbPressSpecial(duk_context *ctx) {
  // usage: badusbPressSpecial(keycode_number);
  // keycodes list:
https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/src/USBHIDConsumerControl.h
  #if defined(USB_as_HID)
    cc.press(duk_to_int(ctx, 0));
    delay(10);
    cc.release();
    //cc.end();
  #endif
  return 0;
}
*/

/*
// IR functions

static duk_ret_t native_irRead(duk_context *ctx) {
    // usage: irRead();
    // usage: irRead(timeout_in_seconds : number);
    // returns a string of the generated ir file, empty string on timeout or other
    // errors
    duk_int_t magic = duk_get_current_magic(ctx);
    IrRead irRead = IrRead(true, magic); // true == headless mode, true==raw mode
    String result = irRead.loop_headless(duk_get_int_default(ctx, 0, 10));
    duk_push_string(ctx, result.c_str());
    return 1;
}

// Subghz functions

static duk_ret_t native_subghzRead(duk_context *ctx) {
    // usage: subghzRead();
    // usage: subghzRead(timeout_in_seconds : number);
    // returns a string of the generated sub file, empty string on timeout or
    // other errors (decoding failed)
    String r = "";
    if (duk_is_number(ctx, 0)) r = RCSwitch_Read(bruceConfig.rfFreq, duk_to_int(ctx, 0)); // custom timeout
    else r = RCSwitch_Read(bruceConfig.rfFreq, 10);
    duk_push_string(ctx, r.c_str());
    return 1;
}

static duk_ret_t native_subghzReadRaw(duk_context *ctx) {
    String r = "";
    if (duk_is_number(ctx, 0))
        r = RCSwitch_Read(bruceConfig.rfFreq, duk_to_int(ctx, 0),
                          true); // custom timeout
    else r = RCSwitch_Read(bruceConfig.rfFreq, 10, true);
    duk_push_string(ctx, r.c_str());
    return 1;
}

static duk_ret_t native_subghzSetFrequency(duk_context *ctx) {
    // usage: subghzSetFrequency(freq_as_float);
    if (duk_is_number(ctx, 0)) bruceConfig.rfFreq = duk_to_number(ctx, 0); // float global var
    return 0;
}

static duk_ret_t native_keyboard(duk_context *ctx) {
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

// Notification
static duk_ret_t native_notifyBlink(duk_context *ctx) {
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

// Storage functions
static duk_ret_t native_storageReaddir(duk_context *ctx) {
    // usage: storageReaddir(path: string | Path, options?: { withFileTypes?:
    // false }): string[]
    // usage: storageReaddir(path: string | Path, options: {
    // withFileTypes: true }): { name: string, size: number, isDirectory: boolean
    // }[]

    // Extract path
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: Directory does not exist: %s", "storageReaddir", fileParams.path.c_str()
        );
    }

    // Extract options object (optional)
    bool withFileTypes = false;
    if (duk_is_object(ctx, 1)) {
        duk_get_prop_string(ctx, 1, "withFileTypes");
        withFileTypes = duk_get_boolean(ctx, -1);
    }

    // Open directory
    File dir = (fileParams.fs)->open(fileParams.path);
    if (!dir || !dir.isDirectory()) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: Not a directory: %s", "storageReaddir", fileParams.path.c_str()
        );
    }

    // Create result array
    duk_idx_t arr_idx = duk_push_array(ctx);
    int index = 0;

    while (true) {
        File entry = dir.openNextFile();
        if (!entry) break;

        // Get filename
        const char *name = entry.name();
        if (name[0] == '/') name++; // Remove leading '/' if needed

        if (withFileTypes) {
            // Return objects with name, size, and isDirectory
            duk_idx_t obj_idx = duk_push_object(ctx);
            duk_push_string(ctx, name);
            duk_put_prop_string(ctx, obj_idx, "name");

            duk_push_int(ctx, entry.size());
            duk_put_prop_string(ctx, obj_idx, "size");

            duk_push_boolean(ctx, entry.isDirectory());
            duk_put_prop_string(ctx, obj_idx, "isDirectory");

            duk_put_prop_index(ctx, arr_idx, index++);
        } else {
            // Return an array of filenames
            duk_push_string(ctx, name);
            duk_put_prop_index(ctx, arr_idx, index++);
        }

        entry.close();
    }

    dir.close();
    return 1; // Return array
}

static duk_ret_t native_storageRead(duk_context *ctx) {
    // usage: storageRead(path: string | Path, binary: boolean): string |
    // Uint8Array returns: file contents as a string. Empty string on any error.
    bool binary = duk_get_boolean_default(ctx, 1, false);
    size_t fileSize = 0;
    char *fileContent = NULL;
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: File: %s does not exist", "storageRead", fileParams.path.c_str()
        );
    }
    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path; // add "/" if missing

    // TODO: Change to use duk_push_fixed_buffer
    fileContent = readBigFile(*fileParams.fs, fileParams.path, binary, &fileSize);

    if (fileContent == NULL) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: Could not read file: %s", "storageRead", fileParams.path.c_str()
        );
    }

    if (binary && fileSize != 0) {
        void *buf = duk_push_fixed_buffer(ctx, fileSize);
        memcpy(buf, fileContent, fileSize);
        // Convert buffer to Uint8Array
        duk_push_buffer_object(ctx, -1, 0, fileSize, DUK_BUFOBJ_UINT8ARRAY);
    } else {
        duk_push_string(ctx, fileContent);
    }
    free(fileContent);
    return 1;
}

static duk_ret_t native_storageWrite(duk_context *ctx) {
    // usage: storageWrite(path: string | Path, data: string | Uint8Array, mode:
    // "write" | "append", position: number | string): boolean The write function
    // writes a string to a file, returning true if successful. Overwrites
    // existing file. The first parameter is the path of the file or object {fs:
    // string, path: string}. The second parameter is the contents to write

    duk_size_t dataSize;
    void *data = duk_to_buffer(ctx, 1, &dataSize);

    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path; // add "/" if missing

    const char *mode = FILE_APPEND; // default append
    const char *modeString = duk_get_string_default(ctx, 2, "a");
    if (modeString[0] == 'w') mode = FILE_WRITE;

    File file = (fileParams.fs)->open(fileParams.path, mode, true);
    if (!file) {
        duk_push_boolean(ctx, false);
        return 1;
    }

    // Check if position is provided
    if (duk_is_number(ctx, 3)) {
        // Get position as number
        int64_t pos = duk_get_int(ctx, 3);
        if (pos < 0) {
            // Negative index: seek from end
            file.seek(file.size() + pos, SeekSet);
        } else {
            file.seek(pos, SeekSet);
        }
    } else if (duk_is_string(ctx, 3)) {
        // Get position as string
        size_t fileSize = 0;
        char *fileContent = readBigFile(*fileParams.fs, fileParams.path, false, &fileSize);

        if (fileContent == NULL) {
            return duk_error(
                ctx, DUK_ERR_ERROR, "%s: Could not read file: %s", "storageWrite", fileParams.path.c_str()
            );
        }

        char *foundPos = strstr(fileContent, duk_get_string(ctx, 3));
        free(fileContent); // Free fileContent after usage

        if (foundPos) {
            file.seek(foundPos - fileContent, SeekSet);
        } else {
            file.seek(0, SeekEnd); // Append if string is not found
        }
    }

    // Write data
    file.write((const uint8_t *)data, dataSize);
    file.close();

    duk_push_boolean(ctx, true);

    return 1;
}

static duk_ret_t native_storageRename(duk_context *ctx) {
    // usage: storageRename(oldPath: string | Path, newPath: string): boolean
    FileParamsJS oldFileParams = js_get_path_from_params(ctx, true);
    String newPath = duk_get_string_default(ctx, 1, "");

    if (!oldFileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: File: %s does not exist", "storageRename", oldFileParams.path.c_str()
        );
    }

    if (!oldFileParams.path.startsWith("/")) oldFileParams.path = "/" + oldFileParams.path;
    if (!newPath.startsWith("/")) newPath = "/" + newPath;

    bool success = (oldFileParams.fs)->rename(oldFileParams.path, newPath);
    duk_push_boolean(ctx, success);
    return 1;
}

static duk_ret_t native_storageRemove(duk_context *ctx) {
    // usage: storageRemove(path: string | Path): boolean
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);
    if (!fileParams.exist) {
        return duk_error(
            ctx, DUK_ERR_ERROR, "%s: File: %s does not exist", "storageRemove", fileParams.path.c_str()
        );
    }

    if (!fileParams.path.startsWith("/")) { fileParams.path = "/" + fileParams.path; }

    bool success = (fileParams.fs)->remove(fileParams.path);
    duk_push_boolean(ctx, success);
    return 1;
}

static duk_ret_t native_storageMkdir(duk_context *ctx) {
    // usage: storageMkdir(path: string | Path): boolean
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);

    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path;

    String tempPath;
    bool success = true;

    // Create each part of the path
    // for (size_t i = 1; i < fileParams.path.length(); i++) {
    //   if (fileParams.path[i] == '/') {
    //     tempPath = fileParams.path.substring(0, i);
    //     if (!(fileParams.fs)->exists(tempPath)) {
    //       success = (fileParams.fs)->mkdir(tempPath);
    //       if (!success) break;
    //     }
    //   }
    // }

    // Create full directory if it does not exist
    if (success && !(fileParams.fs)->exists(fileParams.path)) {
        success = (fileParams.fs)->mkdir(fileParams.path);
    }

    duk_push_boolean(ctx, success);
    return 1;
}

static duk_ret_t native_storageRmdir(duk_context *ctx) {
    // usage: storageRmdir(path: string | Path): boolean
    FileParamsJS fileParams = js_get_path_from_params(ctx, true);

    if (!fileParams.path.startsWith("/")) fileParams.path = "/" + fileParams.path;

    // Ensure the directory exists before attempting to remove it
    if (!(fileParams.fs)->exists(fileParams.path)) {
        duk_push_boolean(ctx, false);
        return 1;
    }

    bool success = (fileParams.fs)->rmdir(fileParams.path);
    duk_push_boolean(ctx, success);
    return 1;
}

duk_ret_t native_drawStatusBar(duk_context *ctx) {
#if defined(HAS_SCREEN)
    drawStatusBar();
#endif
    return 0;
}
*/
/*
static duk_ret_t native_require(duk_context *ctx) {
    duk_idx_t obj_idx = duk_push_object(ctx);

    if (!duk_is_string(ctx, 0)) { return 1; }
    String filepath = duk_to_string(ctx, 0);

    if (filepath == "audio") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "playFile", native_playAudioFile, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "tone", native_tone, 3, 0);

    } else if (filepath == "badusb") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "setup", native_badusbSetup, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "press", native_badusbPress, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "hold", native_badusbHold, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "release", native_badusbRelease, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "releaseAll", native_badusbReleaseAll, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "print", native_badusbPrint, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "println", native_badusbPrintln, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "pressRaw", native_badusbPressRaw, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "runFile", native_badusbRunFile, 1, 0);
        // bduk_put_prop_c_lightfunc(ctx, obj_idx, "badusbPressSpecial",
        // native_badusbPressSpecial, 1, 0);

    } else if (filepath == "blebeacon") {

    } else if (filepath == "dialog") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "message", native_dialogMessage, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "info", native_dialogNotification, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "success", native_dialogNotification, 2, 1);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "warning", native_dialogNotification, 2, 2);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "error", native_dialogNotification, 2, 3);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "choice", native_dialogChoice, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "prompt", native_keyboard, 3, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "pickFile", native_dialogPickFile, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "viewFile", native_dialogViewFile, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "viewText", native_dialogViewText, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "createTextViewer", native_dialogCreateTextViewer, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "drawStatusBar", native_drawStatusBar, 0, 0);
    } else if (filepath == "display") {
        putPropDisplayFunctions(ctx, obj_idx, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "createSprite", native_createSprite, 2, 0);

    } else if (filepath == "device" || filepath == "flipper") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getName", native_getDeviceName, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getBoard", native_getBoard, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getModel", native_getBoard, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getBatteryCharge", native_getBattery, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getFreeHeapSize", native_getFreeHeapSize, 0, 0);

    } else if (filepath == "gpio") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "pinMode", native_pinMode, 3, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "digitalRead", native_digitalRead, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "analogRead", native_analogRead, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "touchRead", native_touchRead, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "digitalWrite", native_digitalWrite, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "analogWrite", native_analogWrite, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "dacWrite", native_dacWrite, 2,
                                    0); // only pins 25 and 26
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "ledcSetup", native_ledcSetup, 3, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "ledcAttachPin", native_ledcAttachPin, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "ledcWrite", native_ledcWrite, 2, 0);

    } else if (filepath == "http") {
        // TODO: Make the WebServer API compatible with the Node.js API
        // The more compatible we are, the more Node.js scripts can run on Bruce
        // MEMO: We need to implement an event loop so the WebServer can run:
        // https://github.com/svaarala/duktape/tree/master/examples/eventloop

    } else if (filepath == "ir") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "read", native_irRead, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "readRaw", native_irRead, 1, 1);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "transmitFile", native_irTransmitFile, 1, 0);
        // TODO: transmit(string)

    } else if (filepath == "keyboard" || filepath == "input") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "keyboard", native_keyboard, 3, 0);

        bduk_put_prop_c_lightfunc(
            ctx,
            obj_idx,
            "getKeysPressed",
            native_getKeysPressed,
            0,
            0
        ); // keyboard btns for cardputer (entry)
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getPrevPress", native_getPrevPress, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getSelPress", native_getSelPress, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getEscPress", native_getEscPress, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getNextPress", native_getNextPress, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "getAnyPress", native_getAnyPress, 1, 0);

    } else if (filepath == "math") {
        duk_pop(ctx);
        duk_push_global_object(ctx);
        duk_push_string(ctx, "Math");
        duk_get_prop(ctx, -2);
        duk_idx_t idx_top = duk_get_top_index(ctx);
        bduk_put_prop_c_lightfunc(ctx, idx_top, "acosh", native_math_acosh, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, idx_top, "asinh", native_math_asinh, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, idx_top, "atanh", native_math_atanh, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, idx_top, "is_equal", native_math_is_equal, 3, 0);

    } else if (filepath == "notification") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "blink", native_notifyBlink, 2, 0);

    } else if (filepath == "serial") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "print", native_serialPrint, DUK_VARARGS, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "println", native_serialPrintln, DUK_VARARGS, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "readln", native_serialReadln, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "cmd", native_serialCmd, 1, 0);

        bduk_put_prop_c_lightfunc(ctx, obj_idx, "write", native_serialPrint, DUK_VARARGS, 0);

    } else if (filepath == "storage") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "read", native_storageRead, 2, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "write", native_storageWrite, 4, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "rename", native_storageRename, 2);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "remove", native_storageRemove, 1);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "readdir", native_storageReaddir, 1);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "mkdir", native_storageMkdir, 1);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "rmdir", native_storageRmdir, 1);

    } else if (filepath == "subghz") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "setFrequency", native_subghzSetFrequency, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "read", native_subghzRead, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "readRaw", native_subghzReadRaw, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "transmitFile", native_subghzTransmitFile, 1, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "setup", native_noop, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "setIdle", native_noop, 0, 0);

    } else if (filepath == "wifi") {
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "connected", native_wifiConnected, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "connect", native_wifiConnect, 3, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "connectDialog", native_wifiConnectDialog, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "disconnect", native_wifiDisconnect, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "scan", native_wifiScan, 0, 0);
        bduk_put_prop_c_lightfunc(ctx, obj_idx, "httpFetch", native_httpFetch, 2, 0);

    } else {
        FS *fs = NULL;
        if (SD.exists(filepath)) fs = &SD;
        else if (LittleFS.exists(filepath)) fs = &LittleFS;
        if (fs == NULL) { return 1; }

        const char *requiredScript = readBigFile(*fs, filepath);
        if (requiredScript == NULL) { return 1; }

        duk_push_string(ctx, "(function(){exports={};module={exports:exports};\n");
        duk_push_string(ctx, requiredScript);
        duk_push_string(ctx, "\n})");
        duk_concat(ctx, 3);

        duk_int_t pcall_rc = duk_pcompile(ctx, DUK_COMPILE_EVAL);
        if (pcall_rc != DUK_EXEC_SUCCESS) { return 1; }

        pcall_rc = duk_pcall(ctx, 1);
        if (pcall_rc == DUK_EXEC_SUCCESS) {
            duk_get_prop_string(ctx, -1, "exports");
            duk_compact(ctx, -1);
        }
    }

    return 1;
}

static void registerConsole(duk_context *ctx) {
    duk_idx_t obj_idx = duk_push_object(ctx);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "log", native_serialPrintln, DUK_VARARGS);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "debug", native_serialPrintln, DUK_VARARGS, 2);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "warn", native_serialPrintln, DUK_VARARGS, 3);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "error", native_serialPrintln, DUK_VARARGS, 4);

    duk_put_global_string(ctx, "console");
}

static const char *nth_strchr(const char *s, char c, int8_t n) {
    const char *nth = s;
    if (c == '\0' || n < 1) return NULL;

    for (int i = 0; i < n; i++) {
        if ((nth = strchr(nth, c)) == 0) break;
        nth++;
    }

    return nth;
}

static void *ps_alloc_function(void *udata, duk_size_t size) {
    void *res;
    DUK_UNREF(udata);
    res = ps_malloc(size);
    return res;
}

static void *ps_realloc_function(void *udata, void *ptr, duk_size_t newsize) {
    void *res;
    DUK_UNREF(udata);
    res = ps_realloc(ptr, newsize);
    return res;
}

static void ps_free_function(void *udata, void *ptr) {
    DUK_UNREF(udata);
    DUK_ANSI_FREE(ptr);
}

static void js_fatal_error_handler(void *udata, const char *msg) {
    (void)udata;
    tft.setTextSize(FM);
    tft.setTextColor(TFT_RED, bruceConfig.bgColor);
    tft.drawCentreString("Error", tftWidth / 2, 10, 1);
    tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
    tft.setTextSize(FP);
    tft.setCursor(0, 33);

    tft.printf("JS FATAL ERROR: %s\n", (msg != NULL ? msg : "no message"));
    Serial.printf("JS FATAL ERROR: %s\n", (msg != NULL ? msg : "no message"));
    Serial.flush();

    delay(500);
    while (!check(AnyKeyPress));
    // We need to restart esp32 after fatal error
    abort();
}
*/
// Code interpreter, must be called in the loop() function to work
void interpreterHandler(void *pvParameters) {
    log_d(
        "init interpreter:\nPSRAM: [Free: %d, max alloc: %d],\nRAM: [Free: %d, "
        "max alloc: %d]\n",
        ESP.getFreePsram(),
        ESP.getMaxAllocPsram(),
        ESP.getFreeHeap(),
        ESP.getMaxAllocHeap()
    );
    if (script == NULL) { return; }

    tft.fillScreen(TFT_BLACK);
    tft.setRotation(bruceConfig.rotation);
    tft.setTextSize(FM);
    tft.setTextColor(TFT_WHITE);

    // Init containers
    // clearDisplayModuleData();

    // Create context.
    Serial.println("Create context");
    JSRuntime *rt = JS_NewRuntime();

    uint32_t memoryLimit = ESP.getFreeHeap() >> 1;

    JS_SetMemoryLimit(rt, memoryLimit);
    JS_SetGCThreshold(rt, memoryLimit >> 3);

    JSContext *ctx = JS_NewContext(rt);
    JSValue global = JS_GetGlobalObject(ctx);

    // Add native functions to context.
    JS_SetPropertyStr(ctx, global, "now", JS_NewCFunction(ctx, native_now, "now", 0));
    JS_SetPropertyStr(ctx, global, "delay", JS_NewCFunction(ctx, native_delay, "delay", 1));
    JS_SetPropertyStr(ctx, global, "parse_int", JS_NewCFunction(ctx, native_parse_int, "parse_int", 1));
    JS_SetPropertyStr(ctx, global, "to_string", JS_NewCFunction(ctx, native_to_string, "to_string", 1));
    JS_SetPropertyStr(
        ctx, global, "to_hex_string", JS_NewCFunction(ctx, native_to_hex_string, "to_hex_string", 1)
    );
    JS_SetPropertyStr(
        ctx, global, "to_lower_case", JS_NewCFunction(ctx, native_to_lower_case, "to_lower_case", 1)
    );
    JS_SetPropertyStr(
        ctx, global, "to_upper_case", JS_NewCFunction(ctx, native_to_upper_case, "to_upper_case", 1)
    );
    JS_SetPropertyStr(ctx, global, "random", JS_NewCFunction(ctx, native_random, "random", 2));
    // JS_SetPropertyStr(ctx, global, "require", JS_NewCFunction(ctx, native_require, "require", 1));
    JS_SetPropertyStr(ctx, global, "assert", JS_NewCFunction(ctx, native_assert, "assert", 2));
    if (scriptDirpath == NULL || scriptName == NULL) {
        JS_SetPropertyStr(ctx, global, "__filepath", JS_NewAtomString(ctx, ""));
        JS_SetPropertyStr(ctx, global, "__dirpath", JS_NewAtomString(ctx, ""));
    } else {
        // const char *filepath = (String(scriptDirpath) + String(scriptName)).c_str();
        // JS_SetPropertyStr(ctx, global, "__filepath", filepath);
        // const char *dirpath = scriptDirpath;
        // JS_SetPropertyStr(ctx, global, "__dirpath", dirpath);
    }
    JS_SetPropertyStr(ctx, global, "BRUCE_VERSION", JS_NewAtomString(ctx, BRUCE_VERSION));
    JS_SetPropertyStr(ctx, global, "BRUCE_PRICOLOR", bruceConfig.priColor);
    JS_SetPropertyStr(ctx, global, "BRUCE_SECCOLOR", bruceConfig.secColor);
    JS_SetPropertyStr(ctx, global, "BRUCE_BGCOLOR", bruceConfig.bgColor);

    // registerConsole(ctx);

    // Typescript emits: Object.defineProperty(exports, "__esModule", { value:
    // true }); In every file, this is polyfill so typescript project can run on
    // Bruce
    // duk_push_object(ctx);
    // duk_put_global_string(ctx, "exports");

    // Arduino compatible
    JS_SetPropertyStr(ctx, global, "pinMode", JS_NewCFunction(ctx, native_pinMode, "pinMode", 2));
    JS_SetPropertyStr(
        ctx, global, "digitalWrite", JS_NewCFunction(ctx, native_digitalWrite, "digitalWrite", 2)
    );
    JS_SetPropertyStr(ctx, global, "analogWrite", JS_NewCFunction(ctx, native_analogWrite, "analogWrite", 2));
    JS_SetPropertyStr(
        ctx, global, "dacWrite", JS_NewCFunction(ctx, native_dacWrite, "dacWrite", 2)
    ); // only pins 25 and 26
    JS_SetPropertyStr(ctx, global, "digitalRead", JS_NewCFunction(ctx, native_digitalRead, "digitalRead", 1));
    JS_SetPropertyStr(ctx, global, "analogRead", JS_NewCFunction(ctx, native_analogRead, "analogRead", 1));
    JS_SetPropertyStr(ctx, global, "touchRead", JS_NewCFunction(ctx, native_touchRead, "touchRead", 1));
    JS_SetPropertyStr(ctx, global, "HIGH", HIGH);
    JS_SetPropertyStr(ctx, global, "LOW", LOW);
    JS_SetPropertyStr(ctx, global, "INPUT", INPUT);
    JS_SetPropertyStr(ctx, global, "OUTPUT", OUTPUT);
    JS_SetPropertyStr(ctx, global, "PULLUP", PULLUP);
    JS_SetPropertyStr(ctx, global, "INPUT_PULLUP", INPUT_PULLUP);
    JS_SetPropertyStr(ctx, global, "PULLDOWN", PULLDOWN);
    JS_SetPropertyStr(ctx, global, "INPUT_PULLDOWN", INPUT_PULLDOWN);

    /*TODO TODO
    // Deprecated
    JS_SetPropertyStr(ctx, global, "load", JS_NewCFunction(ctx, native_load, "load", 1));

    // Get Informations from the board
    JS_SetPropertyStr(ctx, global, "getBattery", JS_NewCFunction(ctx, native_getBattery, "getBattery", 0));
    JS_SetPropertyStr(ctx, global, "getBoard", JS_NewCFunction(ctx, native_getBoard, "getBoard", 0));
    JS_SetPropertyStr(
        ctx, global, "getFreeHeapSize", JS_NewCFunction(ctx, native_getFreeHeapSize, "getFreeHeapSize", 0)
    );

    // Networking
    JS_SetPropertyStr(ctx, global, "wifiConnect", JS_NewCFunction(ctx, native_wifiConnect, "wifiConnect", 3));
    JS_SetPropertyStr(
        ctx,
        global,
        "wifiConnectDialog",
        JS_NewCFunction(ctx, native_wifiConnectDialog, "wifiConnectDialog", 0)
    );
    JS_SetPropertyStr(
        ctx, global, "wifiDisconnect", JS_NewCFunction(ctx, native_wifiDisconnect, "wifiDisconnect", 0)
    );
    JS_SetPropertyStr(ctx, global, "wifiScan", JS_NewCFunction(ctx, native_wifiScan, "wifiScan", 0));

    // bduk_register_c_lightfunc(ctx, "httpFetch", native_httpFetch, 2, 0);
    // bduk_register_c_lightfunc(ctx, "httpGet", native_httpFetch, 2, 0);

    // Graphics
    JS_SetPropertyStr(ctx, global, "color", JS_NewCFunction(ctx, native_color, "color", 4));
    JS_SetPropertyStr(ctx, global, "fillScreen", JS_NewCFunction(ctx, native_fillScreen, "fillScreen", 1));
    JS_SetPropertyStr(
        ctx, global, "setTextColor", JS_NewCFunction(ctx, native_setTextColor, "setTextColor", 1)
    );
    JS_SetPropertyStr(ctx, global, "setTextSize", JS_NewCFunction(ctx, native_setTextSize, "setTextSize", 1));
    JS_SetPropertyStr(ctx, global, "drawString", JS_NewCFunction(ctx, native_drawString, "drawString", 3));
    JS_SetPropertyStr(ctx, global, "setCursor", JS_NewCFunction(ctx, native_setCursor, "setCursor", 2));
    // bduk_register_c_lightfunc(ctx, "print", native_print, DUK_VARARGS);
    // bduk_register_c_lightfunc(ctx, "println", native_println, DUK_VARARGS);
    JS_SetPropertyStr(ctx, global, "drawPixel", JS_NewCFunction(ctx, native_drawPixel, "drawPixel", 3));
    JS_SetPropertyStr(ctx, global, "drawLine", JS_NewCFunction(ctx, native_drawLine, "drawLine", 5));
    JS_SetPropertyStr(ctx, global, "drawRect", JS_NewCFunction(ctx, native_drawRect, "drawRect", 5));
    JS_SetPropertyStr(
        ctx, global, "drawFillRect", JS_NewCFunction(ctx, native_drawFillRect, "drawFillRect", 5)
    );
    // JS_SetPropertyStr(ctx, global, "drawBitmap",
    //   JS_NewCFunction(ctx, native_drawBitmap, "drawBitmap", 4)
    // );
    JS_SetPropertyStr(ctx, global, "drawJpg", JS_NewCFunction(ctx, native_drawJpg, "drawJpg", 4));
    JS_SetPropertyStr(ctx, global, "drawGif", JS_NewCFunction(ctx, native_drawGif, "drawGif", 6));
    JS_SetPropertyStr(ctx, global, "gifOpen", JS_NewCFunction(ctx, native_gifOpen, "gifOpen", 2));
    JS_SetPropertyStr(ctx, global, "width", JS_NewCFunction(ctx, native_width, "width", 0));
    JS_SetPropertyStr(ctx, global, "height", JS_NewCFunction(ctx, native_height, "height", 0));

    // Input
    JS_SetPropertyStr(
        ctx, global, "getKeysPressed", JS_NewCFunction(ctx, native_getKeysPressed, "getKeysPressed", 0)
    );
    JS_SetPropertyStr(
        ctx, global, "getPrevPress", JS_NewCFunction(ctx, native_getPrevPress, "getPrevPress", 0)
    );
    JS_SetPropertyStr(ctx, global, "getSelPress", JS_NewCFunction(ctx, native_getSelPress, "getSelPress", 0));
    JS_SetPropertyStr(ctx, global, "getEscPress", JS_NewCFunction(ctx, native_getEscPress, "getEscPress", 0));
    JS_SetPropertyStr(
        ctx, global, "getNextPress", JS_NewCFunction(ctx, native_getNextPress, "getNextPress", 0)
    );
    JS_SetPropertyStr(ctx, global, "getAnyPress", JS_NewCFunction(ctx, native_getAnyPress, "getAnyPress", 0));

    // Serial
    JS_SetPropertyStr(
        ctx, global, "serialReadln", JS_NewCFunction(ctx, native_serialReadln, "serialReadln", 1)
    );
    // bduk_register_c_lightfunc(ctx, "serialPrintln", native_serialPrintln, DUK_VARARGS);
    JS_SetPropertyStr(ctx, global, "serialCmd", JS_NewCFunction(ctx, native_serialCmd, "serialCmd", 1));

    // Audio
    JS_SetPropertyStr(
        ctx, global, "playAudioFile", JS_NewCFunction(ctx, native_playAudioFile, "playAudioFile", 1)
    );
    JS_SetPropertyStr(ctx, global, "tone", JS_NewCFunction(ctx, native_tone, "tone", 3));

    // badusb
    JS_SetPropertyStr(ctx, global, "badusbSetup", JS_NewCFunction(ctx, native_badusbSetup, "badusbSetup", 0));
    JS_SetPropertyStr(ctx, global, "badusbPrint", JS_NewCFunction(ctx, native_badusbPrint, "badusbPrint", 1));
    JS_SetPropertyStr(
        ctx, global, "badusbPrintln", JS_NewCFunction(ctx, native_badusbPrintln, "badusbPrintln", 1)
    );
    JS_SetPropertyStr(ctx, global, "badusbPress", JS_NewCFunction(ctx, native_badusbPress, "badusbPress", 1));
    JS_SetPropertyStr(ctx, global, "badusbHold", JS_NewCFunction(ctx, native_badusbHold, "badusbHold", 1));
    JS_SetPropertyStr(
        ctx, global, "badusbRelease", JS_NewCFunction(ctx, native_badusbRelease, "badusbRelease", 1)
    );
    JS_SetPropertyStr(
        ctx, global, "badusbReleaseAll", JS_NewCFunction(ctx, native_badusbReleaseAll, "badusbReleaseAll", 0)
    );
    JS_SetPropertyStr(
        ctx, global, "badusbPressRaw", JS_NewCFunction(ctx, native_badusbPressRaw, "badusbPressRaw", 1)
    );
    JS_SetPropertyStr(
        ctx, global, "badusbRunFile", JS_NewCFunction(ctx, native_badusbRunFile, "badusbRunFile", 1)
    );
    // bduk_register_c_lightfunc(ctx, "badusbPressSpecial",
    // native_badusbPressSpecial, 1);

    // TODO: irTransmit(string)
    // IR
    JS_SetPropertyStr(ctx, global, "irRead", JS_NewCFunction(ctx, native_irRead, "irRead", 1));
    // bduk_register_c_lightfunc(ctx, "irReadRaw", native_irRead, 1, 1);
    JS_SetPropertyStr(
        ctx, global, "irTransmitFile", JS_NewCFunction(ctx, native_irTransmitFile, "irTransmitFile", 1)
    );
    // TODO: irTransmit(string)

    // subghz
    JS_SetPropertyStr(ctx, global, "subghzRead", JS_NewCFunction(ctx, native_subghzRead, "subghzRead", 0));
    JS_SetPropertyStr(
        ctx, global, "subghzReadRaw", JS_NewCFunction(ctx, native_subghzReadRaw, "subghzReadRaw", 0)
    );
    JS_SetPropertyStr(
        ctx,
        global,
        "subghzSetFrequency",
        JS_NewCFunction(ctx, native_subghzSetFrequency, "subghzSetFrequency", 1)
    );
    JS_SetPropertyStr(
        ctx,
        global,
        "subghzTransmitFile",
        JS_NewCFunction(ctx, native_subghzTransmitFile, "subghzTransmitFile", 1)
    );
    // JS_SetPropertyStr(ctx, global, "subghzSetIdle",
    //   JS_NewCFunction(ctx, native_subghzSetIdle, "subghzSetIdle", 1)
    // );
    // TODO: subghzTransmit(string)

    // Dialog functions
    // bduk_register_c_lightfunc(ctx, "dialogMessage", native_dialogNotification, 2, 0);
    // bduk_register_c_lightfunc(ctx, "dialogError", native_dialogNotification, 2, 3);
    // bduk_register_c_lightfunc(ctx, "dialogChoice", native_dialogChoice, 1, 1);
    JS_SetPropertyStr(
        ctx, global, "dialogPickFile", JS_NewCFunction(ctx, native_dialogPickFile, "dialogPickFile", 2)
    );
    JS_SetPropertyStr(
        ctx, global, "dialogViewFile", JS_NewCFunction(ctx, native_dialogViewFile, "dialogViewFile", 1)
    );
    JS_SetPropertyStr(ctx, global, "keyboard", JS_NewCFunction(ctx, native_keyboard, "keyboard", 3));

    // Storage
    JS_SetPropertyStr(
        ctx, global, "storageReaddir", JS_NewCFunction(ctx, native_storageReaddir, "storageReaddir", 1)
    );
    JS_SetPropertyStr(ctx, global, "storageRead", JS_NewCFunction(ctx, native_storageRead, "storageRead", 2));
    JS_SetPropertyStr(
        ctx, global, "storageWrite", JS_NewCFunction(ctx, native_storageWrite, "storageWrite", 4)
    );
    JS_SetPropertyStr(
        ctx, global, "storageRename", JS_NewCFunction(ctx, native_storageRename, "storageRename", 2)
    );
    JS_SetPropertyStr(
        ctx, global, "storageRemove", JS_NewCFunction(ctx, native_storageRemove, "storageRemove", 1)
    );
    TODO TODO*/

    JS_FreeValue(ctx, global);

    log_d(
        "global populated:\nPSRAM: [Free: %d, max alloc: %d],\nRAM: [Free: %d, "
        "max alloc: %d]\n",
        ESP.getFreePsram(),
        ESP.getMaxAllocPsram(),
        ESP.getFreeHeap(),
        ESP.getMaxAllocHeap()
    );

    // TODO: match flipper syntax
    // https://github.com/jamisonderek/flipper-zero-tutorials/wiki/JavaScript
    // MEMO: API https://duktape.org/api.html
    // https://github.com/joeqread/arduino-duktape/blob/main/src/duktape.h

    size_t scriptLength = strlen(script);

    Serial.printf("Script length: %d\n", scriptLength);

    JSValue result = JS_Eval(ctx, script, scriptLength, "eval.js", JS_EVAL_TYPE_GLOBAL);

    if (JS_IsException(result)) {
        tft.fillScreen(bruceConfig.bgColor);
        tft.setTextSize(FM);
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        tft.drawCentreString("Error", tftWidth / 2, 10, 1);
        tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
        tft.setTextSize(FP);
        tft.setCursor(0, 33);

        JSValue error = JS_GetException(ctx);
        const char *errorMessage = JS_ToCString(ctx, error);

        Serial.printf("eval failed: %s\n", errorMessage);
        tft.printf("%s\n\n", errorMessage);

        delay(500);
        while (!check(AnyKeyPress)) { delay(50); }
    } else {
        if (JS_IsString(result) || JS_IsNumber(result)) {
            const char *str = JS_ToCString(ctx, result);
            printf("Script ran successfully, result is: %s\n", str);
            JS_FreeCString(ctx, str);
        } else {
            printf("Script ran successfully\n");
        }
        JS_FreeValue(ctx, result);
    }
    free((char *)script);
    script = NULL;
    free((char *)scriptDirpath);
    scriptDirpath = NULL;
    free((char *)scriptName);
    scriptName = NULL;

    // Destroy runtime
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    // clearDisplayModuleData();

    // delay(1000);
    interpreter_start = false;
    vTaskDelete(NULL);
    return;
}

// function to start the JS Interpreterm choosinng the file, processing and
// start
void run_bjs_script() {
    String filename;
    FS *fs = &LittleFS;
    setupSdCard();
    if (sdcardMounted) {
        options = {
            {"SD Card",  [&]() { fs = &SD; }      },
            {"LittleFS", [&]() { fs = &LittleFS; }},
        };
        loopOptions(options);
    }
    filename = loopSD(*fs, true, "BJS|JS");
    script = readBigFile(*fs, filename);
    if (script == NULL) { return; }

    returnToMenu = true;
    interpreter_start = true;

    // To stop the script, press Prev and Next together for a few seconds
}

bool run_bjs_script_headless(char *code) {
    script = code;
    scriptDirpath = NULL;
    scriptName = NULL;
    returnToMenu = true;
    interpreter_start = true;
    return true;
}

bool run_bjs_script_headless(FS fs, String filename) {
    script = readBigFile(fs, filename);
    if (script == NULL) { return false; }
    const char *sName = filename.substring(0, filename.lastIndexOf('/')).c_str();
    const char *sDirpath = filename.substring(filename.lastIndexOf('/') + 1).c_str();
    scriptDirpath = strdup(sDirpath);
    scriptName = strdup(sName);
    returnToMenu = true;
    interpreter_start = true;
    return true;
}
