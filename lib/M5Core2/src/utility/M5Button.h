#if defined (CORE2)
/*

== M5Button: Buttons, Gestures and Events ==

  * Hardware button support that is 100% Arduino Button Library compatible.

  * Buttons on the screen, either as labels above the original M5Stack's
    hardware buttons or anywhere on the touch screen of the Core2.

  * Zone and Point objects to work with screen locations and areas. Functions
    for distance, direction and more.

  * Touch gestures that are processed before the buttons, so you can still
    use gestures when the screen is full of buttons.

  * Buttons and gestures send events that you can attach handler routines to,
    or poll in a loop. Events include tap, doubletap, pressed, dragged and
    more. Support for key repeat.

  * Extensive screen rotation support, including support for buttons and
    gestures that stay referenced to the physical screen regardless of rotation.

  * Intuitive, consistent and well-documented API.

  * Emulation of the (much less feature-rich) TFT_eSPI_Button class. This
    goes together well with M5Touch's emulation of the TFT_eSPI resistive
    touch screen interface to run a lot of existing programs without
    modification.

  This library was written for the M5Stack series of devices, but was made to
  be general enough to be produce pretty visual buttons with any TFT_eSPI
  display. Its more advanced features need the M5Touch interface, although
  other input methods could be implemented.


== Point and Zone: Describing Points and Areas on the Screen ==

  The Point and Zone classes allow you to create variables that hold a point
  or an area on the screen.

  Point(x, y)

    Holds a point on the screen. Has members x and y that hold the
    coordinates of a touch. Values -1 for x and y indicate an invalid value,
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

  The documentation in src/utility/PointAndZone.h provides more details about
  rotation and examples covering most of the above.


== Buttons ==

  You can create classic Arduino buttons that act on the voltage on a pin of
  the controller. On the M5Stack Core2, you can also create buttons that act
  on touch within a given rectangle on the screen. If you want, that same
  rectangle will also be used for a graphical representation of the button
  that will show a text label in a colored background with a colored outline.
  The colors of background, text, and outline can be configured, both for the
  'off' and the 'on' state of the button.

  Whether on the M5Stack with hardware buttons or on the Core2 with a touch
  screen, buttons are special forms of the 'Zone' object, meaning that all
  functions that apply to 'Zone' objects also work on buttons. On the M5Stack
  with buttons, while this zone cannot be used for touch input, it can still
  be used to display a button that responds to the button state.


== Hardware Buttons ==

  For hardware buttons, use the classic Arduino way of setting up the button,
  by providing the pin, whether the pin is inverted and the debounce time.


    #include <M5Stack.h>

    Button myButton(39, true, 10);

    void setup() {
      M5.begin();
    }

    void loop() {
      M5.update();
      if (myButton.wasPressed()) Serial.print("* ");
    }


  This would set up 'myButton' with the inverse state of pin 39, and a
  debounce time of 10 milliseconds. Because pin 39 is the left button on an
  M5Stack with buttons, this sketch will output a star to the serial port
  every time you release the button. (And because pin 39 is the interrupt
  wire for the touch screen on the Core2, it also happens to output a star on
  that device every time you touch the screen.)

    Note that the sketch uses 'M5.update()' instead of 'myButton.read()'. You
    don't need to read() your buttons explicitly anymore. All buttons created
    with M5Button are automatically read by 'M5.update()'. (Unless you read
    them with 'myButton.read()', in which case 'M5.update()' stops doing that
    to avoid you missing things.)

  The next sections will describe buttons and gestures on the touch screen,
  but if you have an M5Stack device without a touch screen: keep reading
  because many events work on hardware buttons too. Hardware buttons can have
  responsive representation on the screen, we'll get to that also.


== Buttons Using the Touch Screen ==

    Note: It may make sense to also read the documentation in the M5Touch.h
    file, as tells you about the touch sensor and the lower-level touch
    interface that is underneath the M5Button library.

  To have a button that reacts to the touch sensor, all you need to do is
  create a variable for the Button and provide the coordinates (x, y, width
  and height). These buttons can be used in two ways. You can either use them
  the way you would a normal Arduino button, or you can provide handler
  functions to process various events for the button. We'll talk about the
  events later, but here's the same simple sketch from above again but now it
  defines a 100x50 pixel touch button near the top-right of the screen. Note
  that this button does not show anything on the sreen just yet.


    #include <M5Core2.h>

    Button myButton(10, 10, 200, 100);

    void setup() {
      M5.begin();
    }

    void loop() {
      M5.update();
      if (myButton.wasPressed()) Serial.print("* ");
    }


  'wasPressed()' will only be true once when you press the button. You can
  also use the other Arduino button functions such as 'isPressed()' that is
  true as soon and as long as the button is touched. Note that the buttons
  only become pressed if the touch starts within the button, not if you swipe
  over it, and that they will stay pressed as long as the finger touches,
  even if it leaves the button area. You may want read about the events
  further down to distinguish between different kinds of button-presses.

  On the Core2 the three buttons M5.BtnA, M5.BtnB and M5.BtnC from the older
  M5Stack units come already implemented as touch buttons that lie just below
  the screen where the three circles are.


== Buttons with visual appearance ==

  If you want you button to show on the screen, all you need to do is provide
  a set of three colors for the background of the button, the text printed on
  it and the outline of the button. Using yet the same skech again:


    #include <M5Core2.h>

    Button myButton(10, 10, 200, 100, false, "I'm a button !",
                    {BLACK, WHITE, WHITE});

    void setup() {
      M5.begin();
    }

    void loop() {
      M5.update();
      if (myButton.wasPressed()) Serial.print("* ");
    }


  As you can see the colors are provided in {curly braces}, that's because
  they are one variable, of the 'ButtonColors' type. Especialy if you're
  going to define a bunch of buttons, you're better off replacing the button
  line by:


    ButtonColors col = {BLACK, WHITE, WHITE};
    Button myButton(10, 10, 200, 100, false, "I'm a button !", col);


  The order there is background, text, outline. If you do not want any of
  these components drawn, simply put NODRAW in that position. The thing we are
  defining here is what the button draws in its 'off' state. Since we haven
  specified anything to draw in the 'on' state, the button just stays like it
  is, regardless of whether it's pressed. Thus, if we say


    ButtonColors onCol = {BLACK, WHITE, WHITE};
    ButtonColors offCol = {RED, WHITE, WHITE};
    Button myButton(10, 10, 200, 100, false, "I'm a button !", onCol, offCol);


  the button background would turn red if the button was pressed. The button
  colors can also be addressed directly. "myButton.on.bg = BLUE;" will turn
  the background blue in the on state. The other two properties of the
  ButtonColors variable are predicatably called 'text' and 'outline'.

  If you run the sketches you will see the text is placed in the center of
  the button and the buttons have visually please round edges. The corner
  radius defaults to a quarter of the shortest side of the button. You can
  change all this with the remaining parameters when setting up the button:


    Button myButton(10, 10, 200, 100, false, "I'm a button !", onCol, offCol,
                    TL_DATUM, 0, 10, 0);


  These last parameters indicate where to put the label in the TFT_eSPI
  standard datum values, top-left in this case. The values after that are the
  dx and dy, meaning the offsets from the default position. In this case
  that's no left-right offset and 10 pixels down. Negative values move the
  other way. The last value is the corner radius. In this case it would draw
  an ugly 1980's rectangular button.

  You can make a button draw its current state with "myButton.draw()", or all
  buttons do that with "M5.Buttons.draw()". You can also call draw with a
  ButtonColors variable so "myButton.draw({BLUE, WHITE, WHITE})" draws it
  with those colors. Until the next state-change comes along that is, if you
  have colors for the new state defined.

  Note that the text provided here is the name of the buttton. A button
  always keeps the same name, but the label (that which is shown) can change,
  but initialises to the name. Use 'myButton.setLabel("New text!")' to change
  it.

  With "myButton.hide()" you can make a button temporarily invisible to the
  touch sensor. You can specify an optional color value to draw over the
  button if you want to make it visually disappear also. myButton.draw() makes
  it visible to the touch sensor again, even if you have no colors defined, so
  nothing shows on the screen. "MyButton.erase()" only paints over the button,
  in a color you can specify (default black).


== Visual Buttons (Labels) with Hardware Buttons ==

  You can have a visual representation of the state of a hardware button on
  the screen, for example right above the hardware buttons of the original
  M5Stack. We'll call these buttons "labels", but they're regular buttons
  that just respond to a physical button insetad of the touch sensor. If you
  want to display a label on the screen that responds to the state of a
  hardware button, just set up a hardware button up as usual, but then follow
  the parameter list with "hw" (in quotes), followed by the parameters of the
  touch button below.

  The hardware buttons in the older M5Stack devices are already set up to
  display labels: all you need is supply colors. Their initialization (in
  M5Stack.h in this library) looks like this:


    Button BtnA = Button(BUTTON_A_PIN, true, DEBOUNCE_MS,
                         "hw", 3, 218, 102, 21, true, "BtnA");
    Button BtnB = Button(BUTTON_B_PIN, true, DEBOUNCE_MS,
                         "hw", 109, 218, 102, 21, true, "BtnB");
    Button BtnC = Button(BUTTON_C_PIN, true, DEBOUNCE_MS,
                         "hw", 215, 218, 102, 21, true, "BtnC");


  As you can see: its just a hardware button that has a zone to display the
  label. So the sketch below is all that is needed to show repsonsive labels
  on the M5Stack:


    #include <M5Stack.h>

    void setup() {
      M5.BtnA.off = M5.BtnB.off = M5.BtnC.off = {BLUE, WHITE, NODRAW};
      M5.BtnA.on  = M5.BtnB.on  = M5.BtnC.on  = {RED,  WHITE, NODRAW};
      M5.begin();
      M5.Buttons.draw();
    }

    void loop() {
      M5.update();
    }


  If you looked closely you might have noticed that the mysterious fifth
  argument has changed from 'false' to 'true'. This argument is called
  'rot1', and it determines that the location of this Zone or Button is
  specified in rotation one, i.e. the normal default screen rotation. What
  that means is that no matter what rotation you set the display to, these
  button will always stay in the same place. The documentation in
  src/utility/PointAndZone.h has more details if you want to know more about
  this. You will only ever need rot1 if you need multiple screen rotations
  AND you want objects to stay in the same physical place regardless.


== M5.Buttons ==

  Apart from the class "Button" that you use to create buttons of your own,
  there is an instance called "M5.Buttons" (plural), that is used to talk to
  the M5Button library for things that involve all buttons. For instance:
  "M5.Buttons.setFont" sets a font for all buttons, and you can use
  "M5.Buttons.addHandler" to add a handler that gets events for all buttons
  (and gestures).


== Events ==

  Buttons (and gestures, but we'll get to those later) have a set of simple
  functions to see if they are pressed or not. These Arduino-compatible
  functions work fine for that purpose. But if you want to detect whether a
  button received a short tap, or even a double-tap on many buttons
  simultaneously, you find yourself writing quite a bit of code for that.

  Events are M5Button's way of making it easier to react to events on
  hardware buttons or the touch screen. For this you need to define one or
  more event handler functions. This is done like this:

    void myHandler(Event& e) { ... }

  It's important to do it exactly this way, only changing the name of the
  function. You can then set things up so that this function receives events.

  Here's an events-based sketch for the Core2. We'll base it on the same
  buton we've seen before.


    #include <M5Core2.h>

    ButtonColors onCol = {BLACK, WHITE, WHITE};
    ButtonColors offCol = {RED, WHITE, WHITE};
    Button myButton(10, 10, 200, 100, false, "I'm a button !", onCol, offCol);

    void setup() {
      M5.begin();
      myButton.addHandler(touched, E_TOUCH);
      myButton.addHandler(released, E_RELEASE);
    }

    void loop() {
      M5.update();
    }

    void touched(Event& e) {
      Serial.println("Touched!");
    }

    void released(Event& e) {
      Serial.println("Released!");
    }


  Note that the function names "touched" and "released" are provided to
  addHandler without the parenthesis. Here's two ways you can set up a handler
  function to receive events:

    M5.Buttons.addHandler(myHandler);

    - or -

    myButton.addHandler(myHandler);

  The first form receives all the events relating to all buttons and gestures,
  the second form only receives the events for that specific button. After the
  name of the function, without the brackets, you can specify which events the
  function needs to receive. You can add together (or "bitwise or") the names
  of the events if you want a handler function to reive multiple events.

  The Event object that is passed to the handler function contains all sorts
  of information about the event: where on the screen it started, where it
  ended, the duration, etc. etc.

  Let's first look at all the possible events and when they are fired. The
  first three events always happen when a finger touches the display.

  E_TOUCH, E_MOVE and E_RELEASE

    The E_TOUCH and E_RELEASE events fire when a button is pressed and
    released. On a touch sensor, E_MOVE will fire every time it detects the
    finger has moved. These events cannot be prevented from firing, like most
    of the other ones. So every time your finger touches the display it will
    fire E_TOUCH and then E_MOVEs until finally, when you release your
    finger, an E_RELEASE.

  E_PRESSING and E_LONGPRESSING

    There are also events that happen while the button is still pressed.
    These are E_PRESSING and E_LONGPRESSING. E_PRESSING happens as soon as
    M5Button is sure that's not just a short tap (more later). The maximum
    time for a tap is settable, but defaults to 150 ms. So if the button is
    still held 150 ms after E_TOUCH, E_PRESSING fires. Just once, unless you
    have set up a key repeat, more about that later too. Then at some point
    you might get a E_LONGPRESSING, if you have set up a duration for that to
    happen.

  E_TAP, E_DBLTAP, E_PRESSED, E_LONGPRESSED and E_DRAGGED

    Unless the keypress is cancelled (more later), exactly one of these events
    will fire after the button has been released, after E_RELEASE has fired.
    Think of these as final decisions on what kind of keypress this was.
    (E_TAP takes a tiny bit longer before it fires because M5Button needs to
    make sure it wasn't a doubletap, in that case E_DBLTAP wil fire instead.)

    So tap and doubletap are sort of obvious, E_LONGPRESSED fires if the key
    was pressed more that the set time in ms. E_DRAGGED fires if the finger
    has moved outside of the button area when it was released. E_PRESSED is
    fires in all other cases.

  E_GESTURE

    Doesn't really fit in with the others, but is the event that gets fired
    when a gesture is detected.


  If at any point after the pressing of a button, "myButton.cancel()" is
  called, no further high-level events for that button will fire. What that
  means is nothing other than possible E_MOVEs and one E_RELEASE event will
  fire for that button until it is released and then pressed again. This is
  used internally when a gesture is detected, so that when a touch gesture
  starts on a button, there won't be an E_PRESSED, or any of the others.

  The second thing to look at more closely is the 'Event' object itself. When
  you set up a handler function like this

    void myhandler(Event& e) {

  what that means is you're creating a function that recives a (reference to)
  an event. That event has all sorts of properties that we can look at.


    e.type

      The type of event, such as E_TOUCH or E_TAP from above. The event
      itself, when you evaluate it, also returns the type. What that means is
      that "if (e.type == E_TAP) .." is equivalent with "if (e == E_TAP) .."

    e.finger

      0 or 1, whether this is the first or second finger detected on the
      touch screen. Left at zero on the M5Stack with buttons.

    e.from and e.to

      Points that say from where to where this event happened. Left at
      invalid for the M5Stack with buttons.

    e.duration

      Duration of the event in milliseconds.

    e.button

      Pointer to the button attached to the event. What that means is that you
      can use all the methods for button as long as you precede them with
      "e.button->". Note the '->' there because this is a pointer to an
      object.

    e.gesture

      e.gesture is a pointer to the gesture attached to the event, and may be
      null if the event is not a gesture. So unless you know for sure this
      event is a gesture (because handler attached to that gesture or because
      you asked for E_GESTURE events only), this pointer needs to be tested
      using "if (e.gesture)" before using -> methods on it, oterwise your
      program will crash.

    other methods

      Additionally, you can ask for the name of the event as text by using
      "e.typeName()" and get the name of the gesture or button with
      "e.objName()". "e.direction()" gives the direction, for instance of a
      gesture or of an E_RELEASE event, where it gives direction between
      E_TOUCH and E_RELEASE. "e.isDirectionTo(0,30)" will output true if the
      swipe was upwards, plus or minus 30 degrees.


  When you add a handler function you can also specify what events it should
  receive by supplying it as the second argument after the handler function.
  If you want to register multiple events for the same function, don't
  register the handler twice, but simply add (or bitwise or) the event
  values. The default value there is the pseudo-event E_ALL, which is simply
  a value with all the event bits turned on. You can also subtract event type
  values from E_ALL to exclude them.

  Here are some examples of ways to add a handler function:


    button1.addHandler(b1Function, E_TOUCH + E_RELEASE);

      b1Function only get these two events for button1.


    M5.Buttons.addHandler(btnHandle, E_ALL - E_MOVE);

      btnHandle gets all events, except E_MOVE.


    swipeUp.addHandler(nextPage);

      Handler nextPage is called when swipeUp gesture detected.


  Note that all handler functions must be of the "void someName(Event& e)"
  type, even if they plan to completely ignore the event that is passed to
  them.


  If your event reads data or calls functions in e.button or e.gesture,
  remember that these are pointers. Without going into too much detail, it
  means it must do so with the -> notation, so to read the button x position,
  you would say "e.button->x".

  Please have a look at the example sketch (see below) to understand how this
  all works and run the sketch to see all the events printed to the serial
  port.


== Taps, Doubletaps, Longpresses and Key Repeat ==

  Some features are best explained with some examples:

    myButton.tapTime = 0;

      Turns off detection of taps and doubletaps, the button will fire
      E_PRESSING immediately when pressed. Any other value makes that the
      maximum time a tap can take in milliseconds, and thus the wait tme
      before "E_PRESSING" fires.

    mybutton.tapWait = 0;

      Turns off detection of doubletaps only. Any other value makes that the
      wait before an E_TAP fires, because M5Button is still waiting to see if
      it's maybe a doubletap.

    mybutton.longPressTime = 700;

      Sets up the button to fire an E_LONGPRESSING after 700 ms, and then fire
      E_LONGPRESSED instead of E_PRESSED when the button is released. By
      default this is set to zero, meaning longpress detection is off.

    myButton.repeatDelay = 500;
    myButton.repeatInterval = 250;

      Makes the button repeat the sending of its E_PRESSING event every 250
      milliseconds if key is held for 500 ms.


== In Loop vs. Event Handlers ==

  Button and Gesture objects have an 'event' method that returns the event
  that was detected this time around by 'M5.update()'. Each event comes in
  it's own rotation of 'M5.update()', so if you prefer to detect events this
  way and not with handler routines that's fine too.

  If nothing was detected, the event type will be set to E_NONE with a value
  of 0, so you can do "if (myButton.event) ...". 'M5.Buttons.event' has the
  event detected this time around, regardless of what button or gesture it was
  attached to. This example prints a star to serial if it is doubletapped.

    #include <M5Core2.h>

    Button myButton(50,70,220, 100, false, "Button",
                    {YELLOW, BLACK, NODRAW},
                    {RED, BLACK, NODRAW} );

    void setup() {
      M5.begin();
      M5.Buttons.setFont(FSS18);
      M5.Buttons.draw();
    }

    void loop() {
      M5.update();
      if (myButton.event == E_DBLTAP) Serial.print("* ");
    }


== M5.background ==

  Only one button can become pressed for any spot on the touch screen. If you
  define overlapping buttons, the first defined button for the overlap become
  pressed and gets all subsequent events.

  One special button, "M5.background", was defined before any others, and it
  has the size of the entire touch sensor. This means it gets all events
  where the first touch was not within any of the defined buttons.


== Gestures on the Touch Screen ==

  Whenever a finger is released from the touch screen and before any
  higher-level button events are fired, the library first checks whether this
  was perhaps a gesture. When you define gestures, you can optionally specify
  the zone in which the gesture must start, the zone in which it must end, the
  minimum distance the finger must have travelled, the direction it has
  travelled in and the maximum time the gesture may take.

    Gesture exampleGesture(fromZone, toZone, "exampleName", minimumDistance,
    direction, plusminus, ro1, maxTime)

  Where fromZone and toZone can be valid zones or the word "ANYWHERE". If you
  want to specify neither fromZone nor toZone, you can also leave them off
  completely. The minimum distance defaults to 75 pixels. The direction
  (default: don't care) is in compass degrees (so 180 is down), but the
  compiler defines DIR_UP, DIR_DOWN, DIR_LEFT and DIR_RIGHT are provided for
  convenience. The plusminus deines how many degress off-course the gesture
  may be, and the rot1 flag defines whether this direction is relative to the
  current rotation, or as seen in rotation 1. maxTime is in milliseconds as
  usual and defaults to 500 ms. DIR_ANY can be used for direction if you need
  to specify it in order provide a rot1 or maximum time value.

  here are a few examples of valid gesture definitions:


    Gesture swipeDown("swipe down", 100, DIR_DOWN, 30);

      Down (plus or minus 30 degrees) for at least 100 pixels within 500 ms


    Gesture fromTop(Zone(0, 0, 360, 30), ANYWHERE, "from top", 100, DIR_DOWN,
30);

      The same but starting from within the top 30 pixels. (If you make that
      too narrow you may miss the swipe because the sensor 'sees' only once
      every 13 ms or so.


  (Note that if you defined both these gestures in this order the second one
  would never fire because any swipe that matched number two would first match
  number one and fire that one instead.)

  Gestures have a 'wasDetected()' method if you want to detect them in the
  main loop, or you attach a handler the same way you would for a button,
  with "myGesture.addhandler(myHandler)"


    #include <M5Core2.h>

    Gesture swipeDown("swipe down", DIR_DOWN, 30);

    void setup() {
      M5.begin();
    }

    void loop() {
      M5.update();
      if (swipeDown.wasDetected()) Serial.println("Swiped down!");
    }


== Advanced Hints and Tricks

  ## drawFn

    If you look at the source code for the library you will see that the
    drawing of the button is done by a static function in the M5Buttons
    object. It's defined as

      void M5Buttons::drawFunction(Button& b, ButtonColors bc)

    If you make your own function that takes the same arguments but that does
    something different, you can make the library use it by saying
    "M5.Buttons.drawFn = myFunction". You can even do that on a per-button
    basis with "myButton.drawFn = myFunction".


  ## drawZone

    A Button instance _is_ also a Zone object, in that it descends from it.
    Which means a Button has all the methods of a Zone object, as well as its
    own. But it contains another zone, called drawZone. This allows you to
    have the visual representation happen somewhere else than where the
    button is on the touch sensor. Normally this is set to "invalid zone",
    but if you set it to a valid screen area, the button will be drawn there.
    This is used internally to put the optional labels for the off-screen
    buttons on the Core2 on the screen just above their touch areas.


  ## Drawing is Non-Invasive

    This library uses a brand-new feature of the M5Display object --
    M5.Lcd.popState() and M5.lcd.pushState() -- that allows it to save and
    reload the complete display driver state before and after drawing a
    button. What that means is that you can draw to the display without
    worrying that the button drawing will mess with your font setting, cursor
    position or anything else that is display-related.


  ## TFT_ePI_Button Emulation

    This libary also defines an object called TFT_eSPI_Button, which is the
    old way of doing buttons that comes as an optional extra with the display
    library. Together with M5Touch's emulation of the TFT_eSPI touch
    interface (written for the older resistive touch-screens), you can use it
    to run software made for those APIs. Do not use either for new code: the
    native interfaces are much more powerful.


  ## Buttons and Variable Scope

    Buttons come into existence and are drawn in their initial state when
    their variables are defined and are not detected anymore when their
    variables are removed from memory when the function they were defined in
    returns. Except for global buttons - defined outside any functions: their
    variables always exist. The programmer has to take responsability for
    erasing expired buttons off the screen because Button doesnt know what is
    supposed to be in the background. If you're not clearing the entire
    screen anyway, this can be done with "myButton.erase(BLACK)" if the
    background is to be black.

*/

