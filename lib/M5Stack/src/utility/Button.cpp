#if defined (CORE)
/*----------------------------------------------------------------------*
 * Arduino Button Library v1.0                                          *
 * Jack Christensen May 2011, published Mar 2012                        *
 *                                                                      *
 * Library for reading momentary contact switches like tactile button   *
 * switches. Intended for use in state machine constructs.              *
 * Use the read() function to read all buttons in the main loop,        *
 * which should execute as fast as possible.                            *
 *                                                                      *
 * This work is licensed under the Creative Commons Attribution-        *
 * ShareAlike 3.0 Unported License. To view a copy of this license,     *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 *----------------------------------------------------------------------*/

#include "Button.h"

/*----------------------------------------------------------------------*
 * Button(pin, puEnable, invert, dbTime) instantiates a button object.  *
 * pin      Is the Arduino pin the button is connected to.              *
 * puEnable Enables the AVR internal pullup resistor if != 0 (can also  *
 *          use true or false).                                         *
 * invert   If invert == 0, interprets a high state as pressed, low as  *
 *          released. If invert != 0, interprets a high state as        *
 *          released, low as pressed  (can also use true or false).     *
 * dbTime   Is the debounce time in milliseconds.                       *
 *                                                                      *
 * (Note that invert cannot be implied from puEnable since an external  *
 *  pullup could be used.)                                              *
 *----------------------------------------------------------------------*/
Button::Button(uint8_t pin, uint8_t invert, uint32_t dbTime) {
    _pin    = pin;
    _invert = invert;
    _dbTime = dbTime;
    pinMode(_pin, INPUT_PULLUP);
    _state = digitalRead(_pin);
    if (_invert != 0) _state = !_state;
    _time       = millis();
    _lastState  = _state;
    _changed    = 0;
    _hold_time  = -1;
    _lastTime   = _time;
    _lastChange = _time;
    _pressTime  = _time;
}

/*----------------------------------------------------------------------*
 * read() returns the state of the button, 1==pressed, 0==released,     *
 * does debouncing, captures and maintains times, previous states, etc. *
 *----------------------------------------------------------------------*/
uint8_t Button::read(void) {
    static uint32_t ms;
    static uint8_t pinVal;

    ms     = millis();
    pinVal = digitalRead(_pin);
    if (_invert != 0) pinVal = !pinVal;
    if (ms - _lastChange < _dbTime) {
        _lastTime = _time;
        _time     = ms;
        _changed  = 0;
        return _state;
    } else {
        _lastTime  = _time;
        _time      = ms;
        _lastState = _state;
        _state     = pinVal;
        if (_state != _lastState) {
            _lastChange = ms;
            _changed    = 1;
            if (_state) {
                _pressTime = _time;
            }
        } else {
            _changed = 0;
        }
        return _state;
    }
}

/*----------------------------------------------------------------------*
 * isPressed() and isReleased() check the button state when it was last *
 * read, and return false (0) or true (!=0) accordingly.                *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
uint8_t Button::isPressed(void) {
    return _state == 0 ? 0 : 1;
}

uint8_t Button::isReleased(void) {
    return _state == 0 ? 1 : 0;
}

/*----------------------------------------------------------------------*
 * wasPressed() and wasReleased() check the button state to see if it   *
 * changed between the last two reads and return false (0) or           *
 * true (!=0) accordingly.                                              *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
uint8_t Button::wasPressed(void) {
    return _state && _changed;
}

uint8_t Button::wasReleased(void) {
    return !_state && _changed && millis() - _pressTime < _hold_time;
}

uint8_t Button::wasReleasefor(uint32_t ms) {
    _hold_time = ms;
    return !_state && _changed && millis() - _pressTime >= ms;
}
/*----------------------------------------------------------------------*
 * pressedFor(ms) and releasedFor(ms) check to see if the button is     *
 * pressed (or released), and has been in that state for the specified  *
 * time in milliseconds. Returns false (0) or true (1) accordingly.     *
 * These functions do not cause the button to be read.                  *
 *----------------------------------------------------------------------*/
uint8_t Button::pressedFor(uint32_t ms) {
    return (_state == 1 && _time - _lastChange >= ms) ? 1 : 0;
}

uint8_t Button::pressedFor(uint32_t ms, uint32_t continuous_time) {
    if (_state == 1 && _time - _lastChange >= ms &&
        _time - _lastLongPress >= continuous_time) {
        _lastLongPress = _time;
        return 1;
    }
    return 0;
}

uint8_t Button::releasedFor(uint32_t ms) {
    return (_state == 0 && _time - _lastChange >= ms) ? 1 : 0;
}
/*----------------------------------------------------------------------*
 * lastChange() returns the time the button last changed state,         *
 * in milliseconds.                                                     *
 *----------------------------------------------------------------------*/
uint32_t Button::lastChange(void) {
    return _lastChange;
}
#endif