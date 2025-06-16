#ifndef __DISPLAY_LOGER
#define __DISPLAY_LOGER
#include <TFT_eSPI.h>
#include <vector>
String inline escapeJsonString(const String &str) {
    String out;
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str[i];
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

void inline appendJsonEntry(String &out, const String &key, const String &value, bool quote = true) {
    out += "\"" + key + "\":";
    if (quote) out += "\"" + escapeJsonString(value) + "\"";
    else out += value;
}

template <typename T> inline String toStringValue(const T &val) { return String(val); }

// dealing with char* and Strings
template <> inline String toStringValue<const char *>(const char *const &val) { return String(val); }

template <> inline String toStringValue<String>(const String &val) { return val; }

// base recursive function
void inline buildJson(String &) {}

// recursive function
template <typename T, typename V, typename... Rest>
void inline buildJson(String &out, const T &key, const V &value, const Rest &...rest) {
    if (out.length() > 1) out += ","; // comma between pairs
    appendJsonEntry(
        out,
        String(key),
        toStringValue(value),
        std::is_convertible<V, String>::value || std::is_convertible<V, const char *>::value
    );
    buildJson(out, rest...); // recursive call
}

template <typename... Args> inline String makeJson(const Args &...args) {
    static_assert(
        sizeof...(args) % 2 == 0, "makeJson requires an even number of arguments (key-value pairs)"
    );
    String result = "{";
    buildJson(result, args...);
    result += "}";
    return result;
}
enum tftFuncs {       // DO NOT CHANGE THE ORDER, ADD NEW FUNCTIONS TO THE END!!!
    FILLSCREEN,       // 0
    DRAWRECT,         // 1
    FILLRECT,         // 2
    DRAWROUNDRECT,    // 3
    FILLROUNDRECT,    // 4
    DRAWCIRCLE,       // 5
    FILLCIRCLE,       // 6
    DRAWTRIAGLE,      // 7
    FILLTRIANGLE,     // 8
    DRAWELIPSE,       // 9
    FILLELIPSE,       // 10
    DRAWLINE,         // 11
    DRAWARC,          // 12
    DRAWWIDELINE,     // 13
    DRAWCENTRESTRING, // 14
    DRAWRIGHTSTRING,  // 15
    DRAWSTRING,       // 16
    PRINT,            // 17
    DRAWIMAGE,        // 18
    DRAWPIXEL,        // 19
    DRAWFASTVLINE,    // 20
    DRAWFASTHLINE,    // 21
    // Add new ones here

    SCREEN_INFO = 99 // 99
};

struct tftLog {
    tftFuncs function;
    String info;
};
class tft_logger : public TFT_eSPI {
private:
    std::vector<tftLog> log;
    bool logging = false;
    bool _logging = false;
    void inline addScreenInfo() {
        tftLog l;
        l.function = SCREEN_INFO;
        l.info = makeJson("width", width(), "height", height(), "rotation", rotation);
        pushLogIfUnique(l);
    }

public:
    void inline setLogging(bool _log = true) {
        _logging = logging = _log;
        log.clear();
        if (_log) addScreenInfo();
    };
    bool inline getLogging(void) { return logging; };
    String inline getJSONLog() {
        String response = "[";
        if (log.size() == 0) {
            response += "]";
            return response;
        }
        int i = 0;
        for (auto l : log) {
            response += "{\"fn\":" + String(int(l.function)) + ", \"in\":" + String(l.info) + "}";
            i++;
            if (i != log.size()) response += ",";
        }
        response += "]";
        return response;
    }
    void inline fillScreen(uint32_t color) {
        if (logging) {
            log.clear();
            addScreenInfo();
        }
        TFT_eSPI::fillScreen(color);
    }
    void inline removeLogEntriesInsideRect(int rx, int ry, int rw, int rh) {
        int rx1 = rx;
        int ry1 = ry;
        int rx2 = rx + rw;
        int ry2 = ry + rh;

        log.erase(
            std::remove_if(
                log.begin(),
                log.end(),
                [&](const tftLog &entry) {
                    // extracts de x/y
                    int x = entry.info.indexOf("\"x\":") >= 0
                                ? entry.info.substring(entry.info.indexOf("\"x\":") + 4).toInt()
                                : -9999;
                    int y = entry.info.indexOf("\"y\":") >= 0
                                ? entry.info.substring(entry.info.indexOf("\"y\":") + 4).toInt()
                                : -9999;

                    // check for invalid coordinates, ignores
                    if (x == -9999 || y == -9999) return false;

                    // return x,y if they ar inside the rectangle area to delete
                    return x >= rx1 && x < rx2 && y >= ry1 && y < ry2;
                }
            ),
            log.end()
        );
    }
    void inline removeOverlappedImages(int x, int y, int center, int ms) {
        log.erase(
            std::remove_if(
                log.begin(),
                log.end(),
                [&](const tftLog &entry) {
                    if (entry.function != DRAWIMAGE) return false;

                    bool matchX = entry.info.indexOf("\"x\":" + String(x)) >= 0;
                    bool matchY = entry.info.indexOf("\"y\":" + String(y)) >= 0;
                    bool matchC = entry.info.indexOf("\"center\":" + String(center)) >= 0;
                    bool matchMs = entry.info.indexOf("\"Ms\":" + String(ms)) >= 0 ||
                                   entry.info.indexOf("\"ms\":" + String(ms)) >= 0;

                    return matchX && matchY && matchC && matchMs;
                }
            ),
            log.end()
        );
    }

    void inline imageToJson(String fs, String file, int x, int y, bool center = false, int Ms = 0) {
        if (logging) {
            removeOverlappedImages(x, y, center, Ms);
            tftLog l;
            l.function = DRAWIMAGE;
            l.info = makeJson("fs", fs, "file", file, "x", x, "y", y, "center", center, "Ms", Ms);
            pushLogIfUnique(l);
        }
    }
    void inline drawLine(uint32_t x, uint32_t y, uint32_t x1, uint32_t y1, uint32_t color) {
        if (logging) {
            tftLog l;
            l.function = DRAWLINE;
            l.info = makeJson("x", x, "y", y, "x1", x1, "y1", y1, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawLine(x, y, x1, y1, color);
        if (_logging) logging = true;
    }
    void inline drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
        if (logging) {
            tftLog l;
            l.function = DRAWRECT;
            l.info = makeJson("x", x, "y", y, "w", w, "h", h, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawRect(x, y, w, h, color);
        if (_logging) logging = true;
    }

    void inline fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
        if (logging && w > 4 && h > 4) {
            removeLogEntriesInsideRect(x, y, w, h);
            tftLog l;
            l.function = FILLRECT;
            l.info = makeJson("x", x, "y", y, "w", w, "h", h, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::fillRect(x, y, w, h, color);
        if (_logging) logging = true;
    }

    void inline drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
        if (logging) {
            tftLog l;
            l.function = DRAWROUNDRECT;
            l.info = makeJson("x", x, "y", y, "w", w, "h", h, "r", r, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawRoundRect(x, y, w, h, r, color);
        if (_logging) logging = true;
    }

    void inline fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
        if (logging) {
            removeLogEntriesInsideRect(x, y, w, h);
            tftLog l;
            l.function = FILLROUNDRECT;
            l.info = makeJson("x", x, "y", y, "w", w, "h", h, "r", r, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::fillRoundRect(x, y, w, h, r, color);
        if (_logging) logging = true;
    }

    void inline drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
        if (logging) {
            tftLog l;
            l.function = DRAWCIRCLE;
            l.info = makeJson("x", x, "y", y, "r", r, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawCircle(x, y, r, color);
        if (_logging) logging = true;
    }

    void inline fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
        if (logging) {
            tftLog l;
            l.function = FILLCIRCLE;
            l.info = makeJson("x", x, "y", y, "r", r, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::fillCircle(x, y, r, color);
        if (_logging) logging = true;
    }

    void inline drawEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color) {
        if (logging) {
            tftLog l;
            l.function = DRAWELIPSE;
            l.info = makeJson("x", x, "y", y, "rx", rx, "ry", ry, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawEllipse(x, y, rx, ry, color);
        if (_logging) logging = true;
    }

    void inline fillEllipse(int16_t x, int16_t y, int32_t rx, int32_t ry, uint16_t color) {
        if (logging) {
            tftLog l;
            l.function = FILLELIPSE;
            l.info = makeJson("x", x, "y", y, "rx", rx, "ry", ry, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::fillEllipse(x, y, rx, ry, color);
        if (_logging) logging = true;
    }

    void inline drawTriangle(
        int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color
    ) {
        if (logging) {
            tftLog l;
            l.function = DRAWTRIAGLE;
            l.info = makeJson("x", x1, "y", y1, "x2", x2, "y2", y2, "x3", x3, "y3", y3, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawTriangle(x1, y1, x2, y2, x3, y3, color);
        if (_logging) logging = true;
    }

    void inline fillTriangle(
        int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color
    ) {
        if (logging) {
            tftLog l;
            l.function = FILLTRIANGLE;
            l.info = makeJson("x", x1, "y", y1, "x2", x2, "y2", y2, "x3", x3, "y3", y3, "fg", color);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::fillTriangle(x1, y1, x2, y2, x3, y3, color);
        if (_logging) logging = true;
    }
    void inline drawArc(
        int32_t x, int32_t y, int32_t r, int32_t ir, uint32_t startAngle, uint32_t endAngle,
        uint32_t fg_color, uint32_t bg_color, bool smoothArc = true
    ) {
        if (logging) {
            tftLog l;
            l.function = DRAWARC;
            l.info = makeJson(
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
            );
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawArc(x, y, r, ir, startAngle, endAngle, fg_color, bg_color, smoothArc);
        if (_logging) logging = true;
    }

    void inline drawWideLine(
        float ax, float ay, float bx, float by, float wd, uint32_t fg, uint32_t bg = 0x00FFFFFF
    ) {
        if (logging) {
            tftLog l;
            l.function = DRAWWIDELINE;
            l.info = makeJson("x", ax, "y", ay, "bx", bx, "by", by, "wd", wd, "fg", fg, "bg", bg);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawWideLine(ax, ay, bx, by, wd, fg, bg);
        if (_logging) logging = true;
    }

    void inline drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t fg) {
        bool _lg = logging;
        logging = false;
        if (_lg) {
            tftLog l;
            l.function = DRAWFASTVLINE;
            l.info = makeJson("x", x, "y", y, "h", h, "fg", fg);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawFastVLine(x, y, h, fg);
        if (_lg) logging = true;
    }
    void inline drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t fg) {
        bool _lg = logging;
        logging = false;
        if (_lg) {
            tftLog l;
            l.function = DRAWFASTHLINE;
            l.info = makeJson("x", x, "y", y, "w", w, "fg", fg);
            pushLogIfUnique(l);
            logging = false;
        }
        TFT_eSPI::drawFastHLine(x, y, w, fg);
        if (_lg) logging = true;
    }

    void log_drawString(String s, tftFuncs fn, int32_t x, int32_t y) {
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

    int16_t inline drawString(const String &string, int32_t x, int32_t y, uint8_t font = 1) {
        log_drawString(string, DRAWSTRING, x, y);
        int16_t r = TFT_eSPI::drawString(string, x, y, font);
        if (_logging) logging = true;
        return r;
    }
    // int16_t inline drawString(const char *string, int32_t x, int32_t y, uint8_t font = 1) {
    //     log_drawString(string, DRAWSTRING, x, y);
    //     int16_t r = TFT_eSPI::drawString(string, x, y, font);
    //     if (_logging) logging = true;
    //     return r;
    // }

    int16_t inline drawCentreString(const String &string, int32_t x, int32_t y, uint8_t font = 1) {
        log_drawString(string, DRAWCENTRESTRING, x, y);
        int16_t r = TFT_eSPI::drawCentreString(string, x, y, font);
        if (_logging) logging = true;
        return r;
    }

    // int16_t inline drawCentreString(const char *string, int32_t x, int32_t y, uint8_t font = 1) {
    //     log_drawString(string, DRAWCENTRESTRING, x, y);
    //     int16_t r = TFT_eSPI::drawCentreString(string, x, y, font);
    //     if (_logging) logging = true;
    //     return r;
    // }

    int16_t inline drawRightString(const String &string, int32_t x, int32_t y, uint8_t font = 1) {
        log_drawString(string, DRAWRIGHTSTRING, x, y);
        int16_t r = TFT_eSPI::drawRightString(string, x, y, font);
        if (_logging) logging = true;
        return r;
    }
    // int16_t inline drawRightString(const char *string, int32_t x, int32_t y, uint8_t font = 1) {
    //     log_drawString(string, DRAWRIGHTSTRING, x, y);
    //     int16_t r = TFT_eSPI::drawRightString(string, x, y, font);
    //     if (_logging) logging = true;
    //     return r;
    // }

    void log_print(String s) {
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
    size_t inline println(void) {
        log_print("\n");
        return TFT_eSPI::println();
    }
    size_t inline println(const String &s) {
        log_print(String(s));
        return TFT_eSPI::println(s);
    }
    size_t inline println(char c) {
        log_print(String(c));
        return TFT_eSPI::println(c);
    }
    size_t inline println(unsigned char b, int base) {
        log_print(String(b, base));
        return TFT_eSPI::println(b, base);
    }
    size_t inline println(int n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::println(n, base);
    }
    size_t inline println(unsigned int n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::println(n, base);
    }
    size_t inline println(long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::println(n, base);
    }
    size_t inline println(unsigned long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::println(n, base);
    }
    size_t inline println(long long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::println(n, base);
    }
    size_t inline println(unsigned long long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::println(n, base);
    }
    size_t inline println(double n, int digits) {
        log_print(String(n, digits));
        return TFT_eSPI::println(n, digits);
    }

    size_t inline print(const String &s) {
        log_print(String(s));
        return TFT_eSPI::print(s);
    }
    size_t inline print(char c) {
        log_print(String(c));
        return TFT_eSPI::print(c);
    }
    size_t inline print(unsigned char b, int base) {
        log_print(String(b, base));
        return TFT_eSPI::print(b, base);
    }
    size_t inline print(int n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::print(n, base);
    }
    size_t inline print(unsigned int n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::print(n, base);
    }
    size_t inline print(long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::print(n, base);
    }
    size_t inline print(unsigned long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::print(n, base);
    }
    size_t inline print(long long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::print(n, base);
    }
    size_t inline print(unsigned long long n, int base) {
        log_print(String(n, base));
        return TFT_eSPI::print(n, base);
    }
    size_t inline print(double n, int digits) {
        log_print(String(n, digits));
        return TFT_eSPI::print(n, digits);
    }

    size_t printf(const char *format, ...) {
        va_list args;
        va_start(args, format);

        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);

        log_print(buf);

        return TFT_eSPI::print(buf);
    }

protected:
    bool isLogEqual(const tftLog &a, const tftLog &b) { return a.function == b.function && a.info == b.info; }

    void pushLogIfUnique(const tftLog &l) {
        for (const auto &entry : log) {
            if (isLogEqual(entry, l)) {
                return; // already exists, do nothing
            }
        }
        log.push_back(l); // new! add to the vector!!
    }
};

#endif //__DISPLAY_LOGER