#ifndef _M5BUTTON_H_
#define _M5BUTTON_H_

class Gesture;

#include <Arduino.h>
#include <functional>
#include <Free_Fonts.h>
#include <M5Display.h>
#include <TFT_eSPI.h>
#include <vector>

#include "PointAndZone.h"
#include "utility/Config.h"

//#ifdef M5Stack_M5Core2
#include <M5Touch.h>
//#endif /* M5Stack_M5Core2 */

#define BUTTON_FREEFONT FSS9
#define BUTTON_TEXTFONT 1
#define BUTTON_TEXTSIZE 1
#define BUTTON_DATUM    MC_DATUM

#define TAP_TIME        150
#define DBLTAP_TIME     300
#define LONGPRESS_TIME  0
#define REPEAT_DELAY    0
#define REPEAT_INTERVAL 200

#define GESTURE_MAXTIME 500
#define GESTURE_MINDIST 75
#define ANYWHERE        Zone()

#define NUM_EVENTS     11
#define E_TOUCH        0x0001
#define E_RELEASE      0x0002
#define E_MOVE         0x0004
#define E_GESTURE      0x0008
#define E_TAP          0x0010
#define E_DBLTAP       0x0020
#define E_DRAGGED      0x0040
#define E_PRESSED      0x0080
#define E_PRESSING     0x0100
#define E_LONGPRESSED  0x0200
#define E_LONGPRESSING 0x0400

