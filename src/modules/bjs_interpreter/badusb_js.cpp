#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "badusb_js.h"
#include "modules/badusb_ble/ducky_typer.h"

#include "helpers_js.h"

// #include <USBHIDConsumerControl.h>  // used for badusbPressSpecial
// USBHIDConsumerControl cc;

duk_ret_t putPropBadUSBFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "setup", native_badusbSetup, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "press", native_badusbPress, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "hold", native_badusbHold, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "release", native_badusbRelease, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "releaseAll", native_badusbReleaseAll, 0, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "print", native_badusbPrint, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "println", native_badusbPrintln, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "pressRaw", native_badusbPressRaw, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "runFile", native_badusbRunFile, 1, magic);
    // bduk_put_prop_c_lightfunc(ctx, obj_idx, "badusbPressSpecial",
    // native_badusbPressSpecial, 1, 0);
    return 0;
}

duk_ret_t registerBadUSB(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "badusbSetup", native_badusbSetup, 0);
    bduk_register_c_lightfunc(ctx, "badusbPrint", native_badusbPrint, 1);
    bduk_register_c_lightfunc(ctx, "badusbPrintln", native_badusbPrintln, 1);
    bduk_register_c_lightfunc(ctx, "badusbPress", native_badusbPress, 1);
    bduk_register_c_lightfunc(ctx, "badusbHold", native_badusbHold, 1);
    bduk_register_c_lightfunc(ctx, "badusbRelease", native_badusbRelease, 1);
    bduk_register_c_lightfunc(ctx, "badusbReleaseAll", native_badusbReleaseAll, 0);
    bduk_register_c_lightfunc(ctx, "badusbPressRaw", native_badusbPressRaw, 1);
    bduk_register_c_lightfunc(ctx, "badusbRunFile", native_badusbRunFile, 1);
    // bduk_register_c_lightfunc(ctx, "badusbPressSpecial",
    // native_badusbPressSpecial, 1);
    return 0;
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

    if (hid_usb != nullptr) ducky_startKb(hid_usb, false);
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

#endif
