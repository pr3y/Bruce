#include "dialog_js.h"
#include "core/scrollableTextArea.h"

#include "helpers_js.h"

// Dialog functions

duk_ret_t native_dialogMessage(duk_context *ctx) {
  // usage: dialogMessage(msg : string, waitKeyPress : boolean)
  if (duk_is_boolean(ctx, 1)) {
    displayInfo(String(duk_to_string(ctx, 0)), duk_to_boolean(ctx, 1));
  } else {
    displayInfo(String(duk_to_string(ctx, 0)));
  }
  return 0;
}

duk_ret_t native_dialogError(duk_context *ctx) {
  // usage: dialogError(msg : string, waitKeyPress : boolean)
  if (duk_is_boolean(ctx, 1)) {
    displayError(String(duk_to_string(ctx, 0)), duk_to_boolean(ctx, 1));
  } else {
    displayError(String(duk_to_string(ctx, 0)));
  }
  return 0;
}

duk_ret_t native_dialogPickFile(duk_context *ctx) {
  // usage: dialogPickFile(): string
  // usage: dialogPickFile(path: string | { path: string, filesystem?: string }): string
  // returns: selected file , empty string if cancelled
  String r = "";
  String filepath = "/";
  String extension = "*";
  if (duk_is_string(ctx, 0)) {
    filepath = duk_to_string(ctx, 0);
    if(!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
  }

  if (duk_is_string(ctx, 1)) {
    extension = duk_to_string(ctx, 1);
  }

  FS* fs = NULL;
  if(SD.exists(filepath)) fs = &SD;
  if(LittleFS.exists(filepath)) fs = &LittleFS;
  if(fs) {
    r = loopSD(*fs, true, extension, filepath);
  }
  duk_push_string(ctx, r.c_str());
  return 1;
}

duk_ret_t native_dialogChoice(duk_context *ctx) {
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
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "%s: Choice array elements must be strings.", "dialogChoice");
            }

            // Get the string
            const char *choiceKey = duk_get_string(ctx, -1);
            duk_pop(ctx);
            i++;
            duk_get_prop_index(ctx, 0, i);

            // Ensure it's a string
            if (!duk_is_string(ctx, -1)) {
                duk_pop(ctx);
                duk_error(ctx, DUK_ERR_TYPE_ERROR, "%s: Choice array elements must be strings.", "dialogChoice");
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

duk_ret_t native_dialogViewFile(duk_context *ctx) {
  // usage: dialogViewFile(path: string)
  // returns: nothing
  if(!duk_is_string(ctx, 0)) {
    return 0;
  }

  String filepath = duk_get_string(ctx, 0);
  if (!filepath.startsWith("/")) filepath = "/" + filepath;  // add "/" if missing
  FS* fs = NULL;
  if (SD.exists(filepath)) fs = &SD;
  if (LittleFS.exists(filepath)) fs = &LittleFS;
  if (fs) {
    viewFile(*fs, filepath);
  }
  return 0;
}

duk_ret_t native_dialogViewText(duk_context *ctx) {
  // usage: dialogViewText(text: string, title?: string)
  // returns: nothing
  if(!duk_is_string(ctx, 0)) {
    return 0;
  }
  tft.drawRoundRect(5, 5, tftWidth - 10, tftHeight - 10, 5, bruceConfig.priColor);

  uint8_t padY = 10;

  if(duk_is_string(ctx, 1)) {
    const char *title = duk_get_string(ctx, 1);
    tft.setCursor((tftWidth - (strlen(title) * FM * LW)) / 2, padY);
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    tft.setTextSize(FM);
    tft.println(title);
    padY = tft.getCursorY();
    tft.setTextSize(FP);
  }

  ScrollableTextArea area = ScrollableTextArea(
    1,
    10,
    padY,
    tftWidth - 2 * BORDER_PAD_X,
    tftHeight - BORDER_PAD_X - padY,
    false
  );
  area.fromString(duk_get_string(ctx, 0));

  area.show(true);
  return 0;
}

static ScrollableTextArea *getAreaPointer(duk_context *ctx) {
  ScrollableTextArea *area = NULL;
  duk_push_this(ctx);
  if (duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("areaPointer"))) {
    area = (ScrollableTextArea *)duk_to_pointer(ctx, -1);
  }
  return area;
}

duk_ret_t native_dialogCreateTextViewerDraw(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  area->draw(true);
  return 0;
}

duk_ret_t native_dialogCreateTextViewerScrollUp(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  area->scrollUp();
  return 0;
}

duk_ret_t native_dialogCreateTextViewerScrollDown(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  area->scrollDown();
  return 0;
}

duk_ret_t native_dialogCreateTextViewerGetVisibleText(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  String visibleText;
  visibleText.reserve(area->getMaxVisibleTextLength());

  for (size_t i = area->firstVisibleLine; i < area->lastVisibleLine - 1; i++) {
    visibleText += area->linesBuffer[i];
  }
  duk_push_string(ctx, visibleText.c_str());
  return 1;
}

duk_ret_t native_dialogCreateTextViewerClose(duk_context *ctx) {
  ScrollableTextArea *area = NULL;

  if (duk_is_object(ctx, 0)) {
    duk_to_object(ctx, 0);
  } else {
    duk_push_this(ctx);
  }
  if (duk_get_prop_string(ctx, -1, "areaPointer")) {
    area = (ScrollableTextArea *)duk_get_pointer(ctx, -1);
    duk_pop(ctx);
    bduk_put_prop(ctx, -1, DUK_HIDDEN_SYMBOL("areaPointer"), duk_push_pointer, NULL);
  }
  if (area != NULL) {
    delete area;
  }
  return 0;
}

duk_ret_t native_dialogCreateTextViewer(duk_context *ctx) {
  if(!duk_is_string(ctx, 0)) {
    return 0;
  }

  duk_get_prop_string(ctx, 1, "fontSize");
  uint8_t fontSize = duk_get_uint_default(ctx, -1, 1);
  duk_get_prop_string(ctx, 1, "startX");
  int16_t startX = duk_get_uint_default(ctx, -1, 10);
  duk_get_prop_string(ctx, 1, "startY");
  int16_t startY = duk_get_uint_default(ctx, -1, 10);
  duk_get_prop_string(ctx, 1, "width");
  int32_t width = duk_get_uint_default(ctx, -1, tftWidth - 10);
  duk_get_prop_string(ctx, 1, "height");
  int32_t height = duk_get_uint_default(ctx, -1, tftHeight - 10);
  duk_pop_n(ctx, 5);

  ScrollableTextArea *area = new ScrollableTextArea(
    fontSize,
    startX,
    startY,
    width,
    height,
    false
  );
  area->fromString(duk_get_string(ctx, 0));

  duk_idx_t obj_idx = duk_push_object(ctx);
  bduk_put_prop(ctx, obj_idx, DUK_HIDDEN_SYMBOL("areaPointer"), duk_push_pointer, area);

  bduk_put_prop_c_lightfunc(ctx, obj_idx, "draw", native_dialogCreateTextViewerDraw, 0, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "scrollUp", native_dialogCreateTextViewerScrollUp, 0, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "scrollDown", native_dialogCreateTextViewerScrollDown, 0, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "getVisibleText", native_dialogCreateTextViewerGetVisibleText, 0, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "close", native_dialogCreateTextViewerClose, 0, 0);

  duk_push_c_lightfunc(ctx, native_dialogCreateTextViewerClose, 1, 1, 0);
  duk_set_finalizer(ctx, obj_idx);

  return 1;
}
