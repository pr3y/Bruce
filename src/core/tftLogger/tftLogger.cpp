#include <tftLogger.h>

/*
AUXILIARY FUNCTIONS TO CREATE THE JSONS
*/
String escapeJsonString(const String &str) {
    String out;
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

void appendJsonEntry(String &out, const String &key, const String &value, bool quote = true) {
    out += "\"" + key + "\":";
    if (quote) out += "\"" + escapeJsonString(value) + "\"";
    else out += value;
}

template <typename T> String toStringValue(const T &val) { return String(val); }

// dealing with char* and Strings
template <> String toStringValue<const char *>(const char *const &val) { return String(val); }

template <> String toStringValue<String>(const String &val) { return val; }

// base recursive function
void buildJson(String &) {}

// recursive function
template <typename T, typename V, typename... Rest>
void buildJson(String &out, const T &key, const V &value, const Rest &...rest) {
    if (out.length() > 1) out += ","; // comma between pairs
    appendJsonEntry(
        out,
        String(key),
        toStringValue(value),
        std::is_convertible<V, String>::value || std::is_convertible<V, const char *>::value
    );
    buildJson(out, rest...); // recursive call
}

template <typename... Args> String makeJson(const Args &...args) {
    static_assert(
        sizeof...(args) % 2 == 0, "makeJson requires an even number of arguments (key-value pairs)"
    );
    String result = "{";
    buildJson(result, args...);
    result += "}";
    return result;
}

/* TFT LOGGER FUNCTIONS */
tft_logger::~tft_logger() { log.clear(); }

bool tft_logger::isLogEqual(const tftLog &a, const tftLog &b) {
    return a.function == b.function && a.info == b.info;
}

void tft_logger::pushLogIfUnique(const tftLog &l) {
    for (const auto &entry : log) {
        if (isLogEqual(entry, l)) {
            return; // already exists, do nothing
        }
    }
    log.push_back(l); // new! add to the vector!!
}
void tft_logger::addScreenInfo() {
    tftLog l;
    l.function = SCREEN_INFO;
    l.info = makeJson("width", width(), "height", height(), "rotation", rotation);
    pushLogIfUnique(l);
}

void tft_logger::setLogging(bool _log) {
    _logging = logging = _log;
    log.clear();
    if (_log) addScreenInfo();
};

String tft_logger::getJSONLog() {
    String response = "[";
    if (log.size() == 0) {
        response += "]";
        return response;
    }
    int i = 0;
    for (auto l : log) {
        response += "{\"fn\":" + String(int(l.function)) + ", \"in\":" + l.info + "}";
        i++;
        if (i != log.size()) response += ",";
    }
    response += "]";
    return response;
}
void tft_logger::removeLogEntriesInsideRect(int rx, int ry, int rw, int rh) {
    int rx1 = rx;
    int ry1 = ry;
    int rx2 = rx + rw;
    int ry2 = ry + rh;

    size_t writeIndex = 0;
    for (size_t readIndex = 0; readIndex < log.size(); ++readIndex) {
        const tftLog &entry = log[readIndex];

        int x = entry.info.indexOf("\"x\":") >= 0
                    ? entry.info.substring(entry.info.indexOf("\"x\":") + 4).toInt()
                    : -9999;
        int y = entry.info.indexOf("\"y\":") >= 0
                    ? entry.info.substring(entry.info.indexOf("\"y\":") + 4).toInt()
                    : -9999;

        bool shouldRemove = (x != -9999 && y != -9999 && x >= rx1 && x < rx2 && y >= ry1 && y < ry2);

        if (!shouldRemove) {
            if (writeIndex != readIndex) { log[writeIndex] = std::move(log[readIndex]); }
            ++writeIndex;
        }
    }

    // remove the leftover tail
    if (writeIndex < log.size()) { log.erase(log.begin() + writeIndex, log.end()); }
}
void tft_logger::removeOverlappedImages(int x, int y, int center, int ms) {
    size_t writeIndex = 0;

    for (size_t readIndex = 0; readIndex < log.size(); ++readIndex) {
        const tftLog &entry = log[readIndex];

        if (entry.function != DRAWIMAGE) {
            if (writeIndex != readIndex) { log[writeIndex] = std::move(log[readIndex]); }
            ++writeIndex;
            continue;
        }

        bool matchX = entry.info.indexOf("\"x\":" + String(x)) >= 0;
        bool matchY = entry.info.indexOf("\"y\":" + String(y)) >= 0;
        bool matchC = entry.info.indexOf("\"center\":" + String(center)) >= 0;
        bool matchMs = entry.info.indexOf("\"Ms\":" + String(ms)) >= 0 ||
                       entry.info.indexOf("\"ms\":" + String(ms)) >= 0;

        bool shouldRemove = matchX && matchY && matchC && matchMs;

        if (!shouldRemove) {
            if (writeIndex != readIndex) { log[writeIndex] = std::move(log[readIndex]); }
            ++writeIndex;
        }
    }

    if (writeIndex < log.size()) { log.erase(log.begin() + writeIndex, log.end()); }
}
void tft_logger::fillScreen(uint32_t color) {
    if (logging) {
        log.clear();
        addScreenInfo();
    }
    BRUCE_TFT_DRIVER::fillScreen(color);
}

void tft_logger::checkAndLog(tftFuncs f, String s) {
    tftLog l;
    l.function = f;
    l.info = s;
    pushLogIfUnique(l);
    logging = false;
}
void tft_logger::restoreLogger() {
    if (_logging) logging = true;
}
void tft_logger::imageToJson(String fs, String file, int x, int y, bool center, int Ms) {
    if (logging) {
        removeOverlappedImages(x, y, center, Ms);
        checkAndLog(DRAWIMAGE, makeJson("fs", fs, "file", file, "x", x, "y", y, "center", center, "Ms", Ms));
        restoreLogger();
    }
}
void tft_logger::drawLine(uint32_t x, uint32_t y, uint32_t x1, uint32_t y1, uint32_t color) {
    if (logging) checkAndLog(DRAWLINE, makeJson("x", x, "y", y, "x1", x1, "y1", y1, "fg", color));
    BRUCE_TFT_DRIVER::drawLine(x, y, x1, y1, color);
    restoreLogger();
}
void tft_logger::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (logging) checkAndLog(DRAWRECT, makeJson("x", x, "y", y, "w", w, "h", h, "fg", color));
    BRUCE_TFT_DRIVER::drawRect(x, y, w, h, color);
    restoreLogger();
}

