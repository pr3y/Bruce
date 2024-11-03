#include "interpreter.h"
#include "core/sd_functions.h"
#include "core/wifi_common.h"
#include "core/mykeyboard.h"
#include "core/serialcmds.h"
#include "core/display.h"
#include "modules/rf/rf.h"
#include "modules/ir/ir_read.h"
#include "modules/others/bad_usb.h"

//#include <USBHIDConsumerControl.h>  // used for badusbPressSpecial
//USBHIDConsumerControl cc;

String headers[20];
String script = "drawString('Something wrong.', 4, 4);";
HTTPClient http;

#ifdef CARDPUTER
    Keyboard_Class kb;
#endif


static duk_ret_t native_load(duk_context *ctx) {
  script = duk_to_string(ctx, 0);
  return 0;
}

static duk_ret_t native_print(duk_context *ctx) {
  Serial.println(duk_to_string(ctx, 0));
  return 0;
}

static duk_ret_t native_now(duk_context *ctx) {
    using namespace std::chrono;
    auto now = high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = duration_cast<milliseconds>(duration).count();
    duk_push_number(ctx, static_cast<double>(millis));
    return 1; // Return 1 value (the timestamp) to JavaScript
}

static duk_ret_t native_delay(duk_context *ctx) {
  delay(duk_to_int(ctx, 0));
  return 0;
}

// Hardware GPIO interactions
static duk_ret_t native_digitalWrite(duk_context *ctx) {
  digitalWrite(duk_to_int(ctx, 0), duk_to_boolean(ctx, 1));
  return 0;
}

