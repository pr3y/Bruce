/*
  XPT2046_Touchscreen_TT.h - Support touchscreens using the XPT2046 controller
  chip. This project was cloned from Paul Stoffregen's XPT2046_Touchscreen
  project.

  Copyright (c) 2015, Paul Stoffregen, paul@pjrc.com

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice, development funding notice, and this permission
  notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.


  Details:

  This library makes the following changes to the original XPT2046_Touchscreen
  library by Paul Stoffregen:

    1. Add new code files TS_display.h and .cpp, and new example program files
        TS_DisplayEvents.ino and TS_DisplayCalibrate.ino, to support touchscreen
        touch and release events, mapping touchscreen coordinates to/from
        display coordinates, and calibrating the touchscreen-to-display mapping.

    2. Move defines of Z_THRESHOLD and Z_THRESHOLD_INT from .cpp to .h file.

    3. Add function setThresholds() to allow Z_THRESHOLD and Z_THRESHOLD_INT to
        be changed programmatically, now stored in variables Z_Threshold and
        Z_Threshold_Int.

    4. Add function Zthreshold() which returns the pressure threshold
        Z_Threshold.

    5. Add function Zthreshold_Int() which returns the pressure threshold
        Z_Threshold_Int.

    6. Added doxygen-style function documentation to the .h file.

    7. Example programs have been changed:

        - Use Adafruit_GFX_Library and Adafruit_ILI9341 libraries in place of
          ILI9341_t3 library.

        - Use Adafruit_GFX_library FreeSans12pt7b font in place of Arial fonts
          from ILI9341_t3 library.

        - Set serial monitor speed to 115,200 bps instead of 38,400 bps.

        - Change pin assignments to match my own system and add usage comment
          reminding user to set the constants for his system.

        - Use longer names for _PIN #defines.

        - Allocate display and touchscreen objects with "new".

        - Adjust Serial initialization to start up better.

        - ILI9341Test.ino is renamed to TS_DisplayTest.ino and it now uses new
          files TS_Display.h/.cpp to display a "+" on the display at the tapped
          point, and it doesn't repeatedly write data to monitor and display,
          but only during a tap or untap.
*/
#ifndef XPT2046_Touchscreen_TT_h
#define XPT2046_Touchscreen_TT_h

#include <Arduino.h>
#include <SPI.h>

#if defined(__IMXRT1062__)
#if __has_include(<FlexIOSPI.h>)
	#include <FlexIOSPI.h>
#endif
#endif

#if ARDUINO < 10600
#error "Arduino 1.6.0 or later (SPI library) is required"
#endif

// Initial thresholds, for press and for clearing interrupt flag.
#define Z_THRESHOLD     400
#define Z_THRESHOLD_INT	75

/**************************************************************************/
/*!
  @brief    Class TS_Point holds a touchscreen "point" (x, y, z), where (x,y) is
            a touchscreen coordinate position and z is the touch pressure.
*/
/**************************************************************************/
class TS_Point {
public:
  /**************************************************************************/
  /*!
    @brief    Construct a TS_Point object at coordinate (0,0,0).
  */
  /**************************************************************************/
	TS_Point(void) : x(0), y(0), z(0) {}

  /**************************************************************************/
  /*!
    @brief    Construct a TS_Point object at coordinate (x,y,z).
    @param    x   X-coordinate of TS_Point object.
    @param    y   Y-coordinate of TS_Point object.
    @param    z   Z-coordinate of TS_Point object.
  */
  /**************************************************************************/
	TS_Point(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {}

  /**************************************************************************/
  /*!
    @brief    Compare this TS_Point object to another using == operator.
    @param    p   TS_Point object to be compared to this object.
    @returns  true if objects are equal, false if not.
  */
  /**************************************************************************/
	bool operator==(TS_Point p) { return ((p.x == x) && (p.y == y) && (p.z == z)); }

  /**************************************************************************/
  /*!
    @brief    Compare this TS_Point object to another using != operator.
    @param    p   TS_Point object to be compared to this object.
    @returns  false if objects are equal, true if not.
  */
  /**************************************************************************/
	bool operator!=(TS_Point p) { return ((p.x != x) || (p.y != y) || (p.z != z)); }

	int16_t x, y, z;
};

/**************************************************************************/
/*!
  @brief    Class XPT2046_Touchscreen manages a touchscreen controlled by an
            XPT_2046 controller, with functions for sensing touches.
*/
/**************************************************************************/
class XPT2046_Touchscreen {

private:

  // Test touch pressure and update xraw/yraw/zraw and msraw.
	void update();

	// Pins interfacing to controller.
	uint8_t csPin, tirqPin;

  // Touchscreen rotation, 0-3, meaning same as display rotation.
	uint8_t rotation;

	// Touchscreen most recently read coordinates (xraw,yraw) and pressure (zraw).
	int16_t xraw, yraw, zraw;

  // Touchscreen pressure threshold for touch.
	int16_t Z_Threshold;

  // Touchscreen pressure threshold for clearing isrWake flag.
	int16_t Z_Threshold_Int;

	// Millisecond time used to require a 3ms period of touch before touch is
	// registered.
	uint32_t msraw=0x80000000;

  #if defined(_FLEXIO_SPI_H_)
	// Pointer to FlexIOSPI SPI device connected to controller, nullptr if none.
	FlexIOSPI *_pflexspi;
  #else
	// Pointer to SPIClass SPI device connected to controller, nullptr if none.
	SPIClass *_pspi;
  #endif

  // true when touchscreen interrupt occurs, cleared when touch pressure under
  // Z_Threshold_Int is detected.
	volatile bool isrWake;

public:

  /**************************************************************************/
  /*!
    @brief    Construct an XPT2046_Touchscreen object.
    @param    cspin   Arduino pin number of pin connected to XPT2046 CS pin.
    @param    tirq    Arduino pin number of pin connected to XPT2046 IRQ pin, or
                      255 to not use interrupts.
    @note     Only a single instance is supported.
  */
  /**************************************************************************/
	constexpr XPT2046_Touchscreen(uint8_t cspin, uint8_t tirq=255)
		: csPin(cspin), tirqPin(tirq), rotation(1), xraw(0), yraw(0), zraw(0),
		  Z_Threshold(Z_THRESHOLD), Z_Threshold_Int(Z_THRESHOLD_INT),
		  msraw(0x80000000),
      #if defined(_FLEXIO_SPI_H_)
      _pflexspi(nullptr),
      #else
       _pspi(nullptr),
      #endif
		  isrWake(true) {
	  }

  /**************************************************************************/
  /*!
    @brief    Initialize the XPT2046 and optionally establish interrupts.
    @param    wflexspi  Reference to the FlexIOSPIdevice that is connected to
                        the XPT2046. Argument only present if _FLEXIO_SP_H is
                        defined.
    @param    wspi      Reference to the serial peripheral interface device that
                        is connected to the XPT2046. Argument only present if
                        _FLEXIO_SP_H is not defined.
    @returns  true if successful, false if failure
  */
  /**************************************************************************/
	bool begin(
    #if defined(_FLEXIO_SPI_H_)
    FlexIOSPI &wflexspi);
    #else
  	SPIClass &wspi = SPI);
    #endif

  /**************************************************************************/
  /*!
    @brief    Return last touched point, initially (0,0,0).
    @returns  The last touched point.
  */
  /**************************************************************************/
	TS_Point getPoint();

  /**************************************************************************/
  /*!
    @brief    Return flag indicating if ISR was called due to a touch action.
    @returns  true if interrupt occurred due to a touch action, else false.
  */
  /**************************************************************************/
	bool tirqTouched();

  /**************************************************************************/
  /*!
    @brief    Return flag indicating if active touch action exceeds threshold.
    @returns  true if there is an active touch action that exceeds threshold
              Z_threshold, else false.
  */
  /**************************************************************************/
	bool touched();

  /**************************************************************************/
  /*!
    @brief    Get current touch coordinates and pressure.
    @param    x   Pointer to variable to receive touchscreen x-coordinate last
                  touched, initially 0.
    @param    y   Pointer to variable to receive touchscreen y-coordinate last
                  touched, initially 0.
    @param    z   Pointer to variable to receive touchscreen z-coordinate that
                  represents the current touch pressure, initially 0.
  */
  /**************************************************************************/
	void readData(uint16_t *x, uint16_t *y, uint8_t *z);

  /**************************************************************************/
  /*!
    @brief    Return flag indicating if at least 3 ms has elapsed since the last
              time a press exceeding Z_threshold was recognized.
    @returns  true if less than 3 ms has elapsed since the last time a press
              exceeding Z_threshold was recognized, false if more than 3 ms.
    @note     If true is returned, it can be assumed that a touch just happened
              and may still be in motion and not settled, whereas if false is
              returned, no activity has happened for a while and the last touch
              event might be considered a stable one.
  */
  /**************************************************************************/
	bool bufferEmpty();

  /**************************************************************************/
  /*!
    @brief    Return number of touches available in touch buffer returned by
              getPoint().
    @returns  1. There is no buffer, just one point.
  */
  /**************************************************************************/
	uint8_t bufferSize() { return 1; }

  /**************************************************************************/
  /*!
    @brief    Set the screen rotation.
    @param    n   0=north, 1=east, 2=south, 3=west, 0/2=portrait, 1/3=landscape.
  */
  /**************************************************************************/
	void setRotation(uint8_t n) { rotation = n % 4; }

  /**************************************************************************/
  /*!
    @brief    Set touch thresholds.
    @param    Z_Threshold_press   z-threshold for recognizing a press that
                                  changes the values returned by readData().
                                  Default value is Z_THRESHOLD.
    @param    Z_Threshold_int     z-threshold for clearing the flag returned by
                                  tirqTouched(). If touch pressure falls below
                                  this threshold, the flag is cleared so that
                                  tirqTouched() will return false. Default
                                  value is Z_THRESHOLD_INT (a value less than
                                  Z_THRESHOLD).
  */
  /**************************************************************************/
	void setThresholds(int16_t Z_Threshold_press = Z_THRESHOLD, int16_t Z_Threshold_interrupt = Z_THRESHOLD_INT) {
	  Z_Threshold = Z_Threshold_press; Z_Threshold_Int = Z_Threshold_interrupt;
	  }

  /**************************************************************************/
  /*!
    @brief    Get z-threshold for recognizing a press.
    @returns  z-threshold for recognizing a press that changes the values
              returned by readData(), Z_Threshold.
  */
  /**************************************************************************/
	int16_t Zthreshold() { return(Z_Threshold); }

  /**************************************************************************/
  /*!
    @brief    Get z-threshold for clearing tirqTouched() flag.
    @returns  z-threshold for clearing the flag returned by tirqTouched(),
              Z_Threshold_Int.
  */
  /**************************************************************************/
	int16_t Zthreshold_Int() { return(Z_Threshold_Int); }

  /**************************************************************************/
  /*!
    @brief    Set isrWake value.
    @param    value   The value to which to set isrWake.
  */
  /**************************************************************************/
	void set_isrWake(bool value) { isrWake = value; }

};

#ifndef ISR_PREFIX
  #if defined(ESP8266)
    #define ISR_PREFIX ICACHE_RAM_ATTR
  #elif defined(ESP32)
    // TODO: should this also be ICACHE_RAM_ATTR ??
    #define ISR_PREFIX IRAM_ATTR
  #else
    #define ISR_PREFIX
  #endif
#endif

#endif // XPT2046_Touchscreen_TT_h