#define E_ALL 0x0FFF

#define NODRAW 0x0120  // Special color value: transparent

struct ButtonColors {
    uint16_t bg;
    uint16_t text;
    uint16_t outline;
};

class Button;
class Event;

#ifdef _M5TOUCH_H_
struct Finger {
    Point current, previous, startPoint, tapPoint;
    uint32_t startTime, tapTime;
    Button* button;
};
#endif

class Event {
   public:
    Event();
    operator uint16_t();
    const char* typeName();
    const char* objName();
    uint16_t direction(bool rot1 = false);
    bool isDirection(int16_t wanted, uint8_t plusminus = PLUSMINUS,
                     bool rot1 = false);
    uint16_t distance();
    uint8_t finger;
    uint16_t type;
    Point from, to;
    uint16_t duration;
    Button* button;
    Gesture* gesture;
};

typedef std::function<void(Event&)> EventHandlerCallback;

class Button : public Zone {
   public:
    static std::vector<Button*> instances;
    Button(int16_t x_, int16_t y_, int16_t w_, int16_t h_, bool rot1_ = false,
           const char* name_ = "", ButtonColors off_ = {NODRAW, NODRAW, NODRAW},
           ButtonColors on_ = {NODRAW, NODRAW, NODRAW},
           uint8_t datum_ = BUTTON_DATUM, int16_t dx_ = 0, int16_t dy_ = 0,
           uint8_t r_ = 0xFF);
    Button(uint8_t pin_, uint8_t invert_, uint32_t dbTime_, String hw = "hw",
           int16_t x_ = 0, int16_t y_ = 0, int16_t w_ = 0, int16_t h_ = 0,
           bool rot1_ = false, const char* name_ = "",
           ButtonColors off_ = {NODRAW, NODRAW, NODRAW},
           ButtonColors on_  = {NODRAW, NODRAW, NODRAW},
           uint8_t datum_ = BUTTON_DATUM, int16_t dx_ = 0, int16_t dy_ = 0,
           uint8_t r_ = 0xFF);
    ~Button();
    operator bool();
    bool operator==(const Button& b);
    bool operator!=(const Button& b);
    bool operator==(Button* b);
    bool operator!=(Button* b);
    int16_t instanceIndex();
    bool read(bool manualRead = true);
    void fingerDown(Point pos = Point(), uint8_t finger = 0);
    void fingerUp(uint8_t finger = 0);
    void fingerMove(Point pos, uint8_t finger);
    void cancel();
    bool isPressed();
    bool isReleased();
    bool wasPressed();
    bool wasReleased();
    bool pressedFor(uint32_t ms);
    bool pressedFor(uint32_t ms, uint32_t continuous_time);
    bool releasedFor(uint32_t ms);
    bool wasReleasefor(uint32_t ms);
    void addHandler(EventHandlerCallback fn, uint16_t eventMask = E_ALL);
    void delHandlers(EventHandlerCallback fn = nullptr);
    char* getName();
    uint32_t lastChange();
    Event event;
    uint16_t userData;
    uint16_t tapTime, dbltapTime, longPressTime;
    uint16_t repeatDelay, repeatInterval;

