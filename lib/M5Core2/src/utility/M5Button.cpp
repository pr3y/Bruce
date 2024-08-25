#if defined (CORE2)
#include "M5Button.h"

// Button class

/* static */ std::vector<Button*> Button::instances;

Button::Button(int16_t x_, int16_t y_, int16_t w_, int16_t h_,
               bool rot1_ /* = false */, const char* name_ /* = "" */,
               ButtonColors off_ /*= {NODRAW, NODRAW, NODRAW} */,
               ButtonColors on_ /* = {NODRAW, NODRAW, NODRAW} */,
               uint8_t datum_ /* = BUTTON_DATUM */, int16_t dx_ /* = 0 */,
               int16_t dy_ /* = 0 */, uint8_t r_ /* = 0xFF */
               )
    : Zone(x_, y_, w_, h_, rot1_) {
    _pin    = 0xFF;
    _invert = false;
    _dbTime = 0;
    strncpy(_name, name_, 15);
    off   = off_;
    on    = on_;
    datum = datum_;
    dx    = dx_;
    dy    = dy_;
    r     = r_;
    init();
}

Button::Button(uint8_t pin_, uint8_t invert_, uint32_t dbTime_,
               String hw_ /* = "hw" */, int16_t x_ /* = 0 */,
               int16_t y_ /* = 0 */, int16_t w_ /* = 0 */, int16_t h_ /* = 0 */,
               bool rot1_ /* = false */, const char* name_ /* = "" */,
               ButtonColors off_ /*= {NODRAW, NODRAW, NODRAW} */,
               ButtonColors on_ /* = {NODRAW, NODRAW, NODRAW} */,
               uint8_t datum_ /* = BUTTON_DATUM */, int16_t dx_ /* = 0 */,
               int16_t dy_ /* = 0 */, uint8_t r_ /* = 0xFF */
               )
    : Zone(x_, y_, w_, h_, rot1_) {
    _pin    = pin_;
    _invert = invert_;
    _dbTime = dbTime_;
    strncpy(_name, name_, 15);
    off   = off_;
    on    = on_;
    datum = datum_;
    dx    = dx_;
    dy    = dy_;
    r     = r_;
    init();
}

Button::~Button() {
    for (int i = 0; i < instances.size(); ++i) {
        if (instances[i] == this) {
            BUTTONS->delHandlers(nullptr, this, nullptr);
            instances.erase(instances.begin() + i);
            return;
        }
    }
}

Button::operator bool() {
    return _state;
}

bool Button::operator==(const Button& b) {
    return (this == &b);
}
bool Button::operator!=(const Button& b) {
    return (this != &b);
}
bool Button::operator==(Button* b) {
    return (this == b);
}
bool Button::operator!=(Button* b) {
    return (this != b);
}

void Button::init() {
    _state = _tapWait = _pressing = _manuallyRead = false;
    _time = _lastChange = _pressTime = millis();
    _hold_time                       = -1;
    _textFont = _textSize = 0;
    //_freeFont             = nullptr;
    drawFn                = nullptr;
    _compat               = 0;
    drawZone              = Zone();
    tapTime               = TAP_TIME;
    dbltapTime            = DBLTAP_TIME;
    longPressTime         = LONGPRESS_TIME;
    repeatDelay           = REPEAT_DELAY;
    repeatInterval        = REPEAT_INTERVAL;
    strncpy(_label, _name, 16);
    if (_pin != 0xFF) pinMode(_pin, INPUT_PULLUP);
    instances.push_back(this);
    draw();
}

int16_t Button::instanceIndex() {
    for (int16_t i = 0; i < instances.size(); ++i) {
        if (instances[i] == this) return i;
    }
    return -1;
}

bool Button::read(bool manualRead /* = true */) {
    if (manualRead) _manuallyRead = true;
    event = Event();
    if (_changed) {
        _changed    = false;
        _lastChange = _time;
        if (!_state && !_cancelled && postReleaseEvents()) return _state;
    } else {
        if (!_cancelled && timeoutEvents()) return _state;
        if (!_state) _cancelled = false;
    }
    // Do actual read from the pin if this is a hardware button
    _time            = millis();
    uint8_t newState = false;
    if (_pin != 0xFF) {
        newState = (digitalRead(_pin));
        newState = _invert ? !newState : newState;
        if (newState != _state && _time - _lastChange >= _dbTime) {
            if (newState) fingerDown();
            if (!newState) fingerUp();
        }
    }
    return _state;
}

