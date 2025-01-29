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

static bool isScriptDynamic = false;
static const char *script = "drawString('Something wrong.', 4, 4);";
static const char *scriptDirpath = "";
static const char *scriptName = "";
HTTPClient http;


static void registerFunction(duk_context *ctx, const char *name, duk_c_function func, duk_idx_t nargs) {
	duk_push_c_function(ctx, func, nargs);
	duk_put_global_string(ctx, name);
}

static void registerLightFunction(duk_context *ctx, const char *name, duk_c_function func, duk_idx_t nargs, duk_idx_t magic = 0) {
	duk_push_c_lightfunc(ctx, func, nargs, nargs, magic);
	duk_put_global_string(ctx, name);
}

static void registerInt(duk_context *ctx, const char *name, duk_int_t val) {
  duk_push_int(ctx, val);
  duk_put_global_string(ctx, name);
}

static void registerString(duk_context *ctx, const char *name, const char *val) {
  duk_push_string(ctx, val);
  duk_put_global_string(ctx, name);
}

static void putPropLightFunction(
  duk_context *ctx,
  duk_idx_t obj_idx,
  const char *name,
  duk_c_function func,
  duk_idx_t nargs,
  duk_idx_t magic = 0
) {
  duk_push_c_lightfunc(ctx, func, nargs, nargs, magic);
  duk_put_prop_string(ctx, obj_idx, name);
}

#define putProp(ctx, obj_idx, name, prop_type, prop_value) \
  do { \
    prop_type((ctx), (prop_value)); \
    duk_put_prop_string((ctx), (obj_idx), (name)); \
  } while(0)


static duk_ret_t native_noop(duk_context *ctx) {
  return 0;
}

static duk_ret_t native_load(duk_context *ctx) {
  if (isScriptDynamic) {
    free((char *)script);
    free((char *)scriptDirpath);
    free((char *)scriptName);
  }
  script = strdup(duk_to_string(ctx, 0));
  scriptDirpath = strdup("");
  scriptName = strdup("");
  isScriptDynamic = true;
  return 0;
}

static duk_ret_t native_serialPrintln(duk_context *ctx) {
  for (duk_idx_t argIndex = 0; argIndex < 6; argIndex++) {
    duk_uint_t arg0Type = duk_get_type_mask(ctx, argIndex);
    if (arg0Type & DUK_TYPE_MASK_NONE) {
      break;
    } else if (arg0Type & DUK_TYPE_MASK_UNDEFINED) {
      Serial.print("undefined ");
    } else if (arg0Type & DUK_TYPE_MASK_NULL) {
      Serial.print("null ");
    } else if (arg0Type & (DUK_TYPE_MASK_BOOLEAN | DUK_TYPE_MASK_NUMBER)) {
      Serial.printf("%f ", duk_to_number(ctx, 0));
    } else {
      Serial.printf("%s ", duk_to_string(ctx, 0));
    }
  }
  Serial.println();

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

static duk_ret_t native_random(duk_context *ctx) {
  int val = random(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
  duk_push_int(ctx, val);
  return 1;
}

// Hardware GPIO interactions
static duk_ret_t native_digitalWrite(duk_context *ctx) {
  digitalWrite(duk_to_int(ctx, 0), duk_to_boolean(ctx, 1));
  return 0;
}

static duk_ret_t native_analogWrite(duk_context *ctx) {
  analogWrite(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
  return 0;
}

static duk_ret_t native_digitalRead(duk_context *ctx) {
  int val = digitalRead(duk_to_int(ctx, 0));
  duk_push_int(ctx, val);
  return 1;
}

static duk_ret_t native_analogRead(duk_context *ctx) {
  int val = analogRead(duk_to_int(ctx, 0));
  duk_push_int(ctx, val);
  return 1;
}

static duk_ret_t native_pinMode(duk_context *ctx) {
  pinMode(duk_to_int(ctx, 0), duk_to_int(ctx, 1));
  return 0;
}

static duk_ret_t native_parse_int(duk_context *ctx) {
  duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);

  if (arg0Type & (DUK_TYPE_MASK_STRING | DUK_TYPE_MASK_NUMBER)) {
    duk_push_number(ctx, duk_to_number(ctx, 0));
  } else {
    duk_push_nan(ctx);
  }

  return 1;
}

static duk_ret_t native_to_string(duk_context *ctx) {
  duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);

  if (arg0Type & (DUK_TYPE_MASK_STRING | DUK_TYPE_MASK_NUMBER)) {
    duk_push_string(ctx, duk_to_string(ctx, 0));
  } else {
    duk_push_string(ctx, "");
  }

  return 1;
}

static duk_ret_t native_to_hex_string(duk_context *ctx) {
  duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);

  if (arg0Type & (DUK_TYPE_MASK_STRING | DUK_TYPE_MASK_NUMBER)) {
    duk_push_string(ctx, String(duk_to_number(ctx, 0), HEX).c_str());
  } else {
    duk_push_string(ctx, "");
  }

  return 1;
}

