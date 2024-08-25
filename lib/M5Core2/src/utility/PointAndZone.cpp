#if defined (CORE2)
#include "PointAndZone.h"

// Point class

Point::Point(int16_t x_ /* = INVALID_VALUE */,
             int16_t y_ /* = INVALID_VALUE */) {
    x = x_;
    y = y_;
}

bool Point::operator==(const Point& p) {
    return (Equals(p));
}

bool Point::operator!=(const Point& p) {
    return (!Equals(p));
}

Point::operator char*() {
    if (valid()) {
        sprintf(_text, "(%d, %d)", x, y);
    } else {
        strncpy(_text, "(invalid)", 12);
    }
    return _text;
}

Point::operator bool() {
    return !(x == INVALID_VALUE && y == INVALID_VALUE);
}

void Point::set(int16_t x_ /* = INVALID_VALUE */,
                int16_t y_ /* = INVALID_VALUE */) {
    x = x_;
    y = y_;
}

bool Point::Equals(const Point& p) {
    return (x == p.x && y == p.y);
}

bool Point::in(Zone& z) {
    return (z.contains(x, y));
}

bool Point::valid() {
    return !(x == INVALID_VALUE && y == INVALID_VALUE);
}

uint16_t Point::distanceTo(const Point& p) {
    int16_t dx = x - p.x;
    int16_t dy = y - p.y;
    return sqrt(dx * dx + dy * dy) + 0.5;
}

uint16_t Point::directionTo(const Point& p, bool rot1 /* = false */) {
    // 57.29578 =~ 180/pi, see https://stackoverflow.com/a/62486304
    uint16_t a = (uint16_t)(450.5 + (atan2(p.y - y, p.x - x) * 57.29578));
#ifdef TFT
    if (rot1) {
        if (TFT->rotation < 4) {
            a = (((TFT->rotation + 3) % 4) * 90) + a;
        } else {
            a = (((TFT->rotation + 1) % 4) * 90) - a;
        }
    }
#endif /* TFT */
    a = (360 + a) % 360;
    return a;
}

bool Point::isDirectionTo(const Point& p, int16_t wanted,
                          uint8_t plusminus /* = PLUSMINUS */,
                          bool rot1 /* = false */) {
    uint16_t a = directionTo(p, rot1);
    return (min(abs(wanted - a), 360 - abs(wanted - a)) <= plusminus);
}

void Point::rotate(uint8_t m) {
    if (m == 1 || !valid()) return;
    int16_t normal_x = x;
    int16_t normal_y = y;
    int16_t inv_x    = HIGHEST_X - x;
    int16_t inv_y    = HIGHEST_Y - y;
    // inv_y can be negative for area below screen of M5Core2
    switch (m) {
        case 0:
            x = inv_y;
            y = normal_x;
            break;
        case 2:
            x = normal_y;
            y = inv_x;
            break;
        case 3:
            x = inv_x;
            y = inv_y;
            break;
        // rotations 4-7 are mirrored
        case 4:
            x = inv_y;
            y = inv_x;
            break;
        case 5:
            x = normal_x;
            y = inv_y;
            break;
        case 6:
            x = normal_y;
            y = normal_x;
            break;
        case 7:
            x = inv_x;
            y = normal_y;
            break;
    }
}

// Zone class

Zone::Zone(int16_t x_ /* = INVALID_VALUE */, int16_t y_ /* = INVALID_VALUE */,
           int16_t w_ /* = 0 */, int16_t h_ /* = 0 */, bool rot1_ /* = false */
) {
    set(x_, y_, w_, h_, rot1_);
}

Zone::operator bool() {
    return !(x == INVALID_VALUE && y == INVALID_VALUE);
}

void Zone::set(int16_t x_ /* = INVALID_VALUE */,
               int16_t y_ /* = INVALID_VALUE */, int16_t w_ /* = 0 */,
               int16_t h_ /* = 0 */, bool rot1_ /* = false */) {
    x    = x_;
    y    = y_;
    w    = w_;
    h    = h_;
    rot1 = rot1_;
}

bool Zone::valid() {
    return !(x == INVALID_VALUE && y == INVALID_VALUE);
}

bool Zone::contains(const Point& p) {
    return contains(p.x, p.y);
}

bool Zone::contains(int16_t x_, int16_t y_) {
#ifdef TFT
    if (rot1 && TFT->rotation != 1) {
        Zone t = *this;
        t.rotate(TFT->rotation);
        return (y_ >= t.y && y_ <= t.y + t.h && x_ >= t.x && x_ <= t.x + t.w);
    }
#endif /* TFT */

    return (y_ >= y && y_ <= y + h && x_ >= x && x_ <= x + w);
}

void Zone::rotate(uint8_t m) {
    if (m == 1) return;
    int16_t normal_x = x;
    int16_t normal_y = y;
    int16_t inv_x    = TFT_WIDTH - 1 - x - w;
    int16_t inv_y = TFT_HEIGHT - 1 - y - h;  // negative for area below screen
    switch (m) {
        case 0:
            x = inv_y;
            y = normal_x;
            break;
        case 2:
            x = normal_y;
            y = inv_x;
            break;
        case 3:
            x = inv_x;
            y = inv_y;
            break;
        // rotations 4-7 are mirrored
        case 4:
            x = inv_y;
            y = inv_x;
            break;
        case 5:
            x = normal_x;
            y = inv_y;
            break;
        case 6:
            x = normal_y;
            y = normal_x;
            break;
        case 7:
            x = inv_x;
            y = normal_y;
            break;
    }
    if (!(m % 2)) std::swap(w, h);
}
#endif