void Button::fingerDown(Point pos /* = Point() */, uint8_t finger /* = 0 */) {
    _finger            = finger;
    _currentPt[finger] = _fromPt[finger] = pos;
    if (!_state && !_currentPt[1 - finger]) {
        // other finger not here
        _state      = true;
        _changed    = true;
        _pressTime  = _time;
        _lastChange = _time;
        draw();
    }
    BUTTONS->fireEvent(finger, E_TOUCH, pos, pos, 0, this, nullptr);
}

void Button::fingerUp(uint8_t finger /* = 0  */) {
    uint32_t duration  = _time - _pressTime;
    _finger            = finger;
    _toPt[finger]      = _currentPt[finger];
    _currentPt[finger] = Point();
    if (_state && !_currentPt[1 - finger]) {
        // other finger not here
        _state      = false;
        _changed    = true;
        _lastChange = _time;
        draw();
    }
    BUTTONS->fireEvent(finger, E_RELEASE, _fromPt[finger], _toPt[finger],
                       duration, this, nullptr);
}

void Button::fingerMove(Point pos, uint8_t finger) {
    BUTTONS->fireEvent(finger, E_MOVE, _currentPt[finger], pos,
                       _time - _lastChange, this, nullptr);
    _currentPt[finger] = pos;
}

bool Button::postReleaseEvents() {
    uint32_t duration = _time - _pressTime;
    if (_toPt[_finger] && !contains(_toPt[_finger])) {
        BUTTONS->fireEvent(_finger, E_DRAGGED, _fromPt[_finger], _toPt[_finger],
                           duration, this, nullptr);
        _tapWait      = false;
        _pressing     = false;
        _longPressing = false;
        return true;
    }
    if (duration <= tapTime) {
        if (_tapWait) {
            BUTTONS->fireEvent(_finger, E_DBLTAP, _fromPt[_finger],
                               _toPt[_finger], duration, this, nullptr);
            _tapWait      = false;
            _pressing     = false;
            _longPressing = false;
            return true;
        }
        _tapWait = true;
    } else if (_pressing) {
        BUTTONS->fireEvent(_finger, _longPressing ? E_LONGPRESSED : E_PRESSED,
                           _fromPt[_finger], _toPt[_finger], duration, this,
                           nullptr);
        _pressing     = false;
        _longPressing = false;
        return true;
    }
    return false;
}

bool Button::timeoutEvents() {
    uint32_t duration = _time - _pressTime;
    if (_tapWait && duration >= dbltapTime) {
        BUTTONS->fireEvent(_finger, E_TAP, _fromPt[_finger], _toPt[_finger],
                           duration, this, nullptr);
        _tapWait  = false;
        _pressing = false;
        return true;
    }
    if (!_state) return false;
    if ((!_pressing && duration > tapTime) ||
        (repeatDelay && duration > repeatDelay &&
         _time - _lastRepeat > repeatInterval)) {
        BUTTONS->fireEvent(_finger, E_PRESSING, _fromPt[_finger],
                           _currentPt[_finger], duration, this, nullptr);
        _lastRepeat = _time;
        _pressing   = true;
        return true;
    }
    if (longPressTime && !_longPressing && duration > longPressTime) {
        BUTTONS->fireEvent(_finger, E_LONGPRESSING, _fromPt[_finger],
                           _currentPt[_finger], duration, this, nullptr);
        _longPressing = true;
        return true;
    }
    return false;
}

void Button::cancel() {
    _cancelled = true;
    _tapWait   = false;
    draw(off);
}

char* Button::getName() {
    return _name;
}

bool Button::isPressed() {
    return _state;
}

bool Button::isReleased() {
    return !_state;
}

bool Button::wasPressed() {
    return _state && _changed;
}

bool Button::wasReleased() {
    return (!_state && _changed && millis() - _pressTime < _hold_time);
}