   protected:
    void init();
    bool postReleaseEvents();
    bool timeoutEvents();
    friend class M5Buttons;
    char _name[16];
    uint8_t _pin;
    uint16_t _dbTime;
    bool _invert;
    bool _changed, _state, _tapWait, _pressing;
    bool _longPressing, _cancelled, _manuallyRead;
    uint8_t _setState;
    uint32_t _time, _lastRepeat;
    uint32_t _lastChange, _lastLongPress, _pressTime, _hold_time;
    uint8_t _finger;
    Point _fromPt[2], _toPt[2], _currentPt[2];

    // visual stuff
   public:
    void draw(ButtonColors bc);
    void draw();
    void hide(uint16_t color = NODRAW);
    void erase(uint16_t color = TFT_BLACK);
    void setLabel(const char* label_);
    //void setFont(const GFXfont* freeFont_);
    void setFont(uint8_t textFont_ = 0);
    void setTextSize(uint8_t textSize_ = 0);
    char* label();
    ButtonColors off, on;
    Zone drawZone;
    uint8_t datum, r;
    int16_t dx, dy;
    void (*drawFn)(Button& b, ButtonColors bc);

   protected:
    bool _hidden;
    bool _compat;  // For TFT_eSPI_Button emulation
    char _label[51];
    uint8_t _textFont;
    //const GFXfont* _freeFont;
    uint8_t _textSize;
};

