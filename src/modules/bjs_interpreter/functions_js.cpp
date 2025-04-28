#include "functions_js.h"

#include "core/mykeyboard.h"
#include "modules/badusb_ble/ducky_typer.h"
#include "modules/ir/ir_read.h"
#include "modules/rf/rf_scan.h"
#include <chrono>

#include "display_js.h"
#include "gui_js.h"
#include "helpers_js.h"
#include "interpreter.h"
#include "wifi_js.h"

duk_ret_t native_noop(duk_context *ctx) { return 0; }

duk_ret_t native_serialPrint(duk_context *ctx) {
    internal_print(ctx, false, false);
    return 0;
}

duk_ret_t native_serialPrintln(duk_context *ctx) {
    internal_print(ctx, false, true);
    return 0;
}

duk_ret_t native_now(duk_context *ctx) {
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = duration_cast<milliseconds>(duration).count();
    duk_push_number(ctx, static_cast<double>(millis));
    return 1; // Return 1 value (the timestamp) to JavaScript
}

duk_ret_t native_delay(duk_context *ctx) {
    duk_push_global_object(ctx);
    duk_push_string(ctx, DUK_HIDDEN_SYMBOL("INTERPRETER_POINTER"));
    duk_get_prop(ctx, -2);
    InterpreterJS *interpreterJS = (InterpreterJS *)duk_get_pointer(ctx, -1);
    interpreterJS->_isExecuting = false;

    if (interpreterJS->shouldTerminate == false) {
        interpreterJS->terminate(true);
        return 0;
    }

    delay(duk_to_int(ctx, 0));
    return 0;
}

duk_ret_t native_sleep(duk_context *ctx) {
    duk_push_global_object(ctx);
    duk_push_string(ctx, DUK_HIDDEN_SYMBOL("INTERPRETER_POINTER"));
    duk_get_prop(ctx, -2);
    InterpreterJS *interpreterJS = (InterpreterJS *)duk_get_pointer(ctx, -1);
    interpreterJS->_isExecuting = false;

    if (interpreterJS->shouldTerminate == false) {
        interpreterJS->terminate(true);
        return 0;
    }

    duk_int_t delayMs = duk_to_int(ctx, 0);

    for (int i = 0; i < delayMs; i += 10) {
        delay(10);
        if (interpreterJS->isForeground) break;
    }
    return 0;
}

duk_ret_t native_assert(duk_context *ctx) {
    if (duk_get_boolean_default(ctx, 0, false)) {
        duk_push_boolean(ctx, true);
        return 1;
    }
    return duk_error(ctx, DUK_ERR_ERROR, "Assertion failed: %s", duk_get_string_default(ctx, 1, "assert"));
}

