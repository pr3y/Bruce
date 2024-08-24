#if defined (CORE2)
/*

== M5Touch - The M5Stack Core2 Touch Library ==

  This is the library behind the M5.Touch object that you can use to read
  from the touch sensor on the M5Stack Core2. It was made to be an input
  source for the M5Button library that provides higher level buttons,
  gestures and events, but both libraries can be also be used alone.


== About the Touch Sensor in the M5Stack Core2 ==

  Touchpanel interfacing is done by a FocalTech FT6336 chip, which supports
  two simultaneous touches. However, the M5Stack Core2 touch display is only
  multi-touch in one dimension. What that means is that it can detect two
  separate touches only if they occur on different vertical positions. This
  has to do with the way the touch screen is wired, it's not something that
  can be changed in software. So you will only ever see two points if they do
  not occur side-by-side. Touches that do happen side-by-side blend into one
  touch that is detected somewhere between the actual touches.

  While this limits multi-touch somewhat, you can still create multiple
  buttons and see two that are not on the same row simultaneously. You could
  also use one of the buttons below the screen as a modifier for something
  touched on the screen.

  The touch sensor extends to below the screen of the Core2: the sensor maps
  to 320x280 pixels, the screen is 320x240. The missing 40 pixels are placed
  below the screen, where the printed circles are. This is meant to simulate
  the three hardware buttons on the original M5Stack units. Note that on some
  units the touch sensor in this area only operates properly if the USB cable
  is plugged in or if the unit is placed firmly in your hand on a metal
  surface.

  For a quick view of how the sensor sees the world, try this sketch:

    #include <M5Core2.h>

    void setup() {
      M5.begin();
      M5.Lcd.fillScreen(WHITE);
    }

    void loop() {
      M5.update();
      Event& e = M5.Buttons.event;
      if (e & (E_MOVE | E_RELEASE)) circle(e & E_MOVE ? e.from : e.to, WHITE);
      if (e & (E_TOUCH | E_MOVE)) circle(e.to, e.finger ? BLUE : RED);
    }

    void circle(Point p, uint16_t c) {
      M5.Lcd.drawCircle(p.x, p.y, 50, c);
      M5.Lcd.drawCircle(p.x, p.y, 52, c);
    }

  (Don't worry if this all seems abracadabra now, we'll get to all of
  this is due time.)


== Point and Zone: Describing Points and Areas on the Screen ==

  The Point and Zone classes allow you to create variables that hold a point
  or an area on the screen. You can

  Point(x, y)

    Holds a point on the screen. Has members x and y that hold the coordinates
    of a touch. Values INVALID_VALUE for x and y indicate an invalid value,
    and that's what a point starts out with if you declare it without
    parameters. The 'valid()' method tests if a point is valid. If you
    explicitly evaluate a Point as a boolean ("if (p) ..."), you also get
    whether the point is valid, so this is equivalent to writing "if
    (p.valid()) ...".

  Zone(x, y, w, h)

    Holds a rectangular area on the screen. Members x, y, w and h are for the
    x and y coordinate of the top-left corner and the width and height of the
    rectangle.

  The 'set' method allows you to change the properties of an existing Point
  or Zone. Using the 'in' or 'contains' method you can test if a point lies
  in a zone.

  The PointAndZone library also provides the low-level support for direction
  from one point to another and for screen rotation translations.

  The documentation in src/utility/PointAndZone.h provides more details and
  examples.


== Basic Touch API ==

  The basic touch API provides a way to read the data from the touch sensor.


  M5.update()
    In the loop() part of your sketch, call "M5.update()". This will in turn
    call M5.Touch.update(), which is the only part that talks to the touch
    interface. It updates the data used by the rest of the API.

  M5.Touch.changed
    Is true if anything changed on the touchpad since the last time
    M5.update() was called.

  M5.Touch.points
    Contains the number of touches detected: 0, 1 or 2.

  M5.Touch.point[0], M5.Touch.point[1]
    M5.Touch.point[0] and M5.Touch.point[1] are Points that hold the detected
    touches.


  A very simple sketch to print the location where the screen is touched:

    #include <M5Core2.h>

    void setup() {
      M5.begin();
    }

    void loop() {
      M5.update();
      if ( M5.Touch.changed ) Serial.println( M5.Touch.point[0] );
    }


== Buttons, Gestures, Events ==

  Note that you may not want to use any of the above directly. The M5Buttons
  library provides button, gestures and events that allow you to quickly
  create reactive visual buttons on the screen and react differently based on
  whether a button was clicked, tapped, or double-tapped. Have a look at the
  documentation for that, which is in the M5Button.h file in the src/utility
  directory of this repository. The examples under "File / Examples
  / M5Core2 / Touch" in your Arduino environment should give you an
  idea of what's possible.


== Screen Rotation ==

  If you rotate the screen with M5.Lcd.setRotation, the touch coordinates
  will rotate along with it.

  * What that means is that either x or y for the area below the screen may
    go negative. Say you use the screen upside-down with
    M5.Lcd.setRotation(3). In that case the off-screen touch area (Y
    coordinates 240 through 279) that was below the screen now becomes above
    the screen and has Y coordinates -40 through -1.

  * See the M5Button library for a feature that allows you to keep some Zone
    and Button objects in the same place on the physical screen, regardless
    of rotation.


== TFT_eSPI Resistive Touch API emulation ==

  While technically not part of this library itself, we added an emulation
  for the TFT_eSPI touch API to the M5Display object that merely passes
  informaton on to the M5.Touch object. So M5.Lcd can be addressed as if it's
  a touch screen using that older resistive touch interface. Do note that
  this interface is not nearly as powerful as M5.Touch's native API. But
  together with M5Button's TFT_eSPI_Button emulation, this should allow you
  to compile lots of ESP32 software written for touch screens.


== Advanced Uses of the Touch Library ==

  You should never need any of the below features in everyday use. But
  they're there just in case...

  M5.Touch.wasRead
    True if the sensor was actually read. The sensor can only provide updates
    every 13 milliseconds or so. M5.update() can loop as quick as once every
    20 MICROseconds, meaning it would continually read the sensor when there
    was nothing to read. So M5.Touch.read() only really reads when it's time
    to do so, and returns with M5.Touch.wasRead false otherwise.

  M5.point0finger
    The FT6336 chip keeps track of fingers, each touch has a finger ID of 0
    or 1. So when there are two touches in point[0] and point[1] and then one
    goes away, point0finger allows you to see which touch is left in
    point[0].

  M5.Touch.interval()
    Without arguments returns the current interval between sensor updates in
    milliseconds. If you supply a number that's the new interval. The default
    of 13 seems to give the most updates per second.

  M5.Touch.ft6336(reg)
  M5.Touch.ft6336(reg, value)
  M5.Touch.ft6336(reg, size, *data)
    Allows you to read and write registers on the ft6336 touch interface chip
    directly. The first form reads one byte, the second form writes one and
    the third form reads a block of 'size' bytes starting at 'reg' into a
    buffer at '*data'.

  M5.Touch.dump()
    M5.Touch.dump() dumps the entire register space on the FT6336 chip as a
    formatted hexdump to the serial port.


== Legacy API ==

  There was a previous version of this library, and it provided a number of
  functions that were single touch only. The older version did not have
  M5.update(). Instead it used ispressed() and getPressedPoint() functions as
  well as HotZones, which provided something that worked a little bit like
  Buttons. This older API is still supported (the M5Core2 Factory Test sketch
  still works), but you should not use it for new programs. The ispressed()
  function specifically does not mix well with code that uses M5.update().


== Example ==

  It may sound complicated when you read it all in this document, but it's
  all made to be easy to use.

  Under File / Examples / M5Core2 / Touch in your Arduino environment is an
  example sketch called "events_buttons_gestures_rotation" that shows both
  this library and M5Button in action. Please have a look at it to understand
  how this all works and run the sketch to see all the events printed to the
  serial port. It shows buttons, gestures and events and should be pretty
  self-explanatory.

*/