static duk_ret_t native_pinMode(duk_context *ctx) {
  pinMode(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
  return 0;
}
// Get information from the board;
static duk_ret_t native_getBattery(duk_context *ctx) {
    int bat = getBattery();
    duk_push_int(ctx, bat);
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

static duk_ret_t native_getBoard(duk_context *ctx) {
    String board = "Undefined";
#if defined(STICK_C_PLUS)
    board = "StickCPlus";
#endif
#if defined(STICK_C_PLUS2)
    board = "StickCPlus2";
#endif
#if defined(CARDPUTER)
    board = "Cardputer";
#endif
#if defined(CORE2)
    board = "Core2";
#endif
#if defined(CORE)
    board = "Core";
#endif
#if defined(CORES3)
    board = "CoreS3/SE";
#endif
    duk_push_string(ctx, board.c_str());
    return 1;
}

// Wifi Functions
static duk_ret_t native_wifiConnectDialog(duk_context *ctx) {
    wifiConnectMenu();
    return 0;
}

static duk_ret_t native_wifiConnect(duk_context *ctx) {
    // usage: wifiConnect(ssid : string )
    // usage: wifiConnect(ssid : string, timeout_in_seconds : int)
    // usage: wifiConnect(ssid : string, timeout_in_seconds : int, pwd : string)
    String ssid = duk_to_string(ctx, 0);
    int timeout_in_seconds = 10;
    if(duk_is_number(ctx, 1)) timeout_in_seconds = duk_to_int(ctx, 1);

    bool r = false;

    Serial.println("Connecting to: " + ssid);

    if(duk_is_string(ctx, 2)) {
        String pwd = duk_to_string(ctx, 2);
        WiFi.begin(ssid, pwd);
    } else {
        WiFi.begin(ssid);
    }

    int i=0;
    do {
      delay(1000);
      i++;
      if(i>timeout_in_seconds) {
        Serial.println("timeout");
        break;
      }
    } while (WiFi.status() != WL_CONNECTED);

    if(WiFi.status() == WL_CONNECTED) {
        r = true;
        wifiIP = WiFi.localIP().toString(); // update global var
    }

    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_wifiScan(duk_context *ctx) {
    // Example usage: `print(wifiScan()[0].SSID)`
    wifiDisconnect();
    WiFi.mode(WIFI_MODE_STA);
    //Serial.println("Scanning...");
    int nets = WiFi.scanNetworks();
    duk_push_array(ctx);
    int arrayIndex = 0;
    duk_idx_t obj_idx;
    for(int i=0; i<nets; i++){
      // adds all network infos into an object
      obj_idx = duk_push_object(ctx);
      int enctype = int(WiFi.encryptionType(i));
      String e = "UNKNOWN";
      if(enctype==2) e = "TKIP/WPA";
      else if(enctype==5) e = "WEP";
      else if(enctype==4) e = "CCMP/WPA";
      else if(enctype==7) e = "NONE";
      else if(enctype==8) e = "AUTO";
      duk_push_string(ctx, e.c_str());
      duk_put_prop_string(ctx, obj_idx, "encryptionType");
      duk_push_string(ctx, WiFi.SSID(i).c_str());
      duk_put_prop_string(ctx, obj_idx, "SSID");
      duk_push_string(ctx, WiFi.BSSIDstr(i).c_str());
      duk_put_prop_string(ctx, obj_idx, "MAC");
      duk_put_prop_index(ctx, -2, arrayIndex);
      arrayIndex++;
    }
    return 1;
}

static duk_ret_t native_wifiDisconnect(duk_context *ctx) {
    wifiDisconnect();
    return 0;
}

static duk_ret_t native_get(duk_context *ctx) {
  duk_idx_t obj_idx;
  if(WiFi.status() != WL_CONNECTED) wifiConnectMenu();

  if(WiFi.status()== WL_CONNECTED){
      // Your Domain name with URL path or IP address with path
      http.begin(duk_to_string(ctx, 0));

      // Add Headers if headers are included.
      if (duk_is_array(ctx, 1)) {
         // Get the length of the array
        duk_uint_t len = duk_get_length(ctx, 1);
        for (duk_uint_t i = 0; i < len; i++) {
            // Get each element in the array
            duk_get_prop_index(ctx, 1, i);

            // Ensure it's a string
            if (!duk_is_string(ctx, -1)) {
                duk_pop(ctx);
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "Header array elements must be strings.");
            }

            // Get the string
            const char *headerKey = duk_get_string(ctx, -1);
            duk_pop(ctx);
            i++;
            duk_get_prop_index(ctx, 1, i);

            // Ensure it's a string
            if (!duk_is_string(ctx, -1)) {
                duk_pop(ctx);
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "Header array elements must be strings.");
            }

            // Get the string
            const char *headerValue = duk_get_string(ctx, -1);
            duk_pop(ctx);
            http.addHeader(headerKey, headerValue);
        }
      }

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode>0) {
        String payload = http.getString();

        obj_idx = duk_push_object(ctx);
        duk_push_int(ctx, httpResponseCode);
        duk_put_prop_string(ctx, obj_idx, "response");
        duk_push_string(ctx, payload.c_str());
        duk_put_prop_string(ctx, obj_idx, "body");

      }
      else {
        String errorMessage = "Error Response";
        obj_idx = duk_push_object(ctx);
        duk_push_int(ctx, 0);
        duk_put_prop_string(ctx, obj_idx, "response");
        duk_push_string(ctx, errorMessage.c_str());
        duk_put_prop_string(ctx, obj_idx, "body");
      }
      // Free resources
      http.end();
    }
    else {
      String noWifiMessage = "WIFI Not Connected";
      obj_idx = duk_push_object(ctx);
      duk_push_int(ctx, 0);
      duk_put_prop_string(ctx, obj_idx, "response");
      duk_push_string(ctx, noWifiMessage.c_str());
      duk_put_prop_string(ctx, obj_idx, "body");
    }
  return 1;
}


// TFT display functions

static duk_ret_t native_color(duk_context *ctx) {
  int color = tft.color565(duk_to_int(ctx, 0),duk_to_int(ctx, 1),duk_to_int(ctx, 2));
  duk_push_int(ctx, color);
  return 1;
}

static duk_ret_t native_setTextColor(duk_context *ctx) {
  tft.setTextColor(duk_to_int(ctx, 0));
  return 0;
}

static duk_ret_t native_setTextSize(duk_context *ctx) {
  tft.setTextSize(duk_to_int(ctx, 0));
  return 0;
}

static duk_ret_t native_drawRect(duk_context *ctx) {
  tft.drawRect(duk_to_int(ctx, 0),duk_to_int(ctx, 1),duk_to_int(ctx, 2),duk_to_int(ctx, 3),duk_to_int(ctx, 4));
  return 0;
}