static duk_ret_t native_to_lower_case(duk_context *ctx) {
  String text = duk_to_string(ctx, 0);
  text.toLowerCase();
  duk_push_string(ctx, text.c_str());

  return 1;
}

static duk_ret_t native_to_upper_case(duk_context *ctx) {
  String text = duk_to_string(ctx, 0);
  text.toUpperCase();
  duk_push_string(ctx, text.c_str());

  return 1;
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
    putProp(ctx, obj_idx, "ram_free", duk_push_uint, info.total_free_bytes);
    putProp(ctx, obj_idx, "ram_min_free", duk_push_uint, info.minimum_free_bytes);
    putProp(ctx, obj_idx, "ram_largest_free_block", duk_push_uint, info.largest_free_block);
    putProp(ctx, obj_idx, "ram_size", duk_push_uint, ESP.getHeapSize());
    putProp(ctx, obj_idx, "psram_free", duk_push_uint, ESP.getFreePsram());
    putProp(ctx, obj_idx, "psram_size", duk_push_uint, ESP.getPsramSize());

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

    WiFi.mode(WIFI_MODE_STA);
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
        wifiConnected = true;
    }

    duk_push_boolean(ctx, r);
    return 1;
}

const char *wifi_enc_types[] = {
  "OPEN",
  "WEP",
  "WPA_PSK",
  "WPA2_PSK",
  "WPA_WPA2_PSK",
  "ENTERPRISE",
  "WPA2_ENTERPRISE",
  "WPA3_PSK",
  "WPA2_WPA3_PSK",
  "WAPI_PSK",
  "WPA3_ENT_192",
  "MAX"
};