bool Button::wasReleasefor(uint32_t ms) {
    _hold_time = ms;
    return (!_state && _changed && millis() - _pressTime >= ms);
}

bool Button::pressedFor(uint32_t ms) {
    return (_state && _time - _lastChange >= ms) ? 1 : 0;
}

bool Button::pressedFor(uint32_t ms, uint32_t continuous_time) {
    if (_state && _time - _lastChange >= ms &&
        _time - _lastLongPress >= continuous_time) {
        _lastLongPress = _time;
        return true;
    }
    return false;
}

bool Button::releasedFor(uint32_t ms) {
    return (!_state && _time - _lastChange >= ms);
}

uint32_t Button::lastChange() {
    return (_lastChange);
}

void Button::addHandler(EventHandlerCallback fn,
                        uint16_t eventMask /* = E_ALL */) {
    BUTTONS->addHandler(fn, eventMask, this, nullptr);
}

void Button::delHandlers(EventHandlerCallback fn /* = nullptr */) {
    BUTTONS->delHandlers(fn, this, nullptr);
}

// visual things for Button

void Button::draw() {
    if (_state)
        draw(on);
    else
        draw(off);
}

void Button::erase(uint16_t color /* = BLACK */) {
    draw({color, NODRAW, NODRAW});
}

void Button::draw(ButtonColors bc) {
    _hidden = false;
    // use locally set draw function if aplicable, global one otherwise
    if (drawFn) {
        drawFn(*this, bc);
    } else if (BUTTONS->drawFn) {
        BUTTONS->drawFn(*this, bc);
    }
}

void Button::hide(uint16_t color /* = NODRAW */) {
    _hidden = true;
    if (color != NODRAW) erase(color);
}

char* Button::label() {
    return _label;
}

void Button::setLabel(const char* label_) {
    strncpy(_label, label_, 50);
}
/*
void Button::setFont(const GFXfont* freeFont_) {
    //_freeFont = freeFont_;
    _textFont = 1;
}
*/
void Button::setFont(uint8_t textFont_ /* = 0 */) {
    //_freeFont = nullptr;
    _textFont = textFont_;
}

void Button::setTextSize(uint8_t textSize_ /* = 0 */) {
    _textSize = textSize_;
}

// M5Buttons class

/* static */ M5Buttons* M5Buttons::instance;

/* static */ void M5Buttons::drawFunction(Button& b, ButtonColors bc) {
    if (bc.bg == NODRAW && bc.outline == NODRAW && bc.text == NODRAW) return;
    Zone z = (b.drawZone) ? b.drawZone : b;
    if (z.rot1) z.rotate(TFT->rotation);

    uint8_t r = (b.r == 0xFF) ? min(z.w, z.h) / 4 : b.r;

    if (bc.bg != NODRAW) {
        if (r >= 2) {
            TFT->fillRoundRect(z.x, z.y, z.w, z.h, r, bc.bg);
        } else {
            TFT->fillRect(z.x, z.y, z.w, z.h, bc.bg);
        }
    }

    if (bc.outline != NODRAW) {
        if (r >= 2) {
            TFT->drawRoundRect(z.x, z.y, z.w, z.h, r, bc.outline);
        } else {
            TFT->drawRect(z.x, z.y, z.w, z.h, bc.outline);
        }
    }

    if (bc.text != NODRAW && bc.text != bc.bg && strlen(b._label)) {
        // figure out where to put the text
        uint16_t tx, ty;
        tx = z.x + (z.w / 2);
        ty = z.y + (z.h / 2);

        if (!b._compat) {
            uint8_t margin = max(r / 2, 6);
            switch (b.datum) {
                case TL_DATUM:
                case ML_DATUM:
                case BL_DATUM:
                    tx = z.x + margin;
                    break;
                case TR_DATUM:
                case MR_DATUM:
                case BR_DATUM:
                    tx = z.x + z.w - margin;
                    break;
            }
            switch (b.datum) {
                case TL_DATUM:
                case TC_DATUM:
                case TR_DATUM:
                    ty = z.y + margin;
                    break;
                case BL_DATUM:
                case BC_DATUM:
                case BR_DATUM:
                    ty = z.y + z.h - margin;
                    break;
            }
        }

        // Save state
        uint8_t tempdatum    = TFT->getTextDatum();
        uint16_t tempPadding = TFT->padX;
        if (!b._compat) TFT->pushState();

        // Actual drawing of text
        TFT->setTextColor(bc.text);
        if (b._textSize)
            TFT->setTextSize(b._textSize);
        else
            TFT->setTextSize(BUTTONS->_textSize);
        /*
        if (b._textFont) {
            if (b._freeFont)
                TFT->setFreeFont(b._freeFont);
            else
                TFT->setTextFont(b._textFont);
        } else {
            if (BUTTONS->_freeFont)
                TFT->setFreeFont(BUTTONS->_freeFont);
            else
                TFT->setTextFont(BUTTONS->_textFont);
        }
        */
        TFT->setTextDatum(b.datum);
        TFT->setTextPadding(0);
        TFT->drawString(b._label, tx + b.dx, ty + b.dy);
        // Set state back
        if (!b._compat) {
            TFT->popState();
        } else {
            TFT->setTextDatum(tempdatum);
            TFT->setTextPadding(tempPadding);
        }
    }
}

