#include "interpreter.h"
#include "core/sd_functions.h"
#include "core/wifi_common.h"
#include "core/mykeyboard.h"


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
  Serial.print(duk_to_string(ctx, 0));
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
  delay(duk_to_number(ctx, 0));
  return 0;
}

// Hardware GPIO interactions 
static duk_ret_t native_digitalWrite(duk_context *ctx) {
  digitalWrite(duk_to_number(ctx, 0),duk_to_boolean(ctx, 1));
  return 0;
}

static duk_ret_t native_pinMode(duk_context *ctx) {
  pinMode(duk_to_number(ctx, 0),duk_to_number(ctx, 1));
  return 0;
}
// Get information from the board;
static duk_ret_t native_getBattery(duk_context *ctx) {
    int bat = getBattery();
    duk_push_int(ctx, bat);
    return 1;
}
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
static duk_ret_t native_wifiConnect(duk_context *ctx) {
    wifiConnectMenu();
    return 0;
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
  tft.setTextSize(duk_to_number(ctx, 0));
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

static duk_ret_t native_drawString(duk_context *ctx) {
  tft.drawString(duk_to_string(ctx, 0),duk_to_int(ctx, 1),duk_to_int(ctx, 2));
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
void interpreter() {
        if(!checkPrevPress() && !checkNextPress()) interpreter_start=false;
        tft.fillRect(0,0,WIDTH,HEIGHT,TFT_BLACK);
        tft.setRotation(rotation);
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

        // Get Informations from the board
        duk_push_c_function(ctx, native_getBattery, 0);
        duk_put_global_string(ctx, "getBattery");
        duk_push_c_function(ctx, native_getBoard, 0);
        duk_put_global_string(ctx, "getBoard");


        // Networking
        duk_push_c_function(ctx, native_wifiConnect, 0);
        duk_put_global_string(ctx, "wifiConnect");
        duk_push_c_function(ctx, native_wifiDisconnect, 0);
        duk_put_global_string(ctx, "wifiDisconnect");        
        duk_push_c_function(ctx, native_get, 2);
        duk_put_global_string(ctx, "httpGet");        

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
        duk_push_c_function(ctx, native_drawString, 3);
        duk_put_global_string(ctx, "drawString");
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

        duk_push_string(ctx, script.c_str());
        if (duk_peval(ctx) != 0) {
            printf("eval failed: %s\n", duk_safe_to_string(ctx, -1));
        } else {
            printf("result is: %s\n", duk_safe_to_string(ctx, -1));
        }
        duk_pop(ctx);

        // Clean up.
        duk_destroy_heap(ctx);

        delay(1000);
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