class Gesture {
   public:
    static std::vector<Gesture*> instances;
    Gesture(Zone fromZone_, Zone toZone_, const char* name_ = "",
            uint16_t minDistance_ = GESTURE_MINDIST,
            int16_t direction_ = INVALID_VALUE, uint8_t plusminus_ = PLUSMINUS,
            bool rot1_ = false, uint16_t maxTime_ = GESTURE_MAXTIME);
    Gesture(const char* name_ = "", uint16_t minDistance_ = GESTURE_MINDIST,
            int16_t direction_ = INVALID_VALUE, uint8_t plusminus_ = PLUSMINUS,
            bool rot1_ = false, uint16_t maxTime_ = GESTURE_MAXTIME);
    ~Gesture();
    operator bool();
    int16_t instanceIndex();
    bool test(Point& from, Point& to, uint16_t duration);
    bool wasDetected();
    void addHandler(EventHandlerCallback fn, uint16_t eventMask = E_ALL);
    void delHandlers(EventHandlerCallback fn = nullptr);
    char* getName();
    Zone fromZone;
    Zone toZone;
    Event event;
    int16_t direction;
    uint8_t plusminus;
    bool rot1;
    uint16_t maxTime, minDistance;

   protected:
    friend class M5Buttons;
    bool _detected;
    char _name[16];
};