M5Buttons::M5Buttons() {
    if (!instance) instance = this;
    drawFn    = drawFunction;
    //_freeFont = BUTTON_FREEFONT;
    _textFont = BUTTON_TEXTFONT;
    _textSize = BUTTON_TEXTSIZE;
}

Button* M5Buttons::which(Point& p) {
    if (!Button::instances.size()) return nullptr;
    for (int i = Button::instances.size() - 1; i >= 0; --i) {
        Button* b = Button::instances[i];
        // Always return button when i == 0 --> background
        if (!i || (b->_pin == 0xFF && !b->_hidden && b->contains(p))) return b;
    }
    return nullptr;
}

void M5Buttons::draw() {
    for (auto button : Button::instances) button->draw();
}

void M5Buttons::update() {
#ifdef _M5TOUCH_H_
    for (auto gesture : Gesture::instances) gesture->_detected = false;
    BUTTONS->event = Event();
    if (TOUCH->wasRead || _leftovers) {
        _finger[TOUCH->point0finger].current     = TOUCH->point[0];
        _finger[1 - TOUCH->point0finger].current = TOUCH->point[1];
        _leftovers                               = true;
        for (uint8_t i = 0; i < 2; i++) {
            if (i == 1) _leftovers = false;
            Finger& fi  = _finger[i];
            Point& curr = fi.current;
            Point prev  = fi.previous;
            fi.previous = fi.current;
            if (curr == prev) continue;
            if (!prev && curr) {
                // A new touch happened
                fi.startTime  = millis();
                fi.startPoint = curr;
                fi.button     = BUTTONS->which(curr);
                if (fi.button) {
                    fi.button->fingerDown(curr, i);
                    return;
                }
            } else if (prev && !curr) {
                // Finger removed
                uint16_t duration = millis() - fi.startTime;
                for (auto gesture : Gesture::instances) {
                    if (gesture->test(fi.startPoint, prev, duration)) {
                        BUTTONS->fireEvent(i, E_GESTURE, fi.startPoint, prev,
                                           duration, nullptr, gesture);
                        if (fi.button) fi.button->cancel();
                        break;
                    }
                }
                if (fi.button) {
                    fi.button->fingerUp(i);
                    return;
                }
            } else {
                // Finger moved
                if (fi.button) {
                    fi.button->fingerMove(curr, i);
                    return;
                }
            }
        }
    }
#endif /* _M5TOUCH_H_ */

    for (auto button : Button::instances) {
        if (!button->_manuallyRead) button->read(false);
    }
}
/*
void M5Buttons::setFont(const GFXfont* freeFont_) {
    //_freeFont = freeFont_;
    _textFont = 1;
}
*/
void M5Buttons::setFont(uint8_t textFont_) {
    //_freeFont = nullptr;
    _textFont = textFont_;
}

void M5Buttons::setTextSize(uint8_t textSize_) {
    _textSize = textSize_;
}

