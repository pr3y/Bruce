
#if defined (CORE)/*----------------------------------------------------------------------*
 * Arduino Button Library v1.0                                          *
 * Jack Christensen Mar 2012                                            *
 *                                                                      *
 * This work is licensed under the Creative Commons Attribution-        *
 * ShareAlike 3.0 Unported License. To view a copy of this license,     *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 *----------------------------------------------------------------------*/
#ifndef Button_h
#define Button_h
// #if ARDUINO >= 100
#include <Arduino.h>
// #else
// #include <WProgram.h>
// #endif
class Button {
   public:
    Button(uint8_t pin, uint8_t invert, uint32_t dbTime);
    uint8_t read();
    uint8_t isPressed();
    uint8_t isReleased();
    uint8_t wasPressed();
    uint8_t wasReleased();
    uint8_t pressedFor(uint32_t ms);
    uint8_t pressedFor(uint32_t ms, uint32_t continuous_time);
    uint8_t releasedFor(uint32_t ms);
    uint8_t wasReleasefor(uint32_t ms);
    uint32_t lastChange();

   private:
    uint8_t _pin;       // arduino pin number
    uint8_t _puEnable;  // internal pullup resistor enabled
    uint8_t _invert;    // if 0, interpret high state as pressed, else interpret
                        // low state as pressed
    uint8_t _state;     // current button state
    uint8_t _lastState;       // previous button state
    uint8_t _changed;         // state changed since last read
    uint32_t _time;           // time of current state (all times are in ms)
    uint32_t _lastTime;       // time of previous state
    uint32_t _lastChange;     // time of last state change
    uint32_t _lastLongPress;  // time of last state change
    uint32_t _dbTime;         // debounce time
    uint32_t _pressTime;      // press time
    uint32_t _hold_time;      // hold time call wasreleasefor
};
#endif
#endif