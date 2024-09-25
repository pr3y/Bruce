#if defined (CORE2)
/*

== The PointAndZone Library ==

  This library was written to supply Point and Zone, common primitives for
  M5Display and M5Button, libraries originally written for the M5Stack series
  of devices. They could be useful for many other applications, especially
  anything based on a TFT_eSPI display driver.


== Point and Zone: Describing Points and Areas on the Screen ==

  The Point and Zone classes allow you to create variables that hold a point
  or an area on the screen.

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


== Looking for Directions? ==

  This library allows you to find the distance in pixels between two Point
  objects with "A.distanceTo(B)". Using "A.directionTo(B)" will output a
  compass course in degrees from A to B. That is, if A lies directly above A
  on the screen the output will be 0, if B lies to the left of A, the output
  will be 270. You can also test whether a direction matches some other
  direction by using "A.isDirectionTo(B, 0, 30)". What this does is take the
  direction from A to B and output 'true' if it is 0, plus or minus 30
  degrees. (So either between 330 and 359 or between 0 and 30).

  Do note that unlike in math, on a display the Y-axis points downwards. So
  pixel coordinates (10, 10) are at direction 135 deg from (0, 0).

  As a last argument to the direction functions, you can supply 'rot1' again
  (default is 'false'). Just like in zones and buttons, what that means is
  that the direction is output as if the rotation 1 coordinate system was
  used.

  Distance and direction functionality is used in Gesture recognition in the
  M5Button highler level library. Its 'Event' objects have methods that look
  very much like these, except the 'To' in the name is missing because Events
  have a starting and ending point so you can just print
  "myEvent.direction()" or state "if (myEvent.isDirection(0,30) ..."


== Some Examples ==

  Point a;
  Point b(50, 120);
  Serial.println(a.valid());                    // 0
  Serial.println(a);                            // (invalid)
  a.set(10, 30);
  Serial.println(a);                            // (10,30)
  Serial.println(a.valid());                    // 1
  Serial.println(b.y);                          // 120
  Serial.println(a.distanceTo(b));              // 98
  Serial.println(a.directionTo(b));             // 156
  Serial.println(a.isDirectionTo(b, 0, 30));    // 0
  Zone z(0,0,100, 100);
  Serial.println(z.w);                          // 100
  Serial.println(z.contains(a));                // 1
  Serial.println(b.in(z));                      // 0


== Screen Rotation and the 'rot1' Property ==

  TL;DR:  just set it to 'false' if you don't need anything special,
          otherwise read the rest of this section.

  The TFT_eSPI library allows you to rotate the screen. On M5Stack devices,
  you do this with "M5.Lcd.SetRotation", supplying a number between 0 and 7.
  Numbers 0-3 are the obvious rotations, with 1 being the default. 4-7 are
  the other 4 mirrored, so you would not be using these unless you want to
  look at the display through a mirror for a homebrew heads-up display or
  mini-teleprompter.

  The M5Touch library for the Core2 device rotates the data it reads from the
  sensor to match the display using the 'rotate' function on the Point
  objects it reads. So if the display is upside down (rotation 3), the (0,
  0)-point is diagonally accross from where it is in rotation 1. (And the
  area that was below the screen at y 240-279 now has negative y-values.)

  Zones (and buttons, because they are extensions of zones) have a 'rot1'
  property. This would normally set to false, meaning the coordinates are
  treated as specified in the current rotation. If it is set to 'true'
  however, the library treats the cordinates as if they are specified in
  rotation 1 and internally rotates them before evaluating whether a Point is
  in a Zone. The Button object also rotates the coordinates before drawing
  the button.

  So by setting 'rot1' to true, you can create zones and buttons that stay in
  the same place even if the screen is rotated. On the Core2, this is used to
  define the BtnA through BtnC virtual below-screen buttons, which should
  always be in the area below the screen where the circles are printed,
  regardless of rotation.


== Porting ==

  To use this library on other devices, simply replace these two lines

    #include <M5Display.h>        // so that we can get the rotation
    #include "utility/Config.h"   // Defines 'TFT', a pointer to the display

  by whatever you need to do to make the symbol 'TFT' be a pointer to a
  TFT_eSPI-derived display device that has a 'rotation' variable. If you
  don't need rotation just delete the lines: the direction functions and the
  'contains' function will now simply ignore their 'rot1' parameters.

*/

#ifndef _POINTANDZONE_H_
#define _POINTANDZONE_H_

#include <Arduino.h>
#include <M5Display.h>       // so that we can get the rotation
#include "utility/Config.h"  // Defines 'TFT', a pointer to the display

#define INVALID_VALUE -32768
#define PLUSMINUS     45  // default value for isDirectionTo

#define DIR_UP    0
#define DIR_RIGHT 90
#define DIR_DOWN  180
#define DIR_LEFT  270
#define DIR_ANY   INVALID_VALUE

#define HIGHEST_X 319  // Can't trust TFT_WIDTH, driver is portrait
#define HIGHEST_Y 239

class Zone;

class Point {
   public:
    Point(int16_t x_ = INVALID_VALUE, int16_t y_ = INVALID_VALUE);
    bool operator==(const Point& p);
    bool operator!=(const Point& p);
    explicit operator bool();
    operator char*();
    void set(int16_t x_ = INVALID_VALUE, int16_t y_ = INVALID_VALUE);
    bool valid();
    bool in(Zone& z);
    bool Equals(const Point& p);
    uint16_t distanceTo(const Point& p);
    uint16_t directionTo(const Point& p, bool rot1 = false);
    bool isDirectionTo(const Point& p, int16_t wanted,
                       uint8_t plusminus = PLUSMINUS, bool rot1 = false);
    void rotate(uint8_t m);
    int16_t x, y;

   private:
    char _text[12];
};

class Zone {
   public:
    Zone(int16_t x_ = INVALID_VALUE, int16_t y_ = INVALID_VALUE, int16_t w_ = 0,
         int16_t h_ = 0, bool rot1_ = false);
    explicit operator bool();
    bool valid();
    void set(int16_t x_ = INVALID_VALUE, int16_t y_ = INVALID_VALUE,
             int16_t w_ = 0, int16_t h_ = 0, bool rot1_ = false);
    bool contains(const Point& p);
    bool contains(int16_t x, int16_t y);
    void rotate(uint8_t m);
    int16_t x, y, w, h;
    bool rot1;
};

#endif /* _POINTANDZONE_H_ */
#endif