void M5Buttons::fireEvent(uint8_t finger, uint16_t type, Point& from, Point& to,
                          uint16_t duration, Button* button, Gesture* gesture) {
    Event e;
    e.finger   = finger;
    e.type     = type;
    e.from     = from;
    e.to       = to;
    e.duration = duration;
    e.button   = button;
    e.gesture  = gesture;
    if (button) button->event = e;
    event = e;
    for (auto h : _eventHandlers) {
        if (!(h.eventMask & e.type)) continue;
        if (h.button && h.button != e.button) continue;
        if (h.gesture && h.gesture != e.gesture) continue;
        h.fn(e);
    }
}

void M5Buttons::addHandler(EventHandlerCallback fn,
                           uint16_t eventMask /* = E_ALL */,
                           Button* button /* = nullptr */,
                           Gesture* gesture /* = nullptr */
) {
    EventHandler handler;
    handler.fn        = fn;
    handler.eventMask = eventMask;
    handler.button    = button;
    handler.gesture   = gesture;
    _eventHandlers.push_back(handler);
}

void M5Buttons::delHandlers(EventHandlerCallback fn /* = nullptr */,
                            Button* button /* = nullptr */,
                            Gesture* gesture /* = nullptr */
) {
    for (int i = _eventHandlers.size() - 1; i >= 0; --i) {
        // this doesn't compile anymore
        // if (fn && fn != _eventHandlers[i].fn) continue;
        if (button && _eventHandlers[i].button != button) continue;
        if (gesture && _eventHandlers[i].gesture != gesture) continue;
        _eventHandlers.erase(_eventHandlers.begin() + i);
    }
}

// Gesture class

std::vector<Gesture*> Gesture::instances;

Gesture::Gesture(Zone fromZone_, Zone toZone_, const char* name_ /* = "" */,
                 uint16_t minDistance_ /* = GESTURE_MINDIST */,
                 int16_t direction_ /* = INVALID_VALUE */,
                 uint8_t plusminus_ /* = PLUSMINUS */, bool rot1_ /* = false */,
                 uint16_t maxTime_ /* = GESTURE_MAXTIME */
) {
    fromZone = fromZone_;
    toZone   = toZone_;
    strncpy(_name, name_, 15);
    minDistance = minDistance_;
    direction   = direction_;
    plusminus   = plusminus_;
    rot1        = rot1_;
    maxTime     = maxTime_;
    _detected   = false;
    instances.push_back(this);
}

Gesture::Gesture(const char* name_ /* = "" */,
                 uint16_t minDistance_ /* = GESTURE_MINDIST */,
                 int16_t direction_ /* = INVALID_VALUE */,
                 uint8_t plusminus_ /* = PLUSMINUS */, bool rot1_ /* = false */,
                 uint16_t maxTime_ /* = GESTURE_MAXTIME */
) {
    fromZone = ANYWHERE;
    toZone   = ANYWHERE;
    strncpy(_name, name_, 15);
    minDistance = minDistance_;
    direction   = direction_;
    plusminus   = plusminus_;
    rot1        = rot1_;
    maxTime     = maxTime_;
    _detected   = false;
    instances.push_back(this);
}

Gesture::~Gesture() {
    for (int i = 0; i < instances.size(); ++i) {
        if (instances[i] == this) {
            instances.erase(instances.begin() + i);
            BUTTONS->delHandlers(nullptr, nullptr, this);
            return;
        }
    }
}

Gesture::operator bool() {
    return _detected;
}

int16_t Gesture::instanceIndex() {
    for (int16_t i = 0; i < instances.size(); ++i) {
        if (instances[i] == this) return i;
    }
    return -1;
}

char* Gesture::getName() {
    return _name;
}

bool Gesture::test(Point& from, Point& to, uint16_t duration) {
    if (from.distanceTo(to) < minDistance) return false;
    if (fromZone && !fromZone.contains(from)) return false;
    if (toZone && !toZone.contains(to)) return false;
    if (direction != INVALID_VALUE &&
        !from.isDirectionTo(to, direction, plusminus, rot1))
        return false;
    if (duration > maxTime) return false;
    _detected = true;
    return true;
}

