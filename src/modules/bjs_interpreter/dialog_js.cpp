#include "dialog_js.h"
#include "core/scrollableTextArea.h"

#include "helpers_js.h"

// Dialog functions

duk_ret_t native_dialogMessage(duk_context *ctx) {
  // usage: dialogMessage(msg : string, waitKeyPress : boolean)
  if (duk_is_boolean(ctx, 1)) {
    displayInfo(duk_to_string(ctx, 0)), duk_to_boolean(ctx, 1);
  } else {
    displayInfo(duk_to_string(ctx, 0));
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
  // usage: dialogChoice(choices : string[] | {[key: string]: string})
  // legacy version dialogChoice takes ["choice1", "return_val1", "choice2", "return_val2", ...]
  // new version dialog.choice takes ["choice1", "choice2" ...] or {"choice1": "return_val1", "choice2": "return_val2", ...}
  // returns: string ("return_val1", "return_val2", ...), or empty string if cancelled
  const char* result = "";
  duk_int_t legacy = duk_get_current_magic(ctx);

  duk_uint_t arg0Type = duk_get_type_mask(ctx, 0);
  if (!(arg0Type & (DUK_TYPE_MASK_OBJECT))) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "%s: Choice argument must be object or array.", "dialogChoice");
    return 1;
  }
  options = {};
  bool arg0IsArray = duk_is_array(ctx, 0);

  duk_enum(ctx, 0, 0);
  while (duk_next(ctx, -1, 1)) {
    const char *choiceKey = NULL;
    const char *choiceValue = duk_get_string(ctx, -1);
    if (!arg0IsArray) {
      choiceKey = duk_get_string(ctx, -2);
    } else {
      if (legacy) {
        duk_next(ctx, -1, 1);
      }
      if (duk_is_string(ctx, -1)) {
        choiceKey = duk_get_string(ctx, -1);
      } else {
        duk_error(ctx, DUK_ERR_TYPE_ERROR, "%s: Choice array elements must be strings.", "dialogChoice");
      }
    }
    choiceValue = duk_get_string(ctx, -1);
    duk_pop_2(ctx);
    options.push_back({choiceKey, [choiceValue, &result]() { result = choiceValue; }});
  }

  if (legacy) {
    options.push_back({"Cancel", [&]() { result = ""; }});
  }

  loopOptions(options);

  duk_push_string(ctx, result);
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

duk_ret_t native_dialogCreateTextViewerScrollToLine(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  area->scrollToLine(duk_get_int(ctx, 0));
  return 0;
}

duk_ret_t native_dialogCreateTextViewerGetLine(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  duk_push_string(ctx, area->getLine(duk_get_int(ctx, 0)).c_str());
  return 1;
}

duk_ret_t native_dialogCreateTextViewerGetMaxLines(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  duk_push_int(ctx, area->getMaxLines());
  return 1;
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

duk_ret_t native_dialogCreateTextViewerFromString(duk_context *ctx) {
  ScrollableTextArea *area = getAreaPointer(ctx);
  if (area == NULL) {
    return duk_error(ctx, DUK_ERR_ERROR, "%s: does not exist", "TextViewer");
  }
  area->fromString(duk_get_string(ctx, 0));
  return 1;
}

duk_ret_t native_dialogCreateTextViewerClose(duk_context *ctx) {
  ScrollableTextArea *area = NULL;

  if (duk_is_object(ctx, 0)) {
    duk_to_object(ctx, 0);
  } else {
    duk_push_this(ctx);
  }

  if (duk_get_prop_string(ctx, -1, DUK_HIDDEN_SYMBOL("areaPointer"))) {
    area = (ScrollableTextArea *)duk_get_pointer(ctx, -1);
    duk_pop(ctx);
    bduk_put_prop(ctx, 0, DUK_HIDDEN_SYMBOL("areaPointer"), duk_push_pointer, NULL);
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
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "scrollToLine", native_dialogCreateTextViewerScrollToLine, 1, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "getLine", native_dialogCreateTextViewerGetLine, 1, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "getMaxLines", native_dialogCreateTextViewerGetMaxLines, 0, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "getVisibleText", native_dialogCreateTextViewerGetVisibleText, 0, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "setText", native_dialogCreateTextViewerFromString, 1, 0);
  bduk_put_prop_c_lightfunc(ctx, obj_idx, "close", native_dialogCreateTextViewerClose, 0, 0);

  duk_push_c_lightfunc(ctx, native_dialogCreateTextViewerClose, 1, 1, 0);
  duk_set_finalizer(ctx, obj_idx);

  return 1;
}