static duk_ret_t native_wifiScan(duk_context *ctx) {
    WiFi.mode(WIFI_MODE_STA);
    int nets = WiFi.scanNetworks();
    duk_idx_t arr_idx = duk_push_array(ctx);
    int arrayIndex = 0;
    duk_idx_t obj_idx;

    for(int i = 0; i < nets; i++) {
      obj_idx = duk_push_object(ctx);
      int enctypeInt = int(WiFi.encryptionType(i));

      const char *enctype = enctypeInt < 12 ? wifi_enc_types[enctypeInt] : "UNKNOWN";
      putProp(ctx, obj_idx, "encryptionType", duk_push_string, enctype);
      putProp(ctx, obj_idx, "SSID", duk_push_string, WiFi.SSID(i).c_str());
      putProp(ctx, obj_idx, "MAC", duk_push_string, WiFi.BSSIDstr(i).c_str());
      duk_put_prop_index(ctx, arr_idx, arrayIndex);
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

  if(WiFi.status()== WL_CONNECTED) {
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
        putProp(ctx, obj_idx, "response", duk_push_int, httpResponseCode);
        putProp(ctx, obj_idx, "body", duk_push_string, payload.c_str());
      }
      else {
        String errorMessage = "Error Response";

        obj_idx = duk_push_object(ctx);
        putProp(ctx, obj_idx, "response", duk_push_int, 0);
        putProp(ctx, obj_idx, "body", duk_push_string, errorMessage.c_str());
      }
      // Free resources
      http.end();
  } else {
    String noWifiMessage = "WIFI Not Connected";

    obj_idx = duk_push_object(ctx);
    putProp(ctx, obj_idx, "response", duk_push_int, 0);
    putProp(ctx, obj_idx, "body", duk_push_string, noWifiMessage.c_str());
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

static duk_ret_t native_setCursor(duk_context *ctx) {
  // setCursor(int16_t x, int16_t y)
  tft.setCursor(duk_to_int(ctx, 0), duk_to_int(ctx, 0));
  return 0;
}

static duk_ret_t native_print(duk_context *ctx) {
  if (duk_is_string(ctx, 0)) {
    tft.print(duk_to_string(ctx, 0));
  } else if (duk_is_number(ctx, 0) || duk_is_boolean(ctx, 0) || duk_is_null_or_undefined(ctx, 0)) {
    tft.print(duk_to_number(ctx, 0));
  }
  return 0;
}

static duk_ret_t native_println(duk_context *ctx) {
  if (duk_is_string(ctx, 0)) {
    tft.println(duk_to_string(ctx, 0));
  } else if (duk_is_number(ctx, 0) || duk_is_boolean(ctx, 0) || duk_is_null_or_undefined(ctx, 0)) {
    tft.println(duk_to_number(ctx, 0));
  }
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
  FS *fss;
  String fsss = duk_to_string(ctx,0);
  fsss.toLowerCase();
  if(fsss == "sd") fss = &SD;
  else if(fsss == "littlefs") fss = &LittleFS;
  else fss = &LittleFS;

  showJpeg(*fss, duk_to_string(ctx, 1), duk_to_int(ctx, 2), duk_to_int(ctx, 3));
  return 0;
}

static duk_ret_t native_drawGif(duk_context *ctx) {
  FS *fss;
  String fsss = duk_to_string(ctx,0);
  fsss.toLowerCase();
  if(fsss == "sd") fss = &SD;
  else if(fsss == "littlefs") fss = &LittleFS;
  else fss = &LittleFS;

  showGif(fss, duk_to_string(ctx, 1), duk_to_int(ctx, 2), duk_to_int(ctx, 3), duk_to_int(ctx, 4), duk_to_int(ctx, 5));
  return 0;
}

static std::vector<Gif*> gifs;
static void clearGifsVector() {
  for (auto gif : gifs) {
    delete gif;
    gif = NULL;
  }
  gifs.clear();
}

static duk_ret_t native_gifPlayFrame(duk_context *ctx) {
  int gifIndex = 0;
  int x = duk_to_int(ctx, 0);
  int y = duk_to_int(ctx, 1);

  duk_push_this(ctx);
  if (duk_get_prop_string(ctx, -1, "gifPointer")) {
    gifIndex = duk_to_int(ctx, -1) - 1;
  }

  uint8_t result = 0;
  if (gifIndex >= 0) {
    Gif *gif = gifs.at(gifIndex);
    if (gif != NULL) {
      result = gif->playFrame(x, y);
    }
  }

  duk_push_int(ctx, result);
  return 1;
}

static duk_ret_t native_gifDimensions(duk_context *ctx) {
  int gifIndex = 0;

  duk_push_this(ctx);
  if (duk_get_prop_string(ctx, -1, "gifPointer")) {
    gifIndex = duk_to_int(ctx, -1) - 1;
  }

  if (gifIndex < 0) {
    duk_push_int(ctx, 0);
  } else {
    Gif *gif = gifs.at(gifIndex);
    if (gif != NULL) {
      int canvasWidth = gifs.at(gifIndex)->getCanvasWidth();
      int canvasHeight = gifs.at(gifIndex)->getCanvasHeight();

      duk_idx_t obj_idx = duk_push_object(ctx);
      putProp(ctx, obj_idx, "width", duk_push_int, canvasWidth);
      putProp(ctx, obj_idx, "height", duk_push_int, canvasHeight);
    }
  }

  return 1;
}

static duk_ret_t native_gifReset(duk_context *ctx) {
  int gifIndex = 0;

  duk_push_this(ctx);
  if (duk_get_prop_string(ctx, -1, "gifPointer")) {
    gifIndex = duk_to_int(ctx, -1) - 1;
  }

  uint8_t result = 0;
  if (gifIndex >= 0) {
    Gif *gif = gifs.at(gifIndex);
    if (gif != NULL) {
      gifs.at(gifIndex)->reset();
      result = 1;
    }
  }
  duk_push_int(ctx, result);

  return 1;
}

static duk_ret_t native_gifClose(duk_context *ctx) {
  int gifIndex = 0;

  if (duk_is_object(ctx, 0)) {
    duk_to_object(ctx, 0);
  } else {
    duk_push_this(ctx);
  }
  if (duk_get_prop_string(ctx, -1, "gifPointer")) {
    gifIndex = duk_to_int(ctx, -1) - 1;
  }

  uint8_t result = 0;
  if (gifIndex >= 0) {
    Gif *gif = gifs.at(gifIndex);
    if (gif != NULL) {
      delete gif;
      gifs.at(gifIndex) = NULL;
      result = 1;
    }
  }
  duk_push_int(ctx, result);

  return 1;
}

static duk_ret_t native_gifOpen(duk_context *ctx) {
  FS *fss;
  String fsss = duk_to_string(ctx, 0);
  fsss.toLowerCase();
  if(fsss == "sd") fss = &SD;
  else if(fsss == "littlefs") fss = &LittleFS;
  else fss = &LittleFS;

  Gif *gif = new Gif();

  bool success = gif->openGIF(fss, duk_to_string(ctx, 1));
  if (!success) {
    duk_push_null(ctx); // return null if not success
  } else {
    gifs.push_back(gif);
    duk_idx_t obj_idx = duk_push_object(ctx);
    putProp(ctx, obj_idx, "gifPointer", duk_push_uint, gifs.size()); // MEMO: 1 is the first element so 0 can be error

    putPropLightFunction(ctx, obj_idx, "playFrame", native_gifPlayFrame, 2);
    putPropLightFunction(ctx, obj_idx, "dimensions", native_gifDimensions, 0);
    putPropLightFunction(ctx, obj_idx, "reset", native_gifReset, 0);
    putPropLightFunction(ctx, obj_idx, "close", native_gifClose, 0);

    duk_push_c_lightfunc(ctx, native_gifClose, 1, 1, 0);
    duk_set_finalizer(ctx, obj_idx);
  }

  return 1;
}


// Input functions

static duk_ret_t native_getPrevPress(duk_context *ctx) {
    if(check(PrevPress)) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getSelPress(duk_context *ctx) {
    if(check(SelPress)) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getNextPress(duk_context *ctx) {
    if(check(NextPress)) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}
static duk_ret_t native_getAnyPress(duk_context *ctx) {
    if(check(AnyKeyPress)) duk_push_boolean(ctx, true);
    else duk_push_boolean(ctx, false);
    return 1;
}

static duk_ret_t native_getKeysPressed(duk_context *ctx) {
#ifdef HAS_KEYBOARD
  // Create a new array on the stack
  duk_push_array(ctx);
  keyStroke key = _getKeyPress();
  if(!key.pressed) return 1; // if nothing has beed pressed, return 1
  int arrayIndex = 0;
  for (auto i : key.word) {
    char str[2] = { i, '\0' };
    duk_push_string(ctx, str);
    duk_put_prop_index(ctx, -2, arrayIndex);
    arrayIndex++;
  }
  if(key.del) {
    duk_push_string(ctx, "Delete");
    duk_put_prop_index(ctx, -2, arrayIndex);
    arrayIndex++;
  }
  if(key.enter) {
    duk_push_string(ctx, "Enter");
    duk_put_prop_index(ctx, -2, arrayIndex);
    arrayIndex++;
  }
  if(key.fn){
    duk_push_string(ctx, "Function");
    duk_put_prop_index(ctx, -2, arrayIndex);
    arrayIndex++;
  }
  for(auto i : key.modifier_keys)
  {
    if(i==0x82) {
      duk_push_string(ctx, "Alt");
      duk_put_prop_index(ctx, -2, arrayIndex);
      arrayIndex++;
    }
    else if(i==0x2B) {
      duk_push_string(ctx, "Tab");
      duk_put_prop_index(ctx, -2, arrayIndex);
      arrayIndex++;
    }
    else if(i==0x00){
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
    duk_push_boolean(ctx, r);
    return 1;
}

static duk_ret_t native_tone(duk_context *ctx) {
    // usage: tone(frequency : number);
    // usage: tone(frequency : number, duration : number);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in processSerialCommand)
    bool r = processSerialCommand("tone " + String(duk_to_int(ctx, 0)) + " " + String(duk_to_int(ctx, 1)));
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

static duk_ret_t native_require(duk_context *ctx) {
  duk_idx_t obj_idx = duk_push_object(ctx);

  if(!duk_is_string(ctx, 0)) {
    return 1;
  }
  String filepath = duk_to_string(ctx, 0);

  if (filepath == "badusb") {
  } else if (filepath == "blebeacon") {

  } else if (filepath == "dialog") {

  } else if (filepath == "flipper" || filepath == "device") {

  } else if (filepath == "gpio") {

  } else if (filepath == "keyboard") {

  } else if (filepath == "math") {

  } else if (filepath == "notification") {

  } else if (filepath == "serial") {

  } else if (filepath == "storage") {

  } else if (filepath == "subghz") {
    putPropLightFunction(ctx, obj_idx, "setFrequency", native_subghzSetFrequency, 1);
    putPropLightFunction(ctx, obj_idx, "transmitFile", native_subghzTransmitFile, 1);
    putPropLightFunction(ctx, obj_idx, "setup", native_noop, 1);
    putPropLightFunction(ctx, obj_idx, "setIdle", native_noop, 1);
  } else if (filepath == "submenu") {

  } else if (filepath == "textbox") {

  } else if (filepath == "usbdisk") {

  } else if (filepath == "vgm") {

  } else if (filepath == "widget") {

  } else {

  }

  return 1;
}

// Read script file
const char *readScriptFile(FS fs, String filename) {
  File file = fs.open(filename);
  const char *fileError = "drawString('Something wrong.', 4, 4);";

  if (!file) {
    Serial.println("Could not open file");
    return "drawString('Could not open file.', 4, 4);";
  }

  char *buf;
  size_t len = file.size();
  if (psramFound()) {
    buf = (char *)ps_malloc((len + 1) * sizeof(char));
  } else {
    buf = (char *)malloc((len + 1) * sizeof(char));
  }

  if (!buf) {
    Serial.println("Could not allocate memory for file");
    return "drawString('Could not allocate memory for file.', 4, 4);";
  }

  Serial.println("Reading from file");
  size_t bytesRead = 0;

  while (bytesRead < len && file.available()) {
    size_t toRead = len - bytesRead;
    if (toRead > 512) {
      toRead = 512;
    }
    file.read((uint8_t *)(buf + bytesRead), toRead);
    bytesRead += toRead;
  }
  buf[bytesRead] = '\0';

  file.close();
  Serial.println("loaded file:");
  Serial.println(buf);

  return buf;
}

static void registerConsole(duk_context *ctx) {
  duk_idx_t obj_idx = duk_push_object(ctx);
  putPropLightFunction(ctx, obj_idx, "error", native_serialPrintln, 6);
  putPropLightFunction(ctx, obj_idx, "warn", native_serialPrintln, 6);
  putPropLightFunction(ctx, obj_idx, "log", native_serialPrintln, 6);
  putPropLightFunction(ctx, obj_idx, "debug", native_serialPrintln, 6);

  duk_put_global_string(ctx, "console");
}

void *ps_alloc_function(void *udata, duk_size_t size) {
	void *res;
	DUK_UNREF(udata);
	res = ps_malloc(size);
	return res;
}

void *ps_realloc_function(void *udata, void *ptr, duk_size_t newsize) {
	void *res;
	DUK_UNREF(udata);
	res = ps_realloc(ptr, newsize);
	return res;
}

void ps_free_function(void *udata, void *ptr) {
	DUK_UNREF(udata);
	DUK_ANSI_FREE(ptr);
}

// Code interpreter, must be called in the loop() function to work
bool interpreter() {
        tft.fillScreen(TFT_BLACK);
        tft.setRotation(bruceConfig.rotation);
        tft.setTextSize(FM);
        tft.setTextColor(TFT_WHITE);
        // Create context.
        auto alloc_function = &ps_alloc_function;
        auto realloc_function = &ps_realloc_function;
        auto free_function = &ps_free_function;
        if (!psramFound()) {
          alloc_function = NULL;
          realloc_function = NULL;
          free_function = NULL;
        }

        duk_context *ctx = duk_create_heap(
          alloc_function,
          realloc_function,
          free_function,
          NULL,
          NULL
        );

        // Init containers
        clearGifsVector();

        // Add native functions to context.
        registerLightFunction(ctx, "now", native_now, 0);
        registerLightFunction(ctx, "delay", native_delay, 1);
        registerLightFunction(ctx, "parse_int", native_parse_int, 1);
        registerLightFunction(ctx, "to_string", native_to_string, 1);
        registerLightFunction(ctx, "to_hex_string", native_to_hex_string, 1);
        registerLightFunction(ctx, "to_lower_case", native_to_lower_case, 1);
        registerLightFunction(ctx, "to_upper_case", native_to_upper_case, 1);
        registerConsole(ctx);
        registerString(ctx, "__filepath", (String(scriptDirpath) + String(scriptName)).c_str());
        registerString(ctx, "__dirpath", scriptDirpath);

        // Arduino compatible
        registerLightFunction(ctx, "digitalWrite", native_digitalWrite, 2);
        registerLightFunction(ctx, "analogWrite", native_analogWrite, 2);
        registerLightFunction(ctx, "digitalRead", native_digitalRead, 1);
        registerLightFunction(ctx, "analogRead", native_analogRead, 1);
        registerLightFunction(ctx, "pinMode", native_pinMode, 2);
        registerInt(ctx, "HIGH", HIGH);
        registerInt(ctx, "LOW", LOW);
        registerInt(ctx, "INPUT", INPUT);
        registerInt(ctx, "OUTPUT", OUTPUT);
        registerInt(ctx, "PULLUP", PULLUP);
        registerInt(ctx, "INPUT_PULLUP", INPUT_PULLUP);
        registerInt(ctx, "PULLDOWN", PULLDOWN);
        registerInt(ctx, "INPUT_PULLDOWN", INPUT_PULLDOWN);

        // Deprecated
        registerLightFunction(ctx, "load", native_load, 1);
        registerLightFunction(ctx, "random", native_random, 2);

        // Get Informations from the board
        registerLightFunction(ctx, "getBattery", native_getBattery, 0);
        registerLightFunction(ctx, "getBoard", native_getBoard, 0);
        registerLightFunction(ctx, "getFreeHeapSize", native_getFreeHeapSize, 0);

        // Networking
        registerLightFunction(ctx, "wifiConnect", native_wifiConnect, 3);
        registerLightFunction(ctx, "wifiConnectDialog", native_wifiConnectDialog, 0);
        registerLightFunction(ctx, "wifiDisconnect", native_wifiDisconnect, 0);
        registerLightFunction(ctx, "wifiScan", native_wifiScan, 0);
        registerLightFunction(ctx, "httpGet", native_get, 2);

        // Graphics
        registerLightFunction(ctx, "color", native_color, 3);
        registerLightFunction(ctx, "fillScreen", native_fillScreen, 1);
        registerLightFunction(ctx, "setTextColor", native_setTextColor, 1);
        registerLightFunction(ctx, "setTextSize", native_setTextSize, 1);
        registerLightFunction(ctx, "drawString", native_drawString, 3);
        registerLightFunction(ctx, "setCursor", native_setCursor, 2);
        registerLightFunction(ctx, "print", native_print, 1);
        registerLightFunction(ctx, "println", native_println, 1);
        registerLightFunction(ctx, "drawPixel", native_drawPixel, 3);
        registerLightFunction(ctx, "drawLine", native_drawLine, 5);
        registerLightFunction(ctx, "drawRect", native_drawRect, 5);
        registerLightFunction(ctx, "drawFillRect", native_drawFillRect, 5);
        // registerLightFunction(ctx, "drawBitmap", native_drawBitmap, 4);
        registerLightFunction(ctx, "drawJpg", native_drawJpg, 4);
        registerLightFunction(ctx, "drawGif", native_drawGif, 6);
        registerLightFunction(ctx, "gifOpen", native_gifOpen, 2);
        registerLightFunction(ctx, "width", native_width, 0);
        registerLightFunction(ctx, "height", native_height, 0);

        // Input
        registerLightFunction(ctx, "getKeysPressed", native_getKeysPressed, 0); // keyboard btns for cardputer (entry)
        registerLightFunction(ctx, "getPrevPress", native_getPrevPress, 0);
        registerLightFunction(ctx, "getSelPress", native_getSelPress, 0);
        registerLightFunction(ctx, "getNextPress", native_getNextPress, 0);
        registerLightFunction(ctx, "getAnyPress", native_getAnyPress, 0);

        // Serial
        registerLightFunction(ctx, "serialReadln", native_serialReadln, 0);
        registerLightFunction(ctx, "serialPrintln", native_serialPrintln, 6);
        registerLightFunction(ctx, "serialCmd", native_serialCmd, 1);

        // Audio
        registerLightFunction(ctx, "playAudioFile", native_playAudioFile, 1);
        registerLightFunction(ctx, "tone", native_tone, 2);

        // badusb
        registerLightFunction(ctx, "badusbSetup", native_badusbSetup, 0);
        registerLightFunction(ctx, "badusbPrint", native_badusbPrint, 1);
        registerLightFunction(ctx, "badusbPrintln", native_badusbPrintln, 1);
        registerLightFunction(ctx, "badusbPress", native_badusbPress, 1);
        registerLightFunction(ctx, "badusbHold", native_badusbHold, 1);
        registerLightFunction(ctx, "badusbRelease", native_badusbRelease, 1);
        registerLightFunction(ctx, "badusbReleaseAll", native_badusbReleaseAll, 0);
        registerLightFunction(ctx, "badusbPressRaw", native_badusbPressRaw, 1);
        registerLightFunction(ctx, "badusbRunFile", native_badusbRunFile, 1);
        //registerLightFunction(ctx, "badusbPressSpecial", native_badusbPressSpecial, 1);

        // IR
        registerLightFunction(ctx, "irRead", native_irRead, 0);
        registerLightFunction(ctx, "irReadRaw", native_irReadRaw, 0);
        registerLightFunction(ctx, "irTransmitFile", native_irTransmitFile, 1);
        // TODO: irTransmit(string)

        // subghz
        registerLightFunction(ctx, "subghzRead", native_subghzRead, 0);
        registerLightFunction(ctx, "subghzReadRaw", native_subghzReadRaw, 0);
        registerLightFunction(ctx, "subghzSetFrequency", native_subghzSetFrequency, 1);
        registerLightFunction(ctx, "subghzTransmitFile", native_subghzTransmitFile, 1);
        // registerLightFunction(ctx, "subghzSetIdle", native_subghzSetIdle, 1);
        // TODO: subghzTransmit(string)

        // Dialog
        registerLightFunction(ctx, "dialogMessage", native_dialogMessage, 1);
        registerLightFunction(ctx, "dialogError", native_dialogError, 1);
        // TODO: dialogYesNo()
        registerLightFunction(ctx, "dialogPickFile", native_dialogPickFile, 1);
        registerLightFunction(ctx, "dialogChoice", native_dialogChoice, 1);
        registerLightFunction(ctx, "dialogViewFile", native_dialogViewFile, 1);
        registerLightFunction(ctx, "keyboard", native_keyboard, 3);

        // Storage
        registerLightFunction(ctx, "storageRead", native_storageRead, 1);
        registerLightFunction(ctx, "storageWrite", native_storageWrite, 2);
        // TODO: wrap more serial storage cmd: mkdir, remove, ...


        // TODO: match flipper syntax https://github.com/jamisonderek/flipper-zero-tutorials/wiki/JavaScript
        // MEMO: API https://duktape.org/api.html  https://github.com/joeqread/arduino-duktape/blob/main/src/duktape.h

        bool r;

        duk_push_string(ctx, script);
        if (duk_peval(ctx) != 0) {
            tft.fillScreen(bruceConfig.bgColor);
            tft.setTextSize(FM);
            tft.setTextColor(TFT_RED, bruceConfig.bgColor);
            tft.drawCentreString("Error", tftWidth / 2, 10, 1);
            tft.setTextColor(TFT_WHITE, bruceConfig.bgColor);
            tft.setTextSize(FP);
            tft.setCursor(0, 33);
            tft.println(duk_safe_to_string(ctx, -1));

            printf("eval failed: %s\n", duk_safe_to_string(ctx, -1));
            r = false;

            delay(500);
            while(!check(AnyKeyPress));
        } else {
            printf("result is: %s\n", duk_safe_to_string(ctx, -1));
            r = true;
        }
        if (isScriptDynamic) {
          free((char *)script);
          free((char *)scriptDirpath);
          free((char *)scriptName);
          isScriptDynamic = false;
          script = "drawString('Something wrong.', 4, 4);";
          scriptDirpath = "";
          scriptName = "";
        }
        duk_pop(ctx);

        // Clean up.
        duk_destroy_heap(ctx);

        clearGifsVector();

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
            {"SD Card", [&](){ fs=&SD; }},
            {"LittleFS", [&](){ fs=&LittleFS; }},
        };
        loopOptions(options);
    }
    filename = loopSD(*fs,true,"BJS|JS");
    script = readScriptFile(*fs, filename);

    returnToMenu=true;
    interpreter_start=true;

    // To stop the script, press Prev and Next together for a few seconds
}

bool run_bjs_script_headless(const char *code) {
    script = code;
    scriptDirpath = strdup("");
    scriptName = strdup("");
    isScriptDynamic = true;
    returnToMenu=true;
    interpreter_start=true;
    return true;
}

bool run_bjs_script_headless(FS fs, String filename) {
    script = readScriptFile(fs, filename);
    const char *sName = filename.substring(0, filename.lastIndexOf('/')).c_str();
    const char *sDirpath = filename.substring(filename.lastIndexOf('/') + 1).c_str();
    scriptDirpath = strdup(sDirpath);
    scriptName = strdup(sName);
    isScriptDynamic = true;
    returnToMenu=true;
    interpreter_start=true;
    return true;
}