bool Gesture::wasDetected() {
    return _detected;
}

void Gesture::addHandler(EventHandlerCallback fn,
                         uint16_t eventMask /* = E_ALL */) {
    BUTTONS->addHandler(fn, eventMask, nullptr, this);
}

void Gesture::delHandlers(EventHandlerCallback fn /* = nullptr */) {
    BUTTONS->delHandlers(fn, nullptr, this);
}

// Event class

Event::Event() {
    finger = type = duration = 0;
    from = to = Point();
    button    = nullptr;
    gesture   = nullptr;
}

Event::operator uint16_t() {
    return type;
}

const char* Event::typeName() {
    const char* unknown                = "E_UNKNOWN";
    const char* none                   = "E_NONE";
    const char* eventNames[NUM_EVENTS] = {
        "E_TOUCH",    "E_RELEASE",     "E_MOVE",        "E_GESTURE",
        "E_TAP",      "E_DBLTAP",      "E_DRAGGED",     "E_PRESSED",
        "E_PRESSING", "E_LONGPRESSED", "E_LONGPRESSING"};
    if (!type) return none;
    for (uint8_t i = 0; i < NUM_EVENTS; i++) {
        if ((type >> i) & 1) return eventNames[i];
    }
    return unknown;
}

const char* Event::objName() {
    const char* empty = "";
    if (gesture) return gesture->getName();
    if (button) return button->getName();
    return empty;
};

uint16_t Event::direction(bool rot1 /* = false */) {
    return from.directionTo(to, rot1);
}

bool Event::isDirection(int16_t wanted, uint8_t plusminus /* = PLUSMINUS */,
                        bool rot1 /* = false */) {
    return from.isDirectionTo(to, wanted, plusminus, rot1);
}

uint16_t Event::distance() {
    return from.distanceTo(to);
}

// TFT_eSPI_Button2 compatibility mode

TFT_eSPI_Button2::TFT_eSPI_Button2() : Button(0, 0, 0, 0) {
    _compat = true;
}

void TFT_eSPI_Button2::initButton(TFT_eSPI* gfx, int16_t x, int16_t y,
                                 uint16_t w, uint16_t h, uint16_t outline,
                                 uint16_t fill, uint16_t textcolor,
                                 char* label_, uint8_t textsize) {
    initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor,
                 label_, textsize);
}

void TFT_eSPI_Button2::initButtonUL(TFT_eSPI* gfx, int16_t x_, int16_t y_,
                                   uint16_t w_, uint16_t h_, uint16_t outline,
                                   uint16_t fill, uint16_t textcolor,
                                   char* label_, uint8_t textsize_) {
    x   = x_;
    y   = y_;
    w   = w_;
    h   = h_;
    off = {fill, textcolor, outline};
    on  = {textcolor, fill, outline};
    setTextSize(textsize_);
    strncpy(_label, label_, 9);
}

void TFT_eSPI_Button2::setLabelDatum(int16_t dx_, int16_t dy_,
                                    uint8_t datum_ /* = MC_DATUM */) {
    dx    = dx_;
    dy    = dy_;
    datum = datum_;
}

void TFT_eSPI_Button2::drawButton(bool inverted /* = false */,
                                 String long_name /* = "" */) {
    char oldLabel[51];
    if (long_name != "") {
        strncpy(oldLabel, _label, 50);
        strncpy(_label, long_name.c_str(), 50);
    }
    draw(inverted ? on : off);
    if (long_name != "") strncpy(_label, oldLabel, 50);
}

bool TFT_eSPI_Button2::isPressed() {
    return currstate;
}

bool TFT_eSPI_Button2::contains(int16_t _x, int16_t _y) {
    return ((_x >= x) && (_x < (x + w)) && (_y >= y) && (_y < (y + h)));
}

void TFT_eSPI_Button2::press(bool p) {
    if (p)
        fingerDown();
    else
        fingerUp();
    laststate = currstate;
    currstate = p;
}

bool TFT_eSPI_Button2::justPressed() {
    return (currstate && !laststate);
}

bool TFT_eSPI_Button2::justReleased() {
    return (!currstate && laststate);
}
#endif