struct EventHandler {
    uint16_t eventMask;
    Button* button;
    Gesture* gesture;
    EventHandlerCallback fn;
};

class M5Buttons {
   public:
    static M5Buttons* instance;
    static void drawFunction(Button& b, ButtonColors bc);
    M5Buttons();
    Button* which(Point& p);
    void draw();
    void update();
    //void setFont(const GFXfont* freeFont_);
    void setFont(uint8_t textFont_);
    void setTextSize(uint8_t textSize_);
    void (*drawFn)(Button& b, ButtonColors bc);
    void fireEvent(uint8_t finger, uint16_t type, Point& from, Point& to,
                   uint16_t duration, Button* button, Gesture* gesture);
    void addHandler(EventHandlerCallback fn, uint16_t eventMask = E_ALL,
                    Button* button = nullptr, Gesture* gesture = nullptr);
    void delHandlers(EventHandlerCallback fn, Button* button, Gesture* gesture);
    Event event;

   protected:
    std::vector<EventHandler> _eventHandlers;
    uint8_t _textFont;
    //const GFXfont* _freeFont;
    uint8_t _textSize;
    bool _leftovers;

#ifdef _M5TOUCH_H_
   protected:
    Finger _finger[2];
#endif
};

// TFT_eSPI_Button compatibility emulation

class TFT_eSPI_Button2 : public Button {
   public:
    TFT_eSPI_Button2();
    void initButton(TFT_eSPI* gfx, int16_t x, int16_t y, uint16_t w, uint16_t h,
                    uint16_t outline, uint16_t fill, uint16_t textcolor,
                    char* label_, uint8_t textsize_);
    void initButtonUL(TFT_eSPI* gfx, int16_t x_, int16_t y_, uint16_t w_,
                      uint16_t h_, uint16_t outline, uint16_t fill,
                      uint16_t textcolor, char* label_, uint8_t textsize_);
    void setLabelDatum(int16_t x_delta, int16_t y_delta,
                       uint8_t datum = MC_DATUM);
    void drawButton(bool inverted = false, String long_name = "");
    bool contains(int16_t x, int16_t y);
    void press(bool p);
    bool isPressed();
    bool justPressed();
    bool justReleased();

   private:
    bool currstate, laststate;
};

#endif /* _M5BUTTON_H_ */
#endif