duk_ret_t native_random(duk_context *ctx) {
    int val;
    if (duk_is_number(ctx, 1)) {
        val = random(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
    } else {
        val = random(duk_to_int(ctx, 0));
    }
    duk_push_int(ctx, val);
    return 1;
}

// Hardware GPIO interactions
duk_ret_t native_digitalWrite(duk_context *ctx) {
    digitalWrite(duk_to_int(ctx, 0), duk_to_boolean(ctx, 1));
    return 0;
}

duk_ret_t native_analogWrite(duk_context *ctx) {
    analogWrite(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
    return 0;
}

duk_ret_t native_digitalRead(duk_context *ctx) {
    int val = digitalRead(duk_to_int(ctx, 0));
    duk_push_int(ctx, val);
    return 1;
}

duk_ret_t native_analogRead(duk_context *ctx) {
    int val = analogRead(duk_to_int(ctx, 0));
    duk_push_int(ctx, val);
    return 1;
}

duk_ret_t native_touchRead(duk_context *ctx) {
    int val = touchRead(duk_to_int(ctx, 0));
    duk_push_int(ctx, val);
    return 1;
}

duk_ret_t native_dacWrite(duk_context *ctx) {
#if defined(SOC_DAC_SUPPORTED)
    dacWrite(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
#else
    return duk_error(ctx, DUK_ERR_TYPE_ERROR, "%s function not supported on this device", "gpio.dacWrite()");
#endif
    return 0;
}

duk_ret_t native_ledcSetup(duk_context *ctx) {
    int val = ledcSetup(duk_get_int(ctx, 0), duk_get_int(ctx, 1), duk_get_int(ctx, 2));
    duk_push_int(ctx, val);
    return 1;
}

duk_ret_t native_ledcAttachPin(duk_context *ctx) {
    ledcAttachPin(duk_get_int(ctx, 0), duk_get_int(ctx, 1));
    return 0;
}

duk_ret_t native_ledcWrite(duk_context *ctx) {
    ledcWrite(duk_get_int(ctx, 0), duk_get_int(ctx, 1));
    return 0;
}

duk_ret_t native_pinMode(duk_context *ctx) {
    uint8_t pin = 255;
    uint8_t mode = INPUT;

    duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);
    if (arg0Type & DUK_TYPE_MASK_NUMBER) {
        pin = duk_to_int(ctx, 0);
    } else if (arg0Type & DUK_TYPE_MASK_STRING) {
        const char *pinString = duk_to_string(ctx, 0);
        if (pinString[0] == 'G') { pin = atoi(&pinString[1]); }
    }

    if (pin == 255) {
        return duk_error(
            ctx, DUK_ERR_TYPE_ERROR, "%s invalid %d argument: %s", "gpio.init()", 1, duk_to_string(ctx, 0)
        );
    }

    if (arg0Type & DUK_TYPE_MASK_NUMBER) {
        mode = duk_to_int(ctx, 0);
    } else if (arg0Type & DUK_TYPE_MASK_STRING) {
        String modeString = duk_to_string(ctx, 1);
        String pullModeString = duk_to_string(ctx, 2);

        if (modeString == "input" || modeString == "analog") {
            if (pullModeString == "up") {
                mode = INPUT_PULLUP;
            } else if (pullModeString == "down") {
                mode = INPUT_PULLDOWN;
            } else {
                mode = INPUT;
            }
        } else if (modeString.startsWith("output")) {
            mode = OUTPUT;
        }
    }

    pinMode(pin, mode);
    return 0;
}

duk_ret_t native_parse_int(duk_context *ctx) {
    duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);

    if (arg0Type & (DUK_TYPE_MASK_STRING | DUK_TYPE_MASK_NUMBER)) {
        duk_push_number(ctx, duk_to_number(ctx, 0));
    } else {
        duk_push_nan(ctx);
    }

    return 1;
}

duk_ret_t native_to_string(duk_context *ctx) {
    duk_push_string(ctx, duk_to_string(ctx, 0));

    return 1;
}

duk_ret_t native_to_hex_string(duk_context *ctx) {
    duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);

    if (arg0Type & (DUK_TYPE_MASK_STRING | DUK_TYPE_MASK_NUMBER | DUK_TYPE_MASK_BOOLEAN)) {
        duk_push_string(ctx, String(duk_to_int(ctx, 0), HEX).c_str());
    } else {
        duk_push_string(ctx, "");
    }

    return 1;
}

duk_ret_t native_to_lower_case(duk_context *ctx) {
    String text = duk_to_string(ctx, 0);
    text.toLowerCase();
    duk_push_string(ctx, text.c_str());

    return 1;
}

duk_ret_t native_to_upper_case(duk_context *ctx) {
    String text = duk_to_string(ctx, 0);
    text.toUpperCase();
    duk_push_string(ctx, text.c_str());

    return 1;
}

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
    duk_double_t a = duk_to_number(ctx, 0);
    duk_double_t b = duk_to_number(ctx, 0);
    duk_double_t epsilon = duk_to_number(ctx, 0);

    duk_push_number(ctx, fabs(a - b) < epsilon);

    return 1;
}

/* 2FIX: not working
// terminate the script
duk_ret_t native_exit(duk_context *ctx) {
  duk_error(ctx, DUK_ERR_ERROR, "Script exited");
  interpreter_start=false;
  return 0;
}
*/

// Get information from the board;
duk_ret_t native_getBattery(duk_context *ctx) {
    int bat = getBattery();
    duk_push_int(ctx, bat);
    return 1;
}

duk_ret_t native_getDeviceName(duk_context *ctx) {
    const char *deviceName = bruceConfig.wifiAp.ssid != NULL ? bruceConfig.wifiAp.ssid.c_str() : "Bruce";
    duk_push_string(ctx, deviceName);
    return 1;
}

duk_ret_t native_getBoard(duk_context *ctx) {
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

// Input functions
duk_ret_t native_getPrevPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(PrevPress)) || PrevPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getSelPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(SelPress)) || SelPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getEscPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(EscPress)) || EscPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getNextPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(NextPress)) || NextPress) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
duk_ret_t native_getAnyPress(duk_context *ctx) {
    if ((!duk_get_boolean_default(ctx, 0, 0) && check(AnyKeyPress)) || AnyKeyPress)
        duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}

duk_ret_t native_getKeysPressed(duk_context *ctx) {
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

duk_ret_t native_serialReadln(duk_context *ctx) {
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

duk_ret_t native_serialCmd(duk_context *ctx) {
    bool r = serialCli.parse(String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_playAudioFile(duk_context *ctx) {
    // usage: playAudioFile(filename : string);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in serialCli.parse)
    bool r = serialCli.parse("music_player " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_tone(duk_context *ctx) {
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

duk_ret_t native_irTransmitFile(duk_context *ctx) {
    // usage: irTransmitFile(filename : string);
    // returns: bool==true on success, false on any error
    bool r = serialCli.parse("ir tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_subghzTransmitFile(duk_context *ctx) {
    // usage: subghzTransmitFile(filename : string);
    // returns: bool==true on success, false on any error
    bool r = serialCli.parse("subghz tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_irTransmit(duk_context *ctx) {
    // usage: irTransmit(data: string, protocol : string = "NEC", bits: int = 32);
    // returns: bool==true on success, false on any error
    bool r = serialCli.parse(
        "IRSend {'Data':'" + String(duk_to_string(ctx, 0)) + "','Protocol':'" +
        String(duk_get_string_default(ctx, 1, "NEC")) +
        "','Bits':" + String(duk_get_uint_default(ctx, 2, 32)) + "}"
    );
    // TODO: ALT usage: irTransmit(protocol : string, address: int, command: int);
    // TODO: bool r = serialCli.parse("ir tx " + String(duk_to_string(ctx, 0)) + " " +
    // String(duk_get_uint_default(ctx, 1)) + " " + String(duk_to_string(ctx, 2)) + " " +
    // String(duk_to_string(ctx, 3)) );
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_subghzTransmit(duk_context *ctx) {
    // usage: subghzTransmit(data : string, frequency : int, te : int, count : int);
    // returns: bool==true on success, false on any error
    bool r = serialCli.parse(
        "subghz tx " + String(duk_to_string(ctx, 0)) + " " + String(duk_get_uint_default(ctx, 1, 433920000)) +
        " " + String(duk_get_uint_default(ctx, 2, 174)) + " " + String(duk_get_uint_default(ctx, 3, 10))
    );
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_badusbRunFile(duk_context *ctx) {
    // usage: badusbRunFile(filename : string);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in serialCli.parse)
    bool r = serialCli.parse("badusb tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

// badusb functions

duk_ret_t native_badusbSetup(duk_context *ctx) {
// usage: badusbSetup();
// returns: bool==true on success, false on any error
#if defined(USB_as_HID)

    if (hid_usb != nullptr) ducky_startKb(hid_usb, KeyboardLayout_en_US, false);
    duk_push_boolean(ctx, true);
#else
    duk_push_boolean(ctx, false);
#endif
    return 1;
}

/*
duk_ret_t native_badusbQuit(duk_context *ctx) {
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

duk_ret_t native_badusbPrint(duk_context *ctx) {
// usage: badusbPrint(msg : string);
#if defined(USB_as_HID)
    if (hid_usb != nullptr) hid_usb->print(duk_to_string(ctx, 0));
#endif
    return 0;
}

duk_ret_t native_badusbPrintln(duk_context *ctx) {
// usage: badusbPrintln(msg : string);
#if defined(USB_as_HID)
    if (hid_usb != nullptr) hid_usb->println(duk_to_string(ctx, 0));
#endif
    return 0;
}

duk_ret_t native_badusbPress(duk_context *ctx) {
// usage: badusbPress(keycode_number);
// keycodes list:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/src/USBHIDKeyboard.h
#if defined(USB_as_HID)
    if (hid_usb != nullptr) hid_usb->press(duk_to_int(ctx, 0));
    delay(1);
    if (hid_usb != nullptr) hid_usb->release(duk_to_int(ctx, 0));
#endif
    return 0;
}

duk_ret_t native_badusbHold(duk_context *ctx) {
// usage: badusbHold(keycode : number);
#if defined(USB_as_HID)
    if (hid_usb != nullptr) hid_usb->press(duk_to_int(ctx, 0));
#endif
    return 0;
}

duk_ret_t native_badusbRelease(duk_context *ctx) {
// usage: badusbHold(keycode : number);
#if defined(USB_as_HID)
    if (hid_usb != nullptr) hid_usb->release(duk_to_int(ctx, 0));
#endif
    return 0;
}

duk_ret_t native_badusbReleaseAll(duk_context *ctx) {
#if defined(USB_as_HID)
    if (hid_usb != nullptr) hid_usb->releaseAll();
#endif
    return 0;
}

duk_ret_t native_badusbPressRaw(duk_context *ctx) {
// usage: badusbPressRaw(keycode_number);
// keycodes list: TinyUSB's HID_KEY_* macros
// https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
#if defined(USB_as_HID)
    if (hid_usb != nullptr) hid_usb->pressRaw(duk_to_int(ctx, 0));
    delay(1);
    if (hid_usb != nullptr) hid_usb->releaseRaw(duk_to_int(ctx, 0));
#endif
    return 0;
}

/*
duk_ret_t native_badusbPressSpecial(duk_context *ctx) {
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

// IR functions

duk_ret_t native_irRead(duk_context *ctx) {
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

duk_ret_t native_subghzRead(duk_context *ctx) {
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

duk_ret_t native_subghzReadRaw(duk_context *ctx) {
    String r = "";
    if (duk_is_number(ctx, 0))
        r = RCSwitch_Read(bruceConfig.rfFreq, duk_to_int(ctx, 0),
                          true); // custom timeout
    else r = RCSwitch_Read(bruceConfig.rfFreq, 10, true);
    duk_push_string(ctx, r.c_str());
    return 1;
}

duk_ret_t native_subghzSetFrequency(duk_context *ctx) {
    // usage: subghzSetFrequency(freq_as_float);
    if (duk_is_number(ctx, 0)) bruceConfig.rfFreq = duk_to_number(ctx, 0); // float global var
    return 0;
}

// Keyboard
duk_ret_t native_keyboard(duk_context *ctx) {
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

// Storage functions
duk_ret_t native_storageReaddir(duk_context *ctx) {
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

duk_ret_t native_storageRead(duk_context *ctx) {
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

duk_ret_t native_storageWrite(duk_context *ctx) {
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

duk_ret_t native_storageRename(duk_context *ctx) {
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

duk_ret_t native_storageRemove(duk_context *ctx) {
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

duk_ret_t native_storageMkdir(duk_context *ctx) {
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

duk_ret_t native_storageRmdir(duk_context *ctx) {
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

duk_ret_t native_require(duk_context *ctx) {
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

    } else if (filepath == "dialog" || filepath == "gui") {
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