void tft_logger::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (logging && w > 4 && h > 4) {
        removeLogEntriesInsideRect(x, y, w, h);
        checkAndLog(FILLRECT, makeJson("x", x, "y", y, "w", w, "h", h, "fg", color));
    }
    BRUCE_TFT_DRIVER::fillRect(x, y, w, h, color);
    restoreLogger();
}

void tft_logger::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    if (logging) checkAndLog(DRAWROUNDRECT, makeJson("x", x, "y", y, "w", w, "h", h, "r", r, "fg", color));
    BRUCE_TFT_DRIVER::drawRoundRect(x, y, w, h, r, color);
    restoreLogger();
}

void tft_logger::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    if (logging) {
        removeLogEntriesInsideRect(x, y, w, h);
        checkAndLog(FILLROUNDRECT, makeJson("x", x, "y", y, "w", w, "h", h, "r", r, "fg", color));
    }
    BRUCE_TFT_DRIVER::fillRoundRect(x, y, w, h, r, color);
    restoreLogger();
}

void tft_logger::drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    if (logging) checkAndLog(DRAWCIRCLE, makeJson("x", x, "y", y, "r", r, "fg", color));
    BRUCE_TFT_DRIVER::drawCircle(x, y, r, color);
    restoreLogger();
}

void tft_logger::fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    if (logging) checkAndLog(FILLCIRCLE, makeJson("x", x, "y", y, "r", r, "fg", color));
    BRUCE_TFT_DRIVER::fillCircle(x, y, r, color);
    restoreLogger();
}

void tft_logger::drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color) {
    if (logging) checkAndLog(DRAWELIPSE, makeJson("x", x, "y", y, "rx", rx, "ry", ry, "fg", color));
    BRUCE_TFT_DRIVER::drawEllipse(x, y, rx, ry, color);
    restoreLogger();
}

void tft_logger::fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color) {
    if (logging) checkAndLog(FILLELIPSE, makeJson("x", x, "y", y, "rx", rx, "ry", ry, "fg", color));
    BRUCE_TFT_DRIVER::fillEllipse(x, y, rx, ry, color);
    restoreLogger();
}

void tft_logger::drawTriangle(
    int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color
) {
    if (logging)
        checkAndLog(
            DRAWTRIAGLE, makeJson("x", x1, "y", y1, "x2", x2, "y2", y2, "x3", x3, "y3", y3, "fg", color)
        );
    BRUCE_TFT_DRIVER::drawTriangle(x1, y1, x2, y2, x3, y3, color);
    restoreLogger();
}

void tft_logger::fillTriangle(
    int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color
) {
    if (logging)
        checkAndLog(
            FILLTRIANGLE, makeJson("x", x1, "y", y1, "x2", x2, "y2", y2, "x3", x3, "y3", y3, "fg", color)
        );
    BRUCE_TFT_DRIVER::fillTriangle(x1, y1, x2, y2, x3, y3, color);
    restoreLogger();
}
void tft_logger::drawArc(
    int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle, uint32_t fg_color,
    uint32_t bg_color, bool smoothArc
) {
    if (logging)
        checkAndLog(
            DRAWARC,
            makeJson(
                "x",
                x,
                "y",
                y,
                "r",
                r,
                "ir",
                ir,
                "startAngle",
                startAngle,
                "endAngle",
                endAngle,
                "fg",
                fg_color,
                "bg",
                bg_color
            )
        );
    BRUCE_TFT_DRIVER::drawArc(x, y, r, ir, startAngle, endAngle, fg_color, bg_color, smoothArc);
    restoreLogger();
}

void tft_logger::drawWideLine(float ax, float ay, float bx, float by, float wd, uint32_t fg, uint32_t bg) {
    if (logging)
        checkAndLog(
            DRAWWIDELINE, makeJson("x", ax, "y", ay, "bx", bx, "by", by, "wd", wd, "fg", fg, "bg", bg)
        );
    BRUCE_TFT_DRIVER::drawWideLine(ax, ay, bx, by, wd, fg, bg);
    restoreLogger();
}

void tft_logger::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t fg) {
    bool _lg = logging;
    logging = false;
    if (_lg) {
        tftLog l;
        l.function = DRAWFASTVLINE;
        l.info = makeJson("x", x, "y", y, "h", h, "fg", fg);
        pushLogIfUnique(l);
        logging = false;
    }
    BRUCE_TFT_DRIVER::drawFastVLine(x, y, h, fg);
    if (_lg) logging = true;
}
void tft_logger::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t fg) {
    bool _lg = logging;
    logging = false;
    if (_lg) {
        tftLog l;
        l.function = DRAWFASTHLINE;
        l.info = makeJson("x", x, "y", y, "w", w, "fg", fg);
        pushLogIfUnique(l);
        logging = false;
    }
    BRUCE_TFT_DRIVER::drawFastHLine(x, y, w, fg);
    if (_lg) logging = true;
}

void tft_logger::log_drawString(String s, tftFuncs fn, int32_t x, int32_t y) {
    if (logging) {
        removeLogEntriesInsideRect(
            x - 1, y - 1, s.length() * LW * textsize + 2, s.length() * LH * textsize + 2
        );
        s.replace("\n", "\\n");
        tftLog l;
        l.function = fn;
        l.info = makeJson("x", x, "y", y, "txt", s, "size", textsize, "fg", textcolor, "bg", textbgcolor);
        pushLogIfUnique(l);
        logging = false;
    }
}

int16_t tft_logger::drawString(const String &string, int32_t x, int32_t y, uint8_t font) {
    log_drawString(string, DRAWSTRING, x, y);
    int16_t r = BRUCE_TFT_DRIVER::drawString(string, x, y, font);
    restoreLogger();
    return r;
}

int16_t tft_logger::drawCentreString(const String &string, int32_t x, int32_t y, uint8_t font) {
    log_drawString(string, DRAWCENTRESTRING, x, y);
    int16_t r = BRUCE_TFT_DRIVER::drawCentreString(string, x, y, font);
    restoreLogger();
    return r;
}

int16_t tft_logger::drawRightString(const String &string, int32_t x, int32_t y, uint8_t font) {
    log_drawString(string, DRAWRIGHTSTRING, x, y);
    int16_t r = BRUCE_TFT_DRIVER::drawRightString(string, x, y, font);
    restoreLogger();
    return r;
}

void tft_logger::log_print(String s) {
    if (logging) {
        removeLogEntriesInsideRect(
            cursor_x - 1, cursor_y - 1, s.length() * LW * textsize + 2, s.length() * LH * textsize + 2
        );
        s.replace("\n", "\\n");
        tftLog l;
        l.function = PRINT;
        l.info = makeJson(
            "x", cursor_x, "y", cursor_y, "txt", s, "size", textsize, "fg", textcolor, "bg", textbgcolor
        );
        pushLogIfUnique(l);
    }
}

size_t tft_logger::println(void) {
    log_print("\n");
    return BRUCE_TFT_DRIVER::println();
}
size_t tft_logger::println(const String &s) {
    log_print(String(s));
    return BRUCE_TFT_DRIVER::println(s);
}
size_t tft_logger::println(char c) {
    log_print(String(c));
    return BRUCE_TFT_DRIVER::println(c);
}
size_t tft_logger::println(unsigned char b, int base) {
    log_print(String(b, base));
    return BRUCE_TFT_DRIVER::println(b, base);
}
size_t tft_logger::println(int n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::println(n, base);
}
size_t tft_logger::println(unsigned int n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::println(n, base);
}
size_t tft_logger::println(long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::println(n, base);
}
size_t tft_logger::println(unsigned long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::println(n, base);
}
size_t tft_logger::println(long long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::println(n, base);
}
size_t tft_logger::println(unsigned long long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::println(n, base);
}
size_t tft_logger::println(double n, int digits) {
    log_print(String(n, digits));
    return BRUCE_TFT_DRIVER::println(n, digits);
}

size_t tft_logger::print(const String &s) {
    log_print(String(s));
    return BRUCE_TFT_DRIVER::print(s);
}
size_t tft_logger::print(char c) {
    log_print(String(c));
    return BRUCE_TFT_DRIVER::print(c);
}
size_t tft_logger::print(unsigned char b, int base) {
    log_print(String(b, base));
    return BRUCE_TFT_DRIVER::print(b, base);
}
size_t tft_logger::print(int n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::print(n, base);
}
size_t tft_logger::print(unsigned int n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::print(n, base);
}
size_t tft_logger::print(long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::print(n, base);
}
size_t tft_logger::print(unsigned long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::print(n, base);
}
size_t tft_logger::print(long long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::print(n, base);
}
size_t tft_logger::print(unsigned long long n, int base) {
    log_print(String(n, base));
    return BRUCE_TFT_DRIVER::print(n, base);
}
size_t tft_logger::print(double n, int digits) {
    log_print(String(n, digits));
    return BRUCE_TFT_DRIVER::print(n, digits);
}

size_t tft_logger::printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    return print(buf);
}
