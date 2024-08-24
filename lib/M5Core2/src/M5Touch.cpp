#if defined (CORE2)
#include <M5Core2.h>

// M5Touch class

/* static */ M5Touch* M5Touch::instance;

M5Touch::M5Touch() {
    if (!instance) instance = this;
}

void M5Touch::begin() {
    Wire1.begin(21, 22);
    pinMode(CST_INT, INPUT);

    // By default, the FT6336 will pulse the INT line for every touch
    // event. But because it shares the Wire1 TwoWire/I2C with other
    // devices, we cannot easily create an interrupt service routine to
    // handle these events. So instead, we set the INT wire to polled mode,
    // so it simply goes low as long as there is at least one valid touch.
    ft6336(0xA4, 0x00);

    Serial.print("touch: ");
    if (interval(DEFAULT_INTERVAL) == DEFAULT_INTERVAL) {
        Serial.printf("FT6336 ready (fw id 0x%02X rel %d, lib 0x%02X%02X)\n",
                      ft6336(0xA6), ft6336(0xAF), ft6336(0xA1), ft6336(0xA2));
    } else {
        Serial.println("ERROR - FT6336 not responding");
    }
}

bool M5Touch::ispressed() {
    return (digitalRead(CST_INT) == LOW);
}

// Single register read and write

uint8_t M5Touch::ft6336(uint8_t reg) {
    Wire1.beginTransmission((uint8_t)CST_DEVICE_ADDR);
    Wire1.write(reg);
    Wire1.endTransmission();
    Wire1.requestFrom((uint8_t)CST_DEVICE_ADDR, uint8_t(1));
    return Wire1.read();
}

void M5Touch::ft6336(uint8_t reg, uint8_t value) {
    Wire1.beginTransmission(CST_DEVICE_ADDR);
    Wire1.write(reg);
    Wire1.write((uint8_t)value);
    Wire1.endTransmission();
}

// Reading size bytes into data
void M5Touch::ft6336(uint8_t reg, uint8_t size, uint8_t* data) {
    Wire1.beginTransmission((uint8_t)CST_DEVICE_ADDR);
    Wire1.write(reg);
    Wire1.endTransmission();
    Wire1.requestFrom((uint8_t)CST_DEVICE_ADDR, size);
    for (uint8_t i = 0; i < size; i++) data[i] = Wire1.read();
}

uint8_t M5Touch::interval(uint8_t ivl) {
    ft6336(0x88, ivl);
    return interval();
}

uint8_t M5Touch::interval() {
    _interval = ft6336(0x88);
    return _interval;
}

// This is normally called from update()
bool M5Touch::read() {
    // true if real read, not a "come back later"
    wasRead = false;

    // true is something actually changed on the touchpad
    changed = false;

    // Return immediately if read() is called more frequently than the
    // touch sensor updates. This prevents unnecessary I2C reads, and the
    // data can also get corrupted if reads are too close together.
    if (millis() - _lastRead < _interval) return false;
    _lastRead = millis();

    Point p[2];
    uint8_t pts = 0;
    uint8_t p0f = 0;
    if (ispressed()) {
        uint8_t data[11];
        ft6336(0x02, 11, data);
        pts = data[0];
        if (pts > 2) return false;
        if (pts) {
            // Read the data. Never mind trying to read the "weight" and
            // "size" properties or using the built-in gestures: they
            // are always set to zero.
            p0f    = (data[3] >> 4) ? 1 : 0;
            p[0].x = ((data[1] << 8) | data[2]) & 0x0fff;
            p[0].y = ((data[3] << 8) | data[4]) & 0x0fff;
            if (p[0].x >= TOUCH_W || p[0].y >= TOUCH_H) return false;
            if (pts == 2) {
                p[1].x = ((data[7] << 8) | data[8]) & 0x0fff;
                p[1].y = ((data[9] << 8) | data[10]) & 0x0fff;
                if (p[1].x >= TOUCH_W || p[1].y >= TOUCH_H) return false;
            }
        }
    }

#ifdef TFT
    p[0].rotate(TFT->rotation);
    p[1].rotate(TFT->rotation);
#endif /* TFT */

    if (p[0] != point[0] || p[1] != point[1]) {
        changed      = true;
        point[0]     = p[0];
        point[1]     = p[1];
        points       = pts;
        point0finger = p0f;
    }
    wasRead = true;
    return true;
}

Point M5Touch::getPressPoint() {
    read();
    if (point[0]) return point[0];
    return Point(-1, -1);  // -1, -1 is old API's definition of invalid
}

void M5Touch::update() {
    read();
}

void M5Touch::dump() {
    uint8_t data[256] = {0};
    ft6336(0x00, 0x80, data);
    ft6336(0x80, 0x80, data + 0x80);
    Serial.printf("\n     ");
    for (uint8_t i = 0; i < 16; i++) Serial.printf(".%1X ", i);
    Serial.printf("\n");
    for (uint16_t i = 0; i < 0x100; i++) {
        if (!(i % 16)) Serial.printf("\n%1X.   ", i / 16);
        Serial.printf("%02X ", data[i]);
    }
    Serial.printf("\n\n\n");
}

// HotZone class (for compatibility with older M5Core2 code)

HotZone::HotZone(int16_t x0_, int16_t y0_, int16_t x1_, int16_t y1_,
                 void (*fun_)() /* = nullptr */
                 )
    : Zone(x0_, y0_, x1_ - x0_, y1_ - y0_) {
    fun = fun_;
}

void HotZone::setZone(int16_t x0_, int16_t y0_, int16_t x1_, int16_t y1_,
                      void (*fun_)() /*= nullptr */
) {
    set(x0_, y0_, x1_ - x0_, y1_ - y0_);
    fun = fun_;
}

bool HotZone::inHotZone(Point& p) {
    return contains(p);
}

bool HotZone::inHotZoneDoFun(Point& p) {
    if (contains(p)) {
        if (fun) fun();
        return true;
    } else {
        return false;
    }
}
#endif