static duk_ret_t native_drawFillRect(duk_context *ctx) {
  tft.fillRect(duk_to_int(ctx, 0),duk_to_int(ctx, 1),duk_to_int(ctx, 2),duk_to_int(ctx, 3),duk_to_int(ctx, 4));
  return 0;
}

static duk_ret_t native_drawLine(duk_context *ctx) {
  // usage: drawLine(int16_t x, int16_t y, int16_t x2, int16_t y2, uint16_t color)
  tft.drawLine(duk_to_int(ctx, 0),duk_to_int(ctx, 1),duk_to_int(ctx, 2),duk_to_int(ctx, 3),duk_to_int(ctx, 4));
  return 0;
}

static duk_ret_t native_drawPixel(duk_context *ctx) {
  // usage: drawPixel(int16_t x, int16_t y, uint16_t color)
  tft.drawPixel(duk_to_int(ctx, 0),duk_to_int(ctx, 1),duk_to_int(ctx, 2));
  return 0;
}

static duk_ret_t native_drawString(duk_context *ctx) {
  // drawString(const char *string, int32_t x, int32_t y)
  tft.drawString(duk_to_string(ctx, 0),duk_to_int(ctx, 1),duk_to_int(ctx, 2));
  return 0;
}

static duk_ret_t native_fillScreen(duk_context *ctx) {
  // fill the screen with the passed color
  tft.fillScreen(duk_to_int(ctx, 0));
  return 0;
}

static duk_ret_t native_width(duk_context *ctx) {
  int width = tft.width();
  duk_push_int(ctx, width);
  return 1;
}

static duk_ret_t native_height(duk_context *ctx) {
  int height = tft.height();
  duk_push_int(ctx, height);
  return 1;
}

static duk_ret_t native_drawJpg(duk_context *ctx) {
  // fill the screen with the passed color
  FS *fss;
  String fsss=duk_to_string(ctx,0);
  fsss.toLowerCase();
  if(fsss == "sd") fss = &SD;
  else if(fsss == "littlefs") fss = &LittleFS;
  else fss = &LittleFS;

  showJpeg(*fss,duk_to_string(ctx,1),duk_to_int(ctx,2),duk_to_int(ctx,3));
  return 0;
}

// Input functions

static duk_ret_t native_getPrevPress(duk_context *ctx) {
    if(checkPrevPress()) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getSelPress(duk_context *ctx) {
    if(checkSelPress()) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getNextPress(duk_context *ctx) {
    if(checkNextPress()) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getAnyPress(duk_context *ctx) {
    if(checkAnyKeyPress()) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}

static duk_ret_t native_getKeysPressed(duk_context *ctx) {
#ifdef CARDPUTER
  // Create a new array on the stack
  duk_push_array(ctx);

  kb.update();
    if (kb.isChange()) {
        if (kb.isPressed()) {
            Keyboard_Class::KeysState status = kb.keysState();
            int arrayIndex = 0;
            for (auto i : status.word) {
                char str[2] = { i, '\0' };
                duk_push_string(ctx, str);
                duk_put_prop_index(ctx, -2, arrayIndex);
                arrayIndex++;
            }

            if (status.del) {
                duk_push_string(ctx, "Delete");
                duk_put_prop_index(ctx, -2, arrayIndex);
                arrayIndex++;
            }

            if (status.enter) {
                duk_push_string(ctx, "Enter");
                duk_put_prop_index(ctx, -2, arrayIndex);
                arrayIndex++;
            }

            if (status.alt) {
                duk_push_string(ctx, "Alt");
                duk_put_prop_index(ctx, -2, arrayIndex);
                arrayIndex++;
            }

            if (status.tab) {
                duk_push_string(ctx, "Tab");
                duk_put_prop_index(ctx, -2, arrayIndex);
                arrayIndex++;
            }

            if (status.fn) {
                duk_push_string(ctx, "Function");
                duk_put_prop_index(ctx, -2, arrayIndex);
                arrayIndex++;
            }

            if (status.opt) {
                duk_push_string(ctx, "Option");
                duk_put_prop_index(ctx, -2, arrayIndex);
                arrayIndex++;
            }
        }
    }
#endif
  return 1;
}


// Serial functions

static duk_ret_t native_serialReadln(duk_context *ctx) {
    // usage: serialReadln();   // default to 10s timeout
    // usage: serialReadln(timeout_in_ms : number);
    String line ;
    int maxloops = 1000*10;
    if(duk_is_number(ctx, 0))
        maxloops = duk_to_int(ctx, 0);
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
    bool r = processSerialCommand(String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_playAudioFile(duk_context *ctx) {
    // usage: playAudioFile(filename : string);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in processSerialCommand)
    bool r = processSerialCommand("music_player " + String(duk_to_string(ctx, 0)));
    setup_gpio(); // temp fix for menu inf. loop
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_tone(duk_context *ctx) {
    // usage: tone(frequency : number);
    // usage: tone(frequency : number, duration : number);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in processSerialCommand)
    bool r = processSerialCommand("tone " + String(duk_to_int(ctx, 0)) + " " + String(duk_to_int(ctx, 1)));
    setup_gpio(); // temp fix for menu inf. loop
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_irTransmitFile(duk_context *ctx) {
    // usage: irTransmitFile(filename : string);
    // returns: bool==true on success, false on any error
    bool r = processSerialCommand("ir tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_subghzTransmitFile(duk_context *ctx) {
    // usage: subghzTransmitFile(filename : string);
    // returns: bool==true on success, false on any error
    bool r = processSerialCommand("subghz tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_badusbRunFile(duk_context *ctx) {
    // usage: badusbRunFile(filename : string);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in processSerialCommand)
    bool r = processSerialCommand("badusb tx_from_file " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

// badusb functions

static duk_ret_t native_badusbSetup(duk_context *ctx) {
  // usage: badusbSetup();
  // returns: bool==true on success, false on any error
  #if defined(USB_as_HID)
    Kb.begin();
    //cc.begin();
    USB.begin();
    duk_push_boolean(ctx, true);
  #else
    duk_push_boolean(ctx, false);
  #endif
  return 1;
}

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
  // keycodes list: https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/src/USBHIDKeyboard.h
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
  // keycodes list: TinyUSB's HID_KEY_* macros https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
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
  // keycodes list:  https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/src/USBHIDConsumerControl.h
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

static duk_ret_t native_irRead(duk_context *ctx) {
  // usage: irRead();
  // usage: irRead(timeout_in_seconds : number);
  // returns a string of the generated ir file, empty string on timeout or other errors
  IrRead i = IrRead(true);  // true == headless mode
  String r = "";
  if(duk_is_number(ctx, 0))
    r = i.loop_headless(duk_to_int(ctx, 0));   // custom timeout
  else
    r = i.loop_headless(10);   // 10s timeout
  duk_push_string(ctx, r.c_str());
  return 1;
}

static duk_ret_t native_irReadRaw(duk_context *ctx) {
  // usage: irReadRaw();
  // usage: irRead(timeout_in_seconds : number);
  // returns a string of the generated ir file, empty string on timeout or other errors
  IrRead i = IrRead(true, true);  // true == headless mode, true==raw mode
  String r = "";
  if(duk_is_number(ctx, 0))
    r = i.loop_headless(duk_to_int(ctx, 0));   // custom timeout
  else
    r = i.loop_headless(10);   // 10s timeout
  duk_push_string(ctx, r.c_str());
  return 1;
}

// Subghz functions

static duk_ret_t native_subghzRead(duk_context *ctx) {
  // usage: subghzRead();
  // usage: subghzRead(timeout_in_seconds : number);
  // returns a string of the generated sub file, empty string on timeout or other errors (decoding failed)
  String r = "";
  if(duk_is_number(ctx, 0))
    r = RCSwitch_Read(bruceConfig.rfFreq, duk_to_int(ctx, 0));   // custom timeout
  else
    r = RCSwitch_Read(bruceConfig.rfFreq, 10);
  duk_push_string(ctx, r.c_str());
  return 1;
}

static duk_ret_t native_subghzReadRaw(duk_context *ctx) {
  String r = "";
  if(duk_is_number(ctx, 0))
    r = RCSwitch_Read(bruceConfig.rfFreq, duk_to_int(ctx, 0), true);   // custom timeout
  else
    r = RCSwitch_Read(bruceConfig.rfFreq, 10, true);
  duk_push_string(ctx, r.c_str());
  return 1;
}


static duk_ret_t native_subghzSetFrequency(duk_context *ctx) {
  // usage: subghzSetFrequency(freq_as_float);
  if(duk_is_number(ctx, 0))
    bruceConfig.rfFreq = duk_to_number(ctx, 0);  // float global var
  return 0;
}

// Dialog functions

static duk_ret_t native_dialogMessage(duk_context *ctx) {
  // usage: dialogMessage(msg : string)
  displayInfo(String(duk_to_string(ctx, 0)));
  return 0;
}

static duk_ret_t native_dialogError(duk_context *ctx) {
  // usage: dialogError(msg : string)
  displayError(String(duk_to_string(ctx, 0)));
  return 0;
}

static duk_ret_t native_dialogPickFile(duk_context *ctx) {
  // usage: dialogPickFile() : string
  // usage: dialogPickFile(path : string) : string
  // returns: selected file , empty string if cancelled
  String r = "";
  String filepath = "/";
  if(duk_is_string(ctx, 0)) {
    filepath = String(duk_to_string(ctx, 0));
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
  }
  FS* fs = NULL;
  if(SD.exists(filepath)) fs = &SD;
  if(LittleFS.exists(filepath)) fs = &LittleFS;
  if(fs) {
    r = loopSD(*fs, true);
  }
  duk_push_string(ctx, r.c_str());
  return 1;
}

static duk_ret_t native_dialogChoice(duk_context *ctx) {
    // usage: dialogChoice(choices : string[])
    // returns: string (val1, 2, ...), or empty string if cancelled
    const char* r = "";

    if (duk_is_array(ctx, 0)) {
        options = {};

        // Get the length of the array
        duk_uint_t len = duk_get_length(ctx, 0);
        for (duk_uint_t i = 0; i < len; i++) {
            // Get each element in the array
            duk_get_prop_index(ctx, 0, i);

            // Ensure it's a string
            if (!duk_is_string(ctx, -1)) {
                duk_pop(ctx);
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "Choice array elements must be strings.");
            }

            // Get the string
            const char *choiceKey = duk_get_string(ctx, -1);
            duk_pop(ctx);
            i++;
            duk_get_prop_index(ctx, 0, i);

            // Ensure it's a string
            if (!duk_is_string(ctx, -1)) {
                duk_pop(ctx);
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "Choice array elements must be strings.");
            }

            // Get the string
            const char *choiceValue = duk_get_string(ctx, -1);
            duk_pop(ctx);

            // add to the choices list
            options.push_back({choiceKey, [choiceValue, &r]() { r = choiceValue; }});
        }  // end for

        options.push_back({"Cancel", [&]() { r = ""; }});

        delay(200);
        loopOptions(options);
      }

      duk_push_string(ctx, r);
      return 1;
}

static duk_ret_t native_dialogViewFile(duk_context *ctx) {
  // usage: dialogViewFile(path : string)
  // returns: nothing
  if(duk_is_string(ctx, 0)) {
    String filepath = String(duk_to_string(ctx, 0));
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    FS* fs = NULL;
    if(SD.exists(filepath)) fs = &SD;
    if(LittleFS.exists(filepath)) fs = &LittleFS;
    if(fs) {
        viewFile(*fs, filepath);
    }
  }
  return 0;
}

static duk_ret_t native_keyboard(duk_context *ctx) {
  // usage: keyboard() : string
  // usage: keyboard(title : string) : string
  // usage: keyboard(title : string, maxlen : int) : string
  // usage: keyboard(title : string, maxlen : int, initval : string) : string
  // returns: text typed by the user
  String r = "";
  if(!duk_is_string(ctx, 0))
    r = keyboard("");
  else if(!duk_is_number(ctx, 1))
    r = keyboard(String(duk_to_string(ctx, 0)));
  else if(!duk_is_string(ctx, 2))
    r = keyboard(String(duk_to_string(ctx, 0)), duk_to_int(ctx, 1));
  else
    r = keyboard(String(duk_to_string(ctx, 0)), duk_to_int(ctx, 1), String(duk_to_string(ctx, 2)));
  duk_push_string(ctx, r.c_str());
  return 1;
}


// Storage functions

static duk_ret_t native_storageRead(duk_context *ctx) {
  // usage: storageRead(filename : string)
  // returns: file contents as a string. Empty string on any error.
  String r = "";
  if(duk_is_string(ctx, 0)) {
    String filepath = String(duk_to_string(ctx, 0));
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    if(SD.exists(filepath)) r = readSmallFile(SD, filepath);
    if(LittleFS.exists(filepath)) r = readSmallFile(LittleFS, filepath);
  }
  duk_push_string(ctx, r.c_str());
  return 1;
}

static duk_ret_t native_storageWrite(duk_context *ctx) {
  // usage: storageWrite(filename : string, data : string)
  // The write function writes a string to a file, returning true if successful. Overwrites existing file.
  // The first parameter is the path of the file.
  // The second parameter is the contents to write
  bool r = false;
  if(duk_is_string(ctx, 0) && duk_is_string(ctx, 1)) {
    String filepath = String(duk_to_string(ctx, 0));
    String data = String(duk_to_string(ctx, 1));
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
    FS* fs = &LittleFS; // default fallback
    if(SD.exists(filepath)) fs = &SD;
    if(LittleFS.exists(filepath)) fs = &LittleFS;
    if(!fs && sdcardMounted) fs = &SD;
    File f = fs->open(filepath, FILE_APPEND, true);  // create if it does not exist, append otherwise
    if(f) {
        f.write((const uint8_t*) data.c_str(), data.length());
        f.close();
        r = true;  // success
    }
  }
  duk_push_boolean(ctx, r);
  return 1;
}


// Read script file
String readScriptFile(FS fs, String filename) {
    String fileError = "drawString('No boot.js file.', 4, 4);";

    File file = fs.open(filename);
    if (!file) {
        return fileError;
    }

    String s;
    Serial.println("Read from file");
    while (file.available()) {
        s += (char)file.read();
    }
    file.close();
    Serial.println("loaded file:");
    Serial.println(s);
    return s;
}
// Code interpreter, must be called in the loop() function to work
bool interpreter() {
        /*
        if(!checkPrevPress() && !checkNextPress()) interpreter_start=false;
        #if defined (CARDPUTER)
            if(checkEscPress()) interpreter_start=false;
        #endif
        * */
        tft.fillRect(0,0,WIDTH,HEIGHT,TFT_BLACK);
        tft.setRotation(bruceConfig.rotation);
        tft.setTextSize(FM);
        tft.setTextColor(TFT_WHITE);
        // Create context.
        duk_context *ctx = duk_create_heap_default();

        // Add native functions to context.
        duk_push_c_function(ctx, native_load, 1);
        duk_put_global_string(ctx, "load");
        duk_push_c_function(ctx, native_print, 1);
        duk_put_global_string(ctx, "print");
        duk_push_c_function(ctx, native_now, 0);
        duk_put_global_string(ctx, "now");
        duk_push_c_function(ctx, native_delay, 1);
        duk_put_global_string(ctx, "delay");
        duk_push_c_function(ctx, native_digitalWrite, 2);
        duk_put_global_string(ctx, "digitalWrite");
        duk_push_c_function(ctx, native_pinMode, 2);
        duk_put_global_string(ctx, "pinMode");
        //duk_push_c_function(ctx, native_exit, 0);
        //duk_put_global_string(ctx, "exit");

        // Get Informations from the board
        duk_push_c_function(ctx, native_getBattery, 0);
        duk_put_global_string(ctx, "getBattery");
        duk_push_c_function(ctx, native_getBoard, 0);
        duk_put_global_string(ctx, "getBoard");


        // Networking
        duk_push_c_function(ctx, native_wifiConnect, 2);
        duk_put_global_string(ctx, "wifiConnect");
        duk_push_c_function(ctx, native_wifiConnectDialog, 0);
        duk_put_global_string(ctx, "wifiConnectDialog");
        duk_push_c_function(ctx, native_wifiDisconnect, 0);
        duk_put_global_string(ctx, "wifiDisconnect");
        duk_push_c_function(ctx, native_wifiScan, 0);
        duk_put_global_string(ctx, "wifiScan");
        duk_push_c_function(ctx, native_get, 2);
        duk_put_global_string(ctx, "httpGet");
        // TODO: get mac addresses

        // Graphics
        duk_push_c_function(ctx, native_color, 3);
        duk_put_global_string(ctx, "color");
        duk_push_c_function(ctx, native_setTextColor, 1);
        duk_put_global_string(ctx, "setTextColor");
        duk_push_c_function(ctx, native_setTextSize, 1);
        duk_put_global_string(ctx, "setTextSize");
        duk_push_c_function(ctx, native_drawRect, 5);
        duk_put_global_string(ctx, "drawRect");
        duk_push_c_function(ctx, native_drawFillRect, 5);
        duk_put_global_string(ctx, "drawFillRect");
        duk_push_c_function(ctx, native_drawLine, 5);
        duk_put_global_string(ctx, "drawLine");
        duk_push_c_function(ctx, native_drawString, 3);
        duk_put_global_string(ctx, "drawString");
        duk_push_c_function(ctx, native_drawPixel, 3);
        duk_put_global_string(ctx, "drawPixel");
        // TODO: drawBitmap(filename:string, x, y)
        duk_push_c_function(ctx, native_fillScreen, 1);
        duk_put_global_string(ctx, "fillScreen");
        duk_push_c_function(ctx, native_drawJpg, 4); //drawJpg(fs,filepath,x,y)
        duk_put_global_string(ctx, "drawJpg");       //drawJpg("SD","/boot.jpg",10,10);



        duk_push_c_function(ctx, native_width, 0);
        duk_put_global_string(ctx, "width");
        duk_push_c_function(ctx, native_height, 0);
        duk_put_global_string(ctx, "height");

        // Input
        duk_push_c_function(ctx, native_getKeysPressed, 0); // keyboard btns for cardputer (entry)
        duk_put_global_string(ctx, "getKeysPressed");
        duk_push_c_function(ctx, native_getPrevPress, 0); // checkPrevPress
        duk_put_global_string(ctx, "getPrevPress");
        duk_push_c_function(ctx, native_getSelPress, 0); // checkSelPress
        duk_put_global_string(ctx, "getSelPress");
        duk_push_c_function(ctx, native_getNextPress, 0); // checkNextPress
        duk_put_global_string(ctx, "getNextPress");
        duk_push_c_function(ctx, native_getAnyPress, 0);
        duk_put_global_string(ctx, "getAnyPress");

        // Serial + wrappers
        duk_push_c_function(ctx, native_serialReadln, 0);
        duk_put_global_string(ctx, "serialReadln");
        duk_push_c_function(ctx, native_serialCmd, 1);
        duk_put_global_string(ctx, "serialCmd");
        duk_push_c_function(ctx, native_playAudioFile, 1);
        duk_put_global_string(ctx, "playAudioFile");
        duk_push_c_function(ctx, native_tone, 2);
        duk_put_global_string(ctx, "tone");
        duk_push_c_function(ctx, native_irTransmitFile, 1);
        duk_put_global_string(ctx, "irTransmitFile");
        duk_push_c_function(ctx, native_subghzTransmitFile, 1);
        duk_put_global_string(ctx, "subghzTransmitFile");
        duk_push_c_function(ctx, native_badusbRunFile, 1);
        duk_put_global_string(ctx, "badusbRunFile");

        // badusb functions
        duk_push_c_function(ctx, native_badusbSetup, 0);
        duk_put_global_string(ctx, "badusbSetup");
        duk_push_c_function(ctx, native_badusbPrint, 1);
        duk_put_global_string(ctx, "badusbPrint");
        duk_push_c_function(ctx, native_badusbPrintln, 1);
        duk_put_global_string(ctx, "badusbPrintln");
        duk_push_c_function(ctx, native_badusbPress, 1);
        duk_put_global_string(ctx, "badusbPress");
        duk_push_c_function(ctx, native_badusbHold, 1);
        duk_put_global_string(ctx, "badusbHold");
        duk_push_c_function(ctx, native_badusbRelease, 1);
        duk_put_global_string(ctx, "badusbRelease");
        duk_push_c_function(ctx, native_badusbReleaseAll, 0);
        duk_put_global_string(ctx, "badusbReleaseAll");
        duk_push_c_function(ctx, native_badusbPressRaw, 1);
        duk_put_global_string(ctx, "badusbPressRaw");
        //duk_push_c_function(ctx, native_badusbPressSpecial, 1);
        //duk_put_global_string(ctx, "badusbPressSpecial");

        // IR functions
        duk_push_c_function(ctx, native_irRead, 0);
        duk_put_global_string(ctx, "irRead");
        duk_push_c_function(ctx, native_irReadRaw, 0);
        duk_put_global_string(ctx, "irReadRaw");
        //TODO: irTransmit(string)

        // subghz functions
        duk_push_c_function(ctx, native_subghzRead, 0);
        duk_put_global_string(ctx, "subghzRead");
        duk_push_c_function(ctx, native_subghzReadRaw, 0);
        duk_put_global_string(ctx, "subghzReadRaw");
        duk_push_c_function(ctx, native_subghzSetFrequency, 1);
        duk_put_global_string(ctx, "subghzSetFrequency");
        //duk_put_global_string(ctx, "subghzSetIdle");
        // TODO: subghzTransmit(string)

        // Dialog functions
        duk_push_c_function(ctx, native_dialogMessage, 1);
        duk_put_global_string(ctx, "dialogMessage");
        duk_push_c_function(ctx, native_dialogError, 1);
        duk_put_global_string(ctx, "dialogError");
        // TODO: dialogYesNo()
        duk_push_c_function(ctx, native_dialogPickFile, 1);
        duk_put_global_string(ctx, "dialogPickFile");
        duk_push_c_function(ctx, native_dialogChoice, 1);
        duk_put_global_string(ctx, "dialogChoice");
        duk_push_c_function(ctx, native_dialogViewFile, 1);
        duk_put_global_string(ctx, "dialogViewFile");
        duk_push_c_function(ctx, native_keyboard, 3);
        duk_put_global_string(ctx, "keyboard");

        // Storage functions
        duk_push_c_function(ctx, native_storageRead, 1);
        duk_put_global_string(ctx, "storageRead");
        duk_push_c_function(ctx, native_storageWrite, 2);
        duk_put_global_string(ctx, "storageWrite");
        // TODO: wrap more serial storage cmd: mkdir, remove, ...

        // TODO: match flipper syntax https://github.com/jamisonderek/flipper-zero-tutorials/wiki/JavaScript
        //    https://github.com/jamisonderek/flipper-zero-tutorials/wiki/JavaScript
        // MEMO: API https://duktape.org/api.html  https://github.com/joeqread/arduino-duktape/blob/main/src/duktape.h

        bool r;

        duk_push_string(ctx, script.c_str());
        if (duk_peval(ctx) != 0) {
            printf("eval failed: %s\n", duk_safe_to_string(ctx, -1));
            r = false;
        } else {
            printf("result is: %s\n", duk_safe_to_string(ctx, -1));
            r = true;
        }
        duk_pop(ctx);

        // Clean up.
        duk_destroy_heap(ctx);

        //delay(1000);
        return r;
}

// function to start the JS Interpreterm choosinng the file, processing and start
void run_bjs_script() {
    String filename;
    FS *fs=&LittleFS;
    setupSdCard();
    if(sdcardMounted) {
        options = {
            {"Sd Card", [&](){ fs=&SD; }},
            {"LittleFs", [&](){ fs=&LittleFS; }},
        };
        delay(200);
        loopOptions(options);
        delay(200);
    }
    filename = loopSD(*fs,true,"BJS|JS");
    script = readScriptFile(*fs, filename);

    returnToMenu=true;
    interpreter_start=true;

    // To stop the script, press Prev and Next together for a few seconds
}

bool run_bjs_script_headless(String code) {
    script = code;
    returnToMenu=true;
    interpreter_start=true;
    //while(interpreter_start) {
        interpreter();
    //    delay(1);
    //}
    interpreter_start=false;
    return true;
}

bool run_bjs_script_headless(FS fs, String filename) {
    script = readScriptFile(fs, filename);
    returnToMenu=true;
    interpreter_start=true;
    return true;
}