#ifndef _M5TOUCH_H_
#define _M5TOUCH_H_

#include <M5Display.h>

#include "utility/Config.h"  // Defines 'TFT', a pointer to the display
#include "utility/PointAndZone.h"

#define TOUCH_W         320
#define TOUCH_H         280
#define CST_DEVICE_ADDR 0x38
#define CST_INT         39

// Strangely, the value 13 leads to slightly more frequent updates than 10
// (Still not every 13 ms, often more like 15 to 20)
#define DEFAULT_INTERVAL 13

class M5Touch {
   public:
    static M5Touch* instance;
    M5Touch();
    void begin();
    uint8_t ft6336(uint8_t reg);
    void ft6336(uint8_t reg, uint8_t value);
    void ft6336(uint8_t reg, uint8_t size, uint8_t* data);
    uint8_t interval(uint8_t ivl);
    uint8_t interval();
    void update();
    bool read();
    bool ispressed();
    void dump();
    Point getPressPoint();
    uint8_t points;
    bool changed, wasRead;
    Point point[2];
    uint8_t point0finger;

   protected:
    uint8_t _interval;
    uint32_t _lastRead;
};

// For compatibility with older M5Core2 code
class HotZone : public Zone {
   public:
    HotZone(int16_t x0_, int16_t y0_, int16_t x1_, int16_t y1_,
            void (*fun_)() = nullptr);
    void setZone(int16_t x0_, int16_t y0_, int16_t x1_, int16_t y1_,
                 void (*fun_)() = nullptr);
    bool inHotZone(Point& p);
    bool inHotZoneDoFun(Point& p);
    void (*fun)();
};

#define HotZone_t    HotZone
#define TouchPoint   Point
#define TouchPoint_t Point

#endif /* _M5TOUCH_H_ */
#endif