/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#if defined (ARDUINO)
 #include <Print.h>
#endif

#include <stdint.h>
#include <stdarg.h>

#include "platforms/common.hpp"
#include "misc/enum.hpp"
#include "misc/colortype.hpp"
#include "misc/pixelcopy.hpp"
#include "misc/DataWrapper.hpp"
#include "lgfx_fonts.hpp"
#include "Touch.hpp"
#include "panel/Panel_Device.hpp"
#include "../boards.hpp"

namespace lgfx
{
 inline namespace v1
 {

#if defined ( _MSVC_LANG )
#define LGFX_INLINE                        inline
#define LGFX_INLINE_T template<typename T> inline
#else
#define LGFX_INLINE                        __attribute__ ((always_inline)) inline
#define LGFX_INLINE_T template<typename T> __attribute__ ((always_inline)) inline
#endif
//----------------------------------------------------------------------------

#if !defined (ARDUINO) || defined (ARDUINO_ARCH_MBED_RP2040) || defined (ARDUINO_ARCH_RP2040)
#define LGFX_PRINTF_ENABLED
#endif


  class LGFXBase
#if defined (ARDUINO)
  : public Print
#endif
  {
  public:
    LGFXBase(void) = default;
    virtual ~LGFXBase(void) = default;

    /// @brief Converts RGB information to 8-bit color code.
    /// @param r red
    /// @param g green
    /// @param b blue
    /// @return 8-bit color code
    LGFX_INLINE static constexpr uint8_t  color332(uint8_t r, uint8_t g, uint8_t b) { return lgfx::color332(r, g, b); }
    /// @brief Converts RGB information to 16-bit color code.
    /// @param r red
    /// @param g green
    /// @param b blue
    /// @return 16-bit color code
    LGFX_INLINE static constexpr uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return lgfx::color565(r, g, b); }
    /// @brief Converts RGB information to 24-bit color code.
    /// @param r red
    /// @param g green
    /// @param b blue
    /// @return 24-bit color code
    LGFX_INLINE static constexpr uint32_t color888(uint8_t r, uint8_t g, uint8_t b) { return lgfx::color888(r, g, b); }

    /// @brief Endian conversion of 16-bit RGB565 format color code.
    /// @param r red
    /// @param g green
    /// @param b blue
    /// @return 16-bit color code (endian converted)
    /// @note This function is used to draw directly to the Sprite's buffer memory.
    LGFX_INLINE static constexpr uint16_t swap565( uint8_t r, uint8_t g, uint8_t b) { return lgfx::swap565( r, g, b); }
    /// @brief Endian conversion of 24-bit RGB888 format color code.
    /// @param r red
    /// @param g green
    /// @param b blue
    /// @return 24-bit color code (endian converted)
    /// @note This function is used to draw directly to the Sprite's buffer memory.
    LGFX_INLINE static constexpr uint32_t swap888( uint8_t r, uint8_t g, uint8_t b) { return lgfx::swap888( r, g, b); }

    /// @brief Convert 16-bit RGB565 format color code to 8-bit RGB332 format.
    /// @param rgb565 16-bit color code
    /// @return 8-bit color code
    LGFX_INLINE static uint8_t  color16to8( uint32_t rgb565) { return lgfx::color_convert<rgb332_t,rgb565_t>(rgb565); }
    /// @brief Convert 8-bit RGB332 format color code to 16-bit RGB565 format.
    /// @param rgb332 16-bit color code
    /// @return 16-bit color code
    LGFX_INLINE static uint16_t color8to16( uint32_t rgb332) { return lgfx::color_convert<rgb565_t,rgb332_t>(rgb332); }
    /// @brief Convert 16-bit RGB565 format color code to 24-bit RGB888 format.
    /// @param rgb565 16-bit color code
    /// @return 24-bit color code
    LGFX_INLINE static uint32_t color16to24(uint32_t rgb565) { return lgfx::color_convert<rgb888_t,rgb565_t>(rgb565); }
    /// @brief Convert 24-bit RGB888 format color code to 16-bit RGB565 format.
    /// @param rgb888 24-bit color code
    /// @return 16-bit color code
    LGFX_INLINE static uint16_t color24to16(uint32_t rgb888) { return lgfx::color_convert<rgb565_t,rgb888_t>(rgb888); }

    /// @brief Specifies the color used to draw the screen.
    /// @param r red
    /// @param g green
    /// @param b blue
    LGFX_INLINE   void setColor(uint8_t r, uint8_t g, uint8_t b) { setColor(lgfx::color888(r,g,b)); }
    /// @brief Specifies the color used to draw the screen.
    /// @param color color code
    LGFX_INLINE_T void setColor(T color) { setRawColor(_write_conv.convert(color)); }
    LGFX_INLINE   void setRawColor(uint32_t c) { *((uint32_t*)&_color) = c; }
    LGFX_INLINE   uint32_t getRawColor(void) const { return *((uint32_t*)&_color); }
    LGFX_INLINE_T void setBaseColor(T c) { _base_rgb888 = hasPalette() ? c : convert_to_rgb888(c); }
    LGFX_INLINE   uint32_t getBaseColor(void) const { return _base_rgb888; }
    LGFX_INLINE   color_conv_t* getColorConverter(void) { return &_write_conv; }
    LGFX_INLINE   color_depth_t getColorDepth(void) const { return _write_conv.depth; }

    /// @brief Allocate bus for screen communication.
    /// @param transaction If true, transaction processing is performed.
    /// @note Although bus allocation and release are automatically performed when drawing functions are called,
    /// using startWrite and endWrite before and after the drawing process suppresses bus allocation
    /// and release and improves drawing speed.
    /// In the case of electronic paper (EPD), drawing after startWrite() is reflected on the screen by calling endWrite().
    LGFX_INLINE   void startWrite(bool transaction = true) { _panel->startWrite(transaction); }
    /// @brief Release bus for screen communication.
    LGFX_INLINE   void endWrite(void)                      { _panel->endWrite(); }
    LGFX_INLINE   void beginTransaction(void)              { _panel->beginTransaction(); }
    LGFX_INLINE   void endTransaction(void)                { _panel->endTransaction(); }
    LGFX_INLINE   uint32_t getStartCount(void) const  { return _panel->getStartCount(); }

    LGFX_INLINE   void setWindow(uint_fast16_t xs, uint_fast16_t ys, uint_fast16_t xe, uint_fast16_t ye) { _panel->setWindow(xs, ys, xe, ye); }
    LGFX_INLINE   void writePixel(int32_t x, int32_t y)  { if (x >= _clip_l && x <= _clip_r && y >= _clip_t && y <= _clip_b) writeFillRectPreclipped(x, y, 1, 1); }
    LGFX_INLINE_T void writePixel      ( int32_t x, int32_t y                      , const T& color) { setColor(color); writePixel    (x, y      ); }
    LGFX_INLINE_T void writeFastVLine  ( int32_t x, int32_t y           , int32_t h, const T& color) { setColor(color); writeFastVLine(x, y   , h); }
                  void writeFastVLine  ( int32_t x, int32_t y           , int32_t h);
    LGFX_INLINE_T void writeFastHLine  ( int32_t x, int32_t y, int32_t w           , const T& color) { setColor(color); writeFastHLine(x, y, w   ); }
                  void writeFastHLine  ( int32_t x, int32_t y, int32_t w);
    LGFX_INLINE_T void writeFillRect   ( int32_t x, int32_t y, int32_t w, int32_t h, const T& color) { setColor(color); writeFillRect (x, y, w, h); }
                  void writeFillRect   ( int32_t x, int32_t y, int32_t w, int32_t h);
    LGFX_INLINE_T void writeFillRectPreclipped( int32_t x, int32_t y, int32_t w, int32_t h, const T& color) { setColor(color); writeFillRectPreclipped(x, y, w, h); }
    LGFX_INLINE   void writeFillRectPreclipped( int32_t x, int32_t y, int32_t w, int32_t h)                 { _panel->writeFillRectPreclipped(x, y, w, h, getRawColor()); }
    LGFX_INLINE_T void writeColor      ( const T& color, uint32_t length) { if (0 == length) return; setColor(color);               _panel->writeBlock(getRawColor(), length); }
    LGFX_INLINE_T void pushBlock       ( const T& color, uint32_t length) { if (0 == length) return; setColor(color); startWrite(); _panel->writeBlock(getRawColor(), length); endWrite(); }

    /// @brief Draw a pixel.
    /// @param x X-coordinate
    /// @param y Y-coordinate
    /// @note Draws in the color specified by setColor().
    LGFX_INLINE   void drawPixel       ( int32_t x, int32_t y) { if (x >= _clip_l && x <= _clip_r && y >= _clip_t && y <= _clip_b) { _panel->drawPixelPreclipped(x, y, getRawColor()); } }
    /// @brief Draw a pixel.
    /// @param x X-coordinate
    /// @param y Y-coordinate
    /// @param color Color to draw with
    LGFX_INLINE_T void drawPixel       ( int32_t x, int32_t y                                 , const T& color) { setColor(color); drawPixel    (x, y         ); }
    /// @brief Draw a vertical line.
    /// @param x Top-most X-coordinate
    /// @param y Top-most Y-coordinate
    /// @param h Height in pixels
    /// @param color Color to draw with
    LGFX_INLINE_T void drawFastVLine   ( int32_t x, int32_t y           , int32_t h           , const T& color) { setColor(color); drawFastVLine(x, y   , h   ); }
    /// @brief Draw a vertical line.
    /// @param x Top-most X-coordinate
    /// @param y Top-most Y-coordinate
    /// @param h Height in pixels
    /// @note Draws in the color specified by setColor().
                  void drawFastVLine   ( int32_t x, int32_t y           , int32_t h);
    /// @brief Draw a horizontal line.
    /// @param x Left-most X-coordinate
    /// @param y Left-most Y-coordinate
    /// @param w Width in pixels
    /// @param color Color to draw with
    LGFX_INLINE_T void drawFastHLine   ( int32_t x, int32_t y, int32_t w                      , const T& color) { setColor(color); drawFastHLine(x, y, w      ); }
    /// @brief Draw a horizontal line.
    /// @param x Left-most X-coordinate
    /// @param y Left-most Y-coordinate
    /// @param w Width in pixels
    /// @note Draws in the color specified by setColor().
                  void drawFastHLine   ( int32_t x, int32_t y, int32_t w);
    /// @brief  Fill a rectangle.
    /// @param x Top-left-corner X-coordinate
    /// @param y Top-left-corner Y-coordinate
    /// @param w Width in pixels
    /// @param h Height in pixels
    /// @param color Color to fill with
    LGFX_INLINE_T void fillRect        ( int32_t x, int32_t y, int32_t w, int32_t h           , const T& color) { setColor(color); fillRect     (x, y, w, h   ); }
    /// @brief  Fill a rectangle.
    /// @param x Top-left-corner X-coordinate
    /// @param y Top-left-corner Y-coordinate
    /// @param w Width in pixels
    /// @param h Height in pixels
    /// @note Draws in the color specified by setColor().
                  void fillRect        ( int32_t x, int32_t y, int32_t w, int32_t h);
    /// @brief Draw a rectangle outline.
    /// @param x Top-left-corner X-coordinate
    /// @param y Top-left-corner Y-coordinate
    /// @param w Width in pixels
    /// @param h Height in pixels
    /// @param color Color to fill with
    LGFX_INLINE_T void drawRect        ( int32_t x, int32_t y, int32_t w, int32_t h           , const T& color) { setColor(color); drawRect     (x, y, w, h   ); }
    /// @brief Draw a rectangle outline.
    /// @param x Top-left-corner X-coordinate
    /// @param y Top-left-corner Y-coordinate
    /// @param w Width in pixels
    /// @param h Height in pixels
    /// @note Draws in the color specified by setColor().
                  void drawRect        ( int32_t x, int32_t y, int32_t w, int32_t h);
    LGFX_INLINE_T void drawRoundRect   ( int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, const T& color) { setColor(color); drawRoundRect(x, y, w, h, r); }
                  void drawRoundRect   ( int32_t x, int32_t y, int32_t w, int32_t h, int32_t r);
    LGFX_INLINE_T void fillRoundRect   ( int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, const T& color) { setColor(color); fillRoundRect(x, y, w, h, r); }
                  void fillRoundRect   ( int32_t x, int32_t y, int32_t w, int32_t h, int32_t r);
    LGFX_INLINE_T void drawCircle      ( int32_t x, int32_t y                      , int32_t r, const T& color) { setColor(color); drawCircle   (x, y      , r); }
                  void drawCircle      ( int32_t x, int32_t y                      , int32_t r);
    LGFX_INLINE_T void fillCircle      ( int32_t x, int32_t y                      , int32_t r, const T& color) { setColor(color); fillCircle   (x, y      , r); }
                  void fillCircle      ( int32_t x, int32_t y                      , int32_t r);
    LGFX_INLINE_T void drawEllipse     ( int32_t x, int32_t y, int32_t rx, int32_t ry         , const T& color) { setColor(color); drawEllipse  (x, y, rx, ry ); }
                  void drawEllipse     ( int32_t x, int32_t y, int32_t rx, int32_t ry);
    LGFX_INLINE_T void fillEllipse     ( int32_t x, int32_t y, int32_t rx, int32_t ry         , const T& color) { setColor(color); fillEllipse  (x, y, rx, ry ); }
                  void fillEllipse     ( int32_t x, int32_t y, int32_t rx, int32_t ry);
    LGFX_INLINE_T void drawLine        ( int32_t x0, int32_t y0, int32_t x1, int32_t y1       , const T& color) { setColor(color); drawLine     (x0,y0,x1, y1 ); }
                  void drawLine        ( int32_t x0, int32_t y0, int32_t x1, int32_t y1);
    LGFX_INLINE_T void drawTriangle    ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const T& color)  { setColor(color); drawTriangle(x0, y0, x1, y1, x2, y2); }
                  void drawTriangle    ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    LGFX_INLINE_T void fillTriangle    ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const T& color)  { setColor(color); fillTriangle(x0, y0, x1, y1, x2, y2); }
                  void fillTriangle    ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    LGFX_INLINE_T void drawBezier      ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, const T& color)  { setColor(color); drawBezier(x0, y0, x1, y1, x2, y2); }
                  void drawBezier      ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    LGFX_INLINE_T void drawBezier      ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, const T& color)  { setColor(color); drawBezier(x0, y0, x1, y1, x2, y2, x3, y3); }
                  void drawBezier      ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3);
    LGFX_INLINE_T void drawEllipseArc  ( int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, const T& color) { setColor(color); drawEllipseArc( x, y, r0x, r1x, r0y, r1y, angle0, angle1); }
                  void drawEllipseArc  ( int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1);
    LGFX_INLINE_T void fillEllipseArc  ( int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1, const T& color) { setColor(color); fillEllipseArc( x, y, r0x, r1x, r0y, r1y, angle0, angle1); }
                  void fillEllipseArc  ( int32_t x, int32_t y, int32_t r0x, int32_t r1x, int32_t r0y, int32_t r1y, float angle0, float angle1);
    LGFX_INLINE_T void drawArc         ( int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, const T& color) { setColor(color); drawEllipseArc( x, y, r0, r1, r0, r1, angle0, angle1); }
                  void drawArc         ( int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1)                 {                  drawEllipseArc( x, y, r0, r1, r0, r1, angle0, angle1); }
    LGFX_INLINE_T void fillArc         ( int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1, const T& color) { setColor(color); fillEllipseArc( x, y, r0, r1, r0, r1, angle0, angle1); }
                  void fillArc         ( int32_t x, int32_t y, int32_t r0, int32_t r1, float angle0, float angle1)                 {                  fillEllipseArc( x, y, r0, r1, r0, r1, angle0, angle1); }
    LGFX_INLINE_T void drawCircleHelper( int32_t x, int32_t y, int32_t r, uint_fast8_t cornername                , const T& color) { setColor(color); drawCircleHelper(x, y, r, cornername    ); }
                  void drawCircleHelper( int32_t x, int32_t y, int32_t r, uint_fast8_t cornername);
    LGFX_INLINE_T void fillCircleHelper( int32_t x, int32_t y, int32_t r, uint_fast8_t corners, int32_t delta, const T& color)  { setColor(color); fillCircleHelper(x, y, r, corners, delta); }
                  void fillCircleHelper( int32_t x, int32_t y, int32_t r, uint_fast8_t corners, int32_t delta);
    LGFX_INLINE_T void floodFill( int32_t x, int32_t y, const T& color) { setColor(color); floodFill(x, y); }
                  void floodFill( int32_t x, int32_t y                );
    LGFX_INLINE_T void paint    ( int32_t x, int32_t y, const T& color) { setColor(color); floodFill(x, y); }
    LGFX_INLINE   void paint    ( int32_t x, int32_t y                ) {                  floodFill(x, y); }

    LGFX_INLINE_T void fillAffine(const float matrix[6], int32_t w, int32_t h, const T& color) { setColor(color); fillAffine(matrix, w, h); }
                  void fillAffine(const float matrix[6], int32_t w, int32_t h);

    LGFX_INLINE_T void drawGradientHLine( int32_t x, int32_t y, int32_t w, const T& colorstart, const T& colorend ) { drawGradientLine( x, y, x + w - 1, y, colorstart, colorend ); }
    LGFX_INLINE_T void drawGradientVLine( int32_t x, int32_t y, int32_t h, const T& colorstart, const T& colorend ) { drawGradientLine( x, y, x, y + h - 1, colorstart, colorend ); }
    LGFX_INLINE_T void drawGradientLine ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, const T& colorstart, const T& colorend ) { draw_gradient_line( x0, y0, x1, y1, convert_to_rgb888(colorstart), convert_to_rgb888(colorend) ); }


//----------------------------------------------------------------------------

    template <const uint32_t N>
                  const colors_t createGradient( const rgb888_t(&colors)[N] )                   { const colors_t ret = { colors, N };     return ret; }
                  const colors_t createGradient( const rgb888_t* colors, const uint32_t count ) { const colors_t ret = { colors, count }; return ret; }
    template <typename T=rgb888_t>
                  T mapGradient( float val, float min, float max, const colors_t gr )                      { rgb888_t c=map_gradient(val, min, max, gr);            return T(c.r, c.g, c.b); }
    template <typename T=rgb888_t>
                  T mapGradient( float val, float min, double max, const rgb888_t *colors, uint32_t count ) { rgb888_t c=map_gradient(val, min, max, colors, count); return T(c.r, c.g, c.b); }

    LGFX_INLINE_T void drawSmoothLine   ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, const T& color )                     { drawWideLine( x0, y0, x1, y1, 0.5f, color); }
                  void drawGradientLine ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, const colors_t colors )              { draw_gradient_line(x0, y0, x1, y1, colors); }
    LGFX_INLINE_T void drawWideLine     ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, float r, const T& color)             { draw_wedgeline(x0, y0, x1, y1, r, r, convert_to_rgb888(color)); }
                  void drawWideLine     ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, float r, const colors_t colors )     { draw_gradient_wedgeline(x0, y0, x1, y1, r, r, colors); }
    LGFX_INLINE_T void drawWedgeLine    ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, float r0, float r1, const T& color ) { draw_wedgeline(x0, y0, x1, y1, r0, r1, convert_to_rgb888(color)); }
                  void drawWedgeLine    ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, float r0, float r1, const colors_t colors ) { draw_gradient_wedgeline(x0, y0, x1, y1, r0, r1, colors); }
    LGFX_INLINE_T void drawSpot         ( int32_t x, int32_t y, float r, const T& color )    { draw_wedgeline(x, y, x, y, r, r, convert_to_rgb888(color)); }
                  void drawGradientSpot ( int32_t x, int32_t y, float r, const colors_t gr ) { draw_gradient_wedgeline(x, y, x, y, r, r, gr); }
                  void drawGradientHLine( int32_t x, int32_t y, uint32_t w, const colors_t colors ) { draw_gradient_line(x, y, x+w, y, colors); }
                  void drawGradientVLine( int32_t x, int32_t y, uint32_t h, const colors_t colors ) { draw_gradient_line(x, y, x, y+h, colors); }
                  void fillGradientRect ( int32_t x, int32_t y, uint32_t w, uint32_t h, const colors_t colors, fill_style_t style=RADIAL)        { fill_rect_gradient(x, y, w, h, colors, style); }
    LGFX_INLINE_T void fillGradientRect ( int32_t x, int32_t y, uint32_t w, uint32_t h, const T& start, const T& end, fill_style_t style=RADIAL) { fill_rect_gradient(x, y, w, h, convert_to_rgb888(start), convert_to_rgb888(end), style); }

//----------------------------------------------------------------------------


    LGFX_INLINE_T void fillSmoothRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, const T& color) { setColor(color); fillSmoothRoundRect(x, y, w, h, r); }
                  void fillSmoothRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r);

    LGFX_INLINE_T void fillSmoothCircle(int32_t x, int32_t y, int32_t r, const T& color) { setColor(color); fillSmoothCircle(x, y, r); }
                  void fillSmoothCircle(int32_t x, int32_t y, int32_t r) { fillSmoothRoundRect(x-r, y-r, r*2+1, r*2+1, r); }

    LGFX_INLINE_T void fillScreen  ( const T& color) { setColor(color); fillRect(0, 0, width(), height()); }
    LGFX_INLINE   void fillScreen  ( void )          {                  fillRect(0, 0, width(), height()); }

    LGFX_INLINE_T void clear       ( const T& color) { setBaseColor(color); clear(); }
    LGFX_INLINE   void clear       ( void )          { setColor(_base_rgb888); fillScreen(); }
    LGFX_INLINE_T void clearDisplay( const T& color) { setBaseColor(color); clear(); }
    LGFX_INLINE   void clearDisplay( void )          { setColor(_base_rgb888); fillScreen(); }

    LGFX_INLINE   void  setPivot(float x, float y) { _xpivot = x; _ypivot = y; }
    LGFX_INLINE   float getPivotX(void) const { return _xpivot; }
    LGFX_INLINE   float getPivotY(void) const { return _ypivot; }

    LGFX_INLINE   int32_t width (void) const { return _panel->width(); }
    LGFX_INLINE   int32_t height(void) const { return _panel->height(); }
    LGFX_INLINE   bool hasPalette (void) const { return _palette_count; }
    LGFX_INLINE   uint32_t getPaletteCount(void) const { return _palette_count; }
    LGFX_INLINE   RGBColor*     getPalette(void) const { return getPalette_impl(); }
    LGFX_INLINE   bool isReadable(void) const { return _panel->isReadable(); }
    LGFX_INLINE   bool isEPD(void) const { return _panel->isEpd(); }
    LGFX_INLINE   bool getSwapBytes(void) const { return _swapBytes; }
    LGFX_INLINE   void setSwapBytes(bool swap) { _swapBytes = swap; }
    LGFX_INLINE   bool isBusShared(void) const { return _panel->isBusShared(); }
    [[deprecated("use isBusShared()")]]
    LGFX_INLINE   bool isSPIShared(void) const { return _panel->isBusShared(); }
                  void display(int32_t x, int32_t y, int32_t w, int32_t h);
    LGFX_INLINE   void display(void) { display(0, 0, 0, 0); }
    LGFX_INLINE   void waitDisplay(void) { _panel->waitDisplay(); }
    LGFX_INLINE   bool displayBusy(void) { return _panel->displayBusy(); }
    LGFX_INLINE   void setAutoDisplay(bool flg) { _panel->setAutoDisplay(flg); }
    LGFX_INLINE   void initDMA(void) { _panel->initDMA(); }
    LGFX_INLINE   void waitDMA(void) { _panel->waitDMA(); }
    LGFX_INLINE   bool dmaBusy(void) { return _panel->dmaBusy(); }

    LGFX_INLINE_T void setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, const T& color) { setBaseColor(color); setScrollRect(x, y, w, h); }

    LGFX_INLINE_T void writePixels(const T*        data, int32_t len           ) { auto pc = create_pc_fast(data      ); _panel->writePixels(&pc, len, false); }
    LGFX_INLINE   void writePixels(const uint16_t* data, int32_t len, bool swap) { auto pc = create_pc_fast(data, swap); _panel->writePixels(&pc, len, false); }
    LGFX_INLINE   void writePixels(const void*     data, int32_t len, bool swap) { auto pc = create_pc_fast(data, swap); _panel->writePixels(&pc, len, false); }

    LGFX_INLINE_T void writePixelsDMA(const T*        data, int32_t len           ) { auto pc = create_pc_fast(data      ); _panel->writePixels(&pc, len, true); }
    LGFX_INLINE   void writePixelsDMA(const uint16_t* data, int32_t len, bool swap) { auto pc = create_pc_fast(data, swap); _panel->writePixels(&pc, len, true); }
    LGFX_INLINE   void writePixelsDMA(const void*     data, int32_t len, bool swap) { auto pc = create_pc_fast(data, swap); _panel->writePixels(&pc, len, true); }

    LGFX_INLINE_T void pushPixels(T*              data, int32_t len           ) { startWrite(); writePixels(data, len      ); endWrite(); }
    LGFX_INLINE   void pushPixels(const uint16_t* data, int32_t len, bool swap) { startWrite(); writePixels(data, len, swap); endWrite(); }
    LGFX_INLINE   void pushPixels(const void*     data, int32_t len, bool swap) { startWrite(); writePixels(data, len, swap); endWrite(); }

    LGFX_INLINE_T void pushPixelsDMA(T*              data, int32_t len           ) { startWrite(); writePixelsDMA(data, len      ); endWrite(); }
    LGFX_INLINE   void pushPixelsDMA(const uint16_t* data, int32_t len, bool swap) { startWrite(); writePixelsDMA(data, len, swap); endWrite(); }
    LGFX_INLINE   void pushPixelsDMA(const void*     data, int32_t len, bool swap) { startWrite(); writePixelsDMA(data, len, swap); endWrite(); }

    template<typename TFunc>
    void effect(int32_t x, int32_t y, int32_t w, int32_t h, TFunc&& effector)
    {
      if (!_clipping(x, y, w, h)) return;
      _panel->effect(x, y, w, h, effector);
    }

    template<typename T>
    void fillRectAlpha(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t alpha, const T& color)
    {
      if (!_clipping(x, y, w, h)) return;
      _panel->writeFillRectAlphaPreclipped(x, y, w, h, convert_to_rgb888(color) | alpha << 24 );
    }

    LGFX_INLINE_T void drawBitmap (int32_t x, int32_t y, const uint8_t* bitmap, int32_t w, int32_t h, const T& color                    ) { draw_bitmap (x, y, bitmap, w, h, _write_conv.convert(color)); }
    LGFX_INLINE_T void drawBitmap (int32_t x, int32_t y, const uint8_t* bitmap, int32_t w, int32_t h, const T& fgcolor, const T& bgcolor) { draw_bitmap (x, y, bitmap, w, h, _write_conv.convert(fgcolor), _write_conv.convert(bgcolor)); }
    LGFX_INLINE_T void drawXBitmap(int32_t x, int32_t y, const uint8_t* bitmap, int32_t w, int32_t h, const T& color                    ) { draw_xbitmap(x, y, bitmap, w, h, _write_conv.convert(color)); }
    LGFX_INLINE_T void drawXBitmap(int32_t x, int32_t y, const uint8_t* bitmap, int32_t w, int32_t h, const T& fgcolor, const T& bgcolor) { draw_xbitmap(x, y, bitmap, w, h, _write_conv.convert(fgcolor), _write_conv.convert(bgcolor)); }

    LGFX_INLINE_T
    void writeIndexedPixels(const uint8_t* data, T* palette, int32_t len, uint8_t depth = 8)
    {
      auto src_depth = (color_depth_t)(depth | color_depth_t::has_palette);
      auto pc = create_pc_fast(data, palette, src_depth);
      _panel->writePixels(&pc, len, false);
    }

    /// Obtains the current scanning line position.
    /// 現在の走査線の位置を取得する。;
    /// @return -1=unsupported. / 0 or more = current scanline position.
    /// @attention This function returns the raw value obtained from the device. Note that screen rotation and offset are not taken into account.
    /// @attention この関数はデバイスから得られる生の値を返す。画面の回転やオフセットは考慮されていないことに注意。;
    int32_t getScanLine(void) { return _panel->getScanLine(); }

    uint8_t getRotation(void) const { return _panel->getRotation(); }
    void setRotation(uint_fast8_t rotation);
    void setColorDepth(int bits) { setColorDepth((color_depth_t)(bits & color_depth_t::bit_mask));}
    void setColorDepth(color_depth_t depth);

    void setAddrWindow(int32_t x, int32_t y, int32_t w, int32_t h);

    void setClipRect(int32_t x, int32_t y, int32_t w, int32_t h);
    void getClipRect(int32_t *x, int32_t *y, int32_t *w, int32_t *h);
    void clearClipRect(void);

    void setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h);
    void getScrollRect(int32_t *x, int32_t *y, int32_t *w, int32_t *h);
    void clearScrollRect(void);

    template<typename T>
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const T* data)
    {
      auto pc = create_pc(data);
      pushImage(x, y, w, h, &pc);
    }

    template<typename T1, typename T2>
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const T1* data, const T2& transparent)
    {
      auto pc = create_pc_tr(data, transparent);
      pushImage(x, y, w, h, &pc);
    }

    template<typename T>
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const void* data, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_palette(data, palette, depth);
      pushImage(x, y, w, h, &pc);
    }

    template<typename T>
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const void* data, uint32_t transparent, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_palette(data, palette, depth, transparent);
      pushImage(x, y, w, h, &pc);
    }

    template<typename T>
    void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, const T* data)
    {
      auto pc = create_pc(data);
      pushImage(x, y, w, h, &pc, true);
    }

    template<typename T>
    void pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, const void* data, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_palette(data, palette, depth);
      pushImage(x, y, w, h, &pc, true);
    }

    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, pixelcopy_t *param, bool use_dma = false);

//----------------------------------------------------------------------------

    template<typename T>
    void pushImageRotateZoom(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const T* data)
    {
      auto pc = create_pc(data);
      push_image_rotate_zoom(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }

    template<typename T1, typename T2>
    void pushImageRotateZoom(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const T1* data, const T2& transparent)
    {
      auto pc = create_pc_tr(data, transparent);
      push_image_rotate_zoom(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }

    template<typename T>
    void pushImageRotateZoom(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const void* data, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_palette(data, palette, depth);
      push_image_rotate_zoom(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }

    template<typename T>
    void pushImageRotateZoom(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const void* data, uint32_t transparent, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_palette(data, palette, depth, transparent);
      push_image_rotate_zoom(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }


    template<typename T>
    void pushImageRotateZoomWithAA(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const T* data)
    {
      auto pc = create_pc_antialias(data);
      push_image_rotate_zoom_aa(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }

    template<typename T1, typename T2>
    void pushImageRotateZoomWithAA(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const T1* data, const T2& transparent)
    {
      auto pc = create_pc_tr_antialias(data, transparent);
      push_image_rotate_zoom_aa(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }

    template<typename T>
    void pushImageRotateZoomWithAA(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const void* data, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_antialias(data, palette, depth);
      push_image_rotate_zoom_aa(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }

    template<typename T>
    void pushImageRotateZoomWithAA(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const void* data, uint32_t transparent, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_antialias(data, palette, depth, transparent);
      push_image_rotate_zoom_aa(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, &pc);
    }

//----------------------------------------------------------------------------

    template<typename T>
    void pushImageAffine(const float matrix[6], int32_t w, int32_t h, const T* data)
    {
      auto pc = create_pc(data);
      push_image_affine(matrix, w, h, &pc);
    }

    template<typename T1, typename T2>
    void pushImageAffine(const float matrix[6], int32_t w, int32_t h, const T1* data, const T2& transparent)
    {
      auto pc = create_pc_tr(data, transparent);
      push_image_affine(matrix, w, h, &pc);
    }

    template<typename T>
    void pushImageAffine(const float matrix[6], int32_t w, int32_t h, const void* data, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_palette(data, palette, depth);
      push_image_affine(matrix, w, h, &pc);
    }

    template<typename T>
    void pushImageAffine(const float matrix[6], int32_t w, int32_t h, const void* data, uint32_t transparent, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_palette(data, palette, depth, transparent);
      push_image_affine(matrix, w, h, &pc);
    }


    template<typename T>
    void pushImageAffineWithAA(const float matrix[6], int32_t w, int32_t h, const T* data)
    {
      auto pc = create_pc_antialias(data);
      push_image_affine_aa(matrix, w, h, &pc);
    }

    template<typename T1, typename T2>
    void pushImageAffineWithAA(const float matrix[6], int32_t w, int32_t h, const T1* data, const T2& transparent)
    {
      auto pc = create_pc_tr_antialias(data, transparent);
      push_image_affine_aa(matrix, w, h, &pc);
    }

    template<typename T>
    void pushImageAffineWithAA(const float matrix[6], int32_t w, int32_t h, const void* data, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_antialias(data, palette, depth);
      push_image_affine_aa(matrix, w, h, &pc);
    }

    template<typename T>
    void pushImageAffineWithAA(const float matrix[6], int32_t w, int32_t h, const void* data, uint32_t transparent, color_depth_t depth, const T* palette)
    {
      auto pc = create_pc_antialias(data, palette, depth, transparent);
      push_image_affine_aa(matrix, w, h, &pc);
    }

//----------------------------------------------------------------------------

    LGFX_INLINE_T void pushGrayscaleImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t* image, color_depth_t depth, const T& forecolor, const T& backcolor) { push_grayimage(x, y, w, h, image, depth, convert_to_rgb888(forecolor), convert_to_rgb888(backcolor)); }
    LGFX_INLINE_T void pushGrayscaleImageRotateZoom(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const uint8_t* image, color_depth_t depth, const T& forecolor, const T& backcolor) { push_grayimage_rotate_zoom(dst_x, dst_y, src_x, src_y, angle, zoom_x, zoom_y, w, h, image, depth, convert_to_rgb888(forecolor), convert_to_rgb888(backcolor)); }
    LGFX_INLINE_T void pushGrayscaleImageAffine(const float matrix[6], int32_t w, int32_t h, const uint8_t* image, color_depth_t depth, const T& forecolor, const T& backcolor) { push_grayimage_affine(matrix, w, h, image, depth, convert_to_rgb888(forecolor), convert_to_rgb888(backcolor)); }

//----------------------------------------------------------------------------

    /// read RGB565 16bit color
    uint16_t readPixel(int32_t x, int32_t y)
    {
      if (x < 0 || x >= width() || y < 0 || y >= height()) return 0;

      pixelcopy_t p(nullptr, swap565_t::depth, _read_conv.depth, false, getPalette());
      uint16_t data = 0;

      _panel->readRect(x, y, 1, 1, &data, &p);

      return (data<<8)+(data>>8);
    }

    /// read RGB888 24bit color
    RGBColor readPixelRGB(int32_t x, int32_t y)
    {
      RGBColor data[1];
      if (x < 0 || x >= width() || y < 0 || y >= height()) return data[0];

      pixelcopy_t p(nullptr, bgr888_t::depth, _read_conv.depth, false, getPalette());

      _panel->readRect(x, y, 1, 1, data, &p);

      return data[0];
    }

    LGFX_INLINE
    void readRectRGB( int32_t x, int32_t y, int32_t w, int32_t h, uint8_t* data) { readRectRGB(x, y, w, h, (bgr888_t*)data); }
    void readRectRGB( int32_t x, int32_t y, int32_t w, int32_t h, RGBColor* data)
    {
      pixelcopy_t p(nullptr, bgr888_t::depth, _read_conv.depth, false, getPalette());
      read_rect(x, y, w, h, data, &p);
    }

    template<typename T> inline
    void readRect( int32_t x, int32_t y, int32_t w, int32_t h, T* data)
    {
      auto src_palette = getPalette();
      pixelcopy_t p(nullptr, get_depth<T>::value, _read_conv.depth, false, src_palette);
      if (std::is_same<rgb565_t, T>::value || std::is_same<rgb888_t, T>::value || std::is_same<argb8888_t, T>::value || std::is_same<grayscale_t, T>::value || p.fp_copy == nullptr)
      {
        p.no_convert = false;
        if (src_palette)
        {
          p.fp_copy = pixelcopy_t::copy_palette_affine<T, bgr888_t>;
        }
        else
        {
          p.fp_copy = pixelcopy_t::get_fp_copy_rgb_affine_dst<T>(_read_conv.depth);
        }
      }
      read_rect(x, y, w, h, data, &p);
    }

    void readRect(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t*  data);
    void readRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* data);
    void readRect(int32_t x, int32_t y, int32_t w, int32_t h, void*     data);

    void scroll(int_fast16_t dx, int_fast16_t dy = 0);
    void copyRect(int32_t dst_x, int32_t dst_y, int32_t w, int32_t h, int32_t src_x, int32_t src_y);


    [[deprecated("use IFont")]]
    void setCursor( int32_t x, int32_t y, uint8_t      font) { setCursor(x, y); setFont(fontdata[font]); }
    void setCursor( int32_t x, int32_t y, const IFont* font) { setCursor(x, y); setFont(font); }
    void setCursor( int32_t x, int32_t y)                    { _decoderState = utf8_state0; _filled_x = 0; _cursor_x = x; _cursor_y = y; }
    int32_t getCursorX(void) const { return _cursor_x; }
    int32_t getCursorY(void) const { return _cursor_y; }
    void setTextStyle(const TextStyle& text_style) { _text_style = text_style; }
    const TextStyle& getTextStyle(void) const { return _text_style; }
    void setTextSize(float size) { setTextSize(size, size); }
    void setTextSize(float sx, float sy) { _text_style.size_x = (sx > 0) ? sx : 1; _text_style.size_y = (sy > 0) ? sy : 1; }
    float getTextSizeX(void) const { return _text_style.size_x; }
    float getTextSizeY(void) const { return _text_style.size_y; }
    //[[deprecated("use textdatum_t")]]
    void setTextDatum(uint8_t datum) { _text_style.datum = (textdatum_t)datum; }
    void setTextDatum(textdatum_t datum) { _text_style.datum = datum; }
    textdatum_t getTextDatum(void) const { return _text_style.datum; }
    void setTextPadding(uint32_t padding_x) { _text_style.padding_x = padding_x; }
    uint32_t getTextPadding(void) const { return _text_style.padding_x; }
    void setTextWrap( bool wrapX, bool wrapY = false) { _textwrap_x = wrapX; _textwrap_y = wrapY; }
    void setTextScroll(bool scroll) { _textscroll = scroll; if (_cursor_x < this->_sx) { _cursor_x = this->_sx; } if (_cursor_y < this->_sy) { _cursor_y = this->_sy; } }

    template<typename T>
    void setTextColor(T color) {
      _text_style.fore_rgb888 = _text_style.back_rgb888 = this->hasPalette() ? color : convert_to_rgb888(color);
    }
    template<typename T1, typename T2>
    void setTextColor(T1 fgcolor, T2 bgcolor) {
      if (!this->hasPalette()) {
        _text_style.fore_rgb888 = convert_to_rgb888(fgcolor);
        _text_style.back_rgb888 = convert_to_rgb888(bgcolor);
      } else {
        _text_style.fore_rgb888 = fgcolor;
        _text_style.back_rgb888 = bgcolor;
      }
    }

    [[deprecated("use IFont")]]
    int32_t fontHeight(uint8_t font) const { return (int32_t)(((const BaseFont*)fontdata[font])->height * _text_style.size_y); }
    int32_t fontHeight(const IFont* font) const;
    int32_t fontHeight(void) const { return (int32_t)(_font_metrics.height * _text_style.size_y); }
    int32_t fontWidth(uint8_t font) const { return (int32_t)(((const BaseFont*)fontdata[font])->width * _text_style.size_x); }
    int32_t fontWidth(const IFont* font) const;
    int32_t fontWidth(void) const { return (int32_t)(_font_metrics.width * _text_style.size_x); }
    int32_t textLength(const char *string, int32_t width);
    int32_t textWidth(const char *string) { return textWidth(string, _font); };
    int32_t textWidth(const char *string, const IFont* font);

    [[deprecated("use IFont")]]
    inline size_t drawString(const char *string, int32_t x, int32_t y, uint8_t      font) { return draw_string(string, x, y, _text_style.datum, fontdata[font]); }
    inline size_t drawString(const char *string, int32_t x, int32_t y                   ) { return draw_string(string, x, y, _text_style.datum); }
    inline size_t drawString(const char *string, int32_t x, int32_t y, const IFont* font) { return draw_string(string, x, y, _text_style.datum, font); }

    [[deprecated("use IFont")]]
    inline size_t drawNumber(long long_num, int32_t poX, int32_t poY, uint8_t font) { return drawNumber(long_num, poX, poY, fontdata[font]); }
    inline size_t drawNumber(long long_num, int32_t poX, int32_t poY              ) { return drawNumber(long_num, poX, poY, _font         ); }
           size_t drawNumber(long long_num, int32_t poX, int32_t poY, const IFont* font);

    [[deprecated("use IFont")]]
    inline size_t drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY, uint8_t font) { return drawFloat(floatNumber, dp, poX, poY, fontdata[font]); }
    inline size_t drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY              ) { return drawFloat(floatNumber, dp, poX, poY, _font         ); }
           size_t drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY, const IFont* font);

    [[deprecated("use IFont")]] inline size_t drawCentreString(const char *string, int32_t x, int32_t y, uint8_t font) { return draw_string(string, x, y, textdatum_t::top_center, fontdata[font]); }
    [[deprecated("use IFont")]] inline size_t drawCenterString(const char *string, int32_t x, int32_t y, uint8_t font) { return draw_string(string, x, y, textdatum_t::top_center, fontdata[font]); }
    [[deprecated("use IFont")]] inline size_t drawRightString( const char *string, int32_t x, int32_t y, uint8_t font) { return draw_string(string, x, y, textdatum_t::top_right , fontdata[font]); }
    inline size_t drawCentreString(const char *string, int32_t x, int32_t y, const IFont* font) { return draw_string(string, x, y, textdatum_t::top_center, font); }
    inline size_t drawCenterString(const char *string, int32_t x, int32_t y, const IFont* font) { return draw_string(string, x, y, textdatum_t::top_center, font); }
    inline size_t drawRightString( const char *string, int32_t x, int32_t y, const IFont* font) { return draw_string(string, x, y, textdatum_t::top_right , font); }
    inline size_t drawCentreString(const char *string, int32_t x, int32_t y                   ) { return draw_string(string, x, y, textdatum_t::top_center); }
    inline size_t drawCenterString(const char *string, int32_t x, int32_t y                   ) { return draw_string(string, x, y, textdatum_t::top_center); }
    inline size_t drawRightString( const char *string, int32_t x, int32_t y                   ) { return draw_string(string, x, y, textdatum_t::top_right ); }

  #if defined (ARDUINO)
    inline int32_t textLength(const String& string, int32_t width) { return textLength(string.c_str(), width); }
    inline int32_t textWidth(const String& string) { return textWidth(string.c_str()); }
    inline int32_t textWidth(const String& string, const IFont* font) { return textWidth(string.c_str(), font); }

    [[deprecated("use IFont")]]
    inline size_t drawString(const String& string, int32_t x, int32_t y, uint8_t      font) { return draw_string(string.c_str(), x, y, _text_style.datum, fontdata[font]); }
    inline size_t drawString(const String& string, int32_t x, int32_t y, const IFont* font) { return draw_string(string.c_str(), x, y, _text_style.datum,          font ); }
    inline size_t drawString(const String& string, int32_t x, int32_t y                   ) { return draw_string(string.c_str(), x, y, _text_style.datum); }

    [[deprecated("use IFont")]] inline size_t drawCentreString(const String& string, int32_t x, int32_t y, uint8_t font) { return draw_string(string.c_str(), x, y, textdatum_t::top_center, fontdata[font]); }
    [[deprecated("use IFont")]] inline size_t drawCenterString(const String& string, int32_t x, int32_t y, uint8_t font) { return draw_string(string.c_str(), x, y, textdatum_t::top_center, fontdata[font]); }
    [[deprecated("use IFont")]] inline size_t drawRightString( const String& string, int32_t x, int32_t y, uint8_t font) { return draw_string(string.c_str(), x, y, textdatum_t::top_right , fontdata[font]); }
    inline size_t drawCentreString(const String& string, int32_t x, int32_t y, const IFont* font) { return draw_string(string.c_str(), x, y, textdatum_t::top_center, font); }
    inline size_t drawCenterString(const String& string, int32_t x, int32_t y, const IFont* font) { return draw_string(string.c_str(), x, y, textdatum_t::top_center, font); }
    inline size_t drawRightString( const String& string, int32_t x, int32_t y, const IFont* font) { return draw_string(string.c_str(), x, y, textdatum_t::top_right , font); }
    inline size_t drawCentreString(const String& string, int32_t x, int32_t y                   ) { return draw_string(string.c_str(), x, y, textdatum_t::top_center); }
    inline size_t drawCenterString(const String& string, int32_t x, int32_t y                   ) { return draw_string(string.c_str(), x, y, textdatum_t::top_center); }
    inline size_t drawRightString( const String& string, int32_t x, int32_t y                   ) { return draw_string(string.c_str(), x, y, textdatum_t::top_right ); }
  #endif

           size_t drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font);
    inline size_t drawChar(uint16_t uniCode, int32_t x, int32_t y) { int32_t dummy_filled_x = 0; return _font->drawChar(this, x, y, uniCode, &_text_style, &_font_metrics, dummy_filled_x); }

    template<typename T>
    inline size_t drawChar(int32_t x, int32_t y, uint16_t uniCode, T color, T bg, float size) { return drawChar(x, y, uniCode, color, bg, size, size); }
    template<typename T>
    inline size_t drawChar(int32_t x, int32_t y, uint16_t uniCode, T color, T bg, float size_x, float size_y)
    {
      TextStyle style = _text_style;
      style.back_rgb888 = convert_to_rgb888(color);
      style.fore_rgb888 = convert_to_rgb888(bg);
      style.size_x = size_x;
      style.size_y = size_y;
      int32_t dummy_filled_x = 0;
      return _font->drawChar(this, x, y, uniCode, &style, &_font_metrics, dummy_filled_x);
      //return (fpDrawChar)(this, x, y, uniCode, &style, _font);
    }

    [[deprecated("use getFont()")]]
    uint_fast8_t getTextFont(void) const
    {
      uint_fast8_t i = 0;
      do {
        if (fontdata[i] == _font) return i;
      } while (fontdata[++i]);
      return 0;
    }

    /// [[deprecated("use setFont(&fonts::Font)")]]
    void setTextFont(int f)
    {
      if (f == 1 && _font && _font->getType() == IFont::font_type_t::ft_gfx) return;
      setFont(fontdata[f]);
    }

    [[deprecated("use setFont(&fonts::Font)")]]
    void setTextFont(const IFont* font = nullptr) { setFont(font); }

    [[deprecated("use setFont()")]]
    void setFreeFont(const IFont* font = nullptr) { setFont(font); }

    LGFX_INLINE const IFont* getFont (void) const { return _font; }

    void setFont(const IFont* font);

    /// load VLW font
    bool loadFont(const uint8_t* array);

    /// load vlw font from filesystem.
    bool loadFont(const char *path)
    {
      unloadFont();
      _font_file.reset(_create_data_wrapper());
      return load_font_with_path(path);
    }


    template <typename T>
    bool loadFont(T &fs, const char *path)
    {
      unloadFont();
      _font_file.reset(new DataWrapperT<T>(&fs));
      return load_font_with_path(path);
    }

    bool loadFont(DataWrapper* data)
    {
      return load_font(data);
    }

    /// unload VLW font
    void unloadFont(void);

    /// show VLW font
    void showFont(uint32_t td = 2000);

    void cp437(bool enable = true) { _text_style.cp437 = enable; }  // AdafruitGFX compatible.

    void setAttribute(attribute_t attr_id, uint8_t param);
    uint8_t getAttribute(attribute_t attr_id);
    uint8_t getAttribute(uint8_t attr_id) { return getAttribute((attribute_t)attr_id); }

    template <typename T>
    void setFileStorage(T& fs) {
      _data_wrapper_factory.reset(new DataWrapperTFactoryT<T>(&fs));
    }

    template <typename T>
    void setFileStorage(T* fs) { _data_wrapper_factory.reset(new DataWrapperTFactoryT<T>(fs)); }

    void clearFileStorage(void) { _data_wrapper_factory.reset(new DataWrapperTFactoryT<void>(nullptr)); }

//----------------------------------------------------------------------------
// print & text support
//----------------------------------------------------------------------------
// Arduino Print.h compatible
  #if !defined (ARDUINO)
    size_t print(const char str[])      { return write(str); }
    size_t print(char c)                { return write(c); }
    size_t print(int  n, int base = 10) { return print((long)n, base); }
    size_t print(long n, int base = 10)
    {
      if (base == 0) { return write(n); }
      if (base == 10) {
        if (n < 0) {
          size_t t = print('-');
          return printNumber(-n, 10) + t;
        }
        return printNumber(n, 10);
      }
      return printNumber(n, base);
    }

    size_t print(unsigned char n, int base = 10) { return print((unsigned long)n, base); }
    size_t print(unsigned int  n, int base = 10) { return print((unsigned long)n, base); }
    size_t print(unsigned long n, int base = 10) { return (base) ? printNumber(n, base) : write(n); }
    size_t print(double        n, int digits= 2) { return printFloat(n, digits); }

    size_t println(void) { return print("\r\n"); }
    size_t println(const char c[])                 { size_t t = print(c); return println() + t; }
    size_t println(char c        )                 { size_t t = print(c); return println() + t; }
    size_t println(int  n, int base = 10)          { size_t t = print(n,base); return println() + t; }
    size_t println(long n, int base = 10)          { size_t t = print(n,base); return println() + t; }
    size_t println(unsigned char n, int base = 10) { size_t t = print(n,base); return println() + t; }
    size_t println(unsigned int  n, int base = 10) { size_t t = print(n,base); return println() + t; }
    size_t println(unsigned long n, int base = 10) { size_t t = print(n,base); return println() + t; }
    size_t println(double        n, int digits= 2) { size_t t = print(n, digits); return println() + t; }

  //size_t print(const String &s) { return write(s.c_str(), s.length()); }
  //size_t print(const __FlashStringHelper *s)   { return print(reinterpret_cast<const char *>(s)); }
  //size_t println(const String &s)              { size_t t = print(s); return println() + t; }
  //size_t println(const __FlashStringHelper *s) { size_t t = print(s); return println() + t; }

    size_t write(const char* str)                 { return (!str) ? 0 : write((const uint8_t*)str, strlen(str)); }
    size_t write(const char *buf, size_t size)    { return write((const uint8_t *) buf, size); }
  #else
    using Print::write;
  #endif

  #if defined (LGFX_PRINTF_ENABLED)
   #ifdef __GNUC__
    size_t printf(const char* format, ...)  __attribute__((format(printf, 2, 3)));
   #else
    size_t printf(const char* format, ...);
   #endif
  #endif

    size_t write(const uint8_t *buf, size_t size) { size_t n = 0; this->startWrite(); while (size--) { n += write(*buf++); } this->endWrite(); return n; }
    size_t write(uint8_t utf8);
    size_t vprintf(const char *format, va_list arg);


#ifdef ARDUINO
    void qrcode(const String &string, int32_t x = -1, int32_t y = -1, int32_t width = -1, uint8_t version = 1) {
      qrcode(string.c_str(), x, y, width, version);
    }
#endif
    void qrcode(const char *string, int32_t x = -1, int32_t y = -1, int32_t width = -1, uint8_t version = 1);

  #define LGFX_FUNCTION_GENERATOR(drawImg, draw_img) \
   protected: \
    bool draw_img(DataWrapper* data, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, float scale_x, float scale_y, datum_t datum); \
   public: \
    bool drawImg(const uint8_t *data, uint32_t len, int32_t x=0, int32_t y=0, int32_t maxWidth=0, int32_t maxHeight=0, int32_t offX=0, int32_t offY=0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      PointerWrapper data_wrapper; \
      data_wrapper.set(data, len); \
      return this->draw_img(&data_wrapper, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \
    inline bool drawImg(DataWrapper *data, int32_t x=0, int32_t y=0, int32_t maxWidth=0, int32_t maxHeight=0, int32_t offX=0, int32_t offY=0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      return this->draw_img(data, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
    } \
    bool drawImg##File(DataWrapper* file, const char *path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      bool res = false; \
      this->prepareTmpTransaction(file); \
      file->preRead(); \
      if (file->open(path)) \
      { \
        res = this->draw_img(file, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
        file->close(); \
      } \
      file->postRead(); \
      return res; \
    } \
    inline bool drawImg##File(const char *path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      auto data = _create_data_wrapper(); \
      bool res = drawImg##File(data, path, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
      delete data; \
      return res; \
    } \
    template <typename T> \
    inline bool drawImg##File(T &fs, const char *path, int32_t x = 0, int32_t y = 0, int32_t maxWidth = 0, int32_t maxHeight = 0, int32_t offX = 0, int32_t offY = 0, float scale_x = 1.0f, float scale_y = 0.0f, datum_t datum = datum_t::top_left) \
    { \
      DataWrapperT<T> file ( &fs ); \
      bool res = this->drawImg##File(&file, path, x, y, maxWidth, maxHeight, offX, offY, scale_x, scale_y, datum); \
      file.close(); \
      return res; \
    }

    LGFX_FUNCTION_GENERATOR(drawBmp, draw_bmp)
    LGFX_FUNCTION_GENERATOR(drawJpg, draw_jpg)
    LGFX_FUNCTION_GENERATOR(drawPng, draw_png)
    LGFX_FUNCTION_GENERATOR(drawQoi, draw_qoi)

  #undef LGFX_FUNCTION_GENERATOR

    [[deprecated("use float scale")]] bool drawJpg(const uint8_t *jpg_data, uint32_t jpg_len, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpg(jpg_data, jpg_len, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }
    [[deprecated("use float scale")]] inline bool drawJpg(DataWrapper *data, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpg(data, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }
    [[deprecated("use float scale")]]
    inline bool drawJpgFile(const char *path, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight, int32_t offX, int32_t offY, jpeg_div::jpeg_div_t scale)
    {
      return drawJpgFile(path, x, y, maxWidth, maxHeight, offX, offY, 1.0f / (1 << scale));
    }

    void* createPng( size_t* datalen, int32_t x = 0, int32_t y = 0, int32_t width = 0, int32_t height = 0);

    void releasePngMemory(void);

    template<typename T>
    [[deprecated("use pushImage")]] void pushRect( int32_t x, int32_t y, int32_t w, int32_t h, const T* data) { pushImage(x, y, w, h, data); }

    template<typename T>
    [[deprecated("use pushBlock")]] void pushColor(const T& color, uint32_t length) { if (0 != length) { setColor(color); startWrite(); _panel->writeBlock(getRawColor(), length); endWrite(); } }
    template<typename T>
    [[deprecated("use pushBlock")]] void pushColor(const T& color                     ) {                     setColor(color); startWrite(); _panel->writeBlock(getRawColor(), 1);      endWrite(); }

    template<typename T>
    [[deprecated("use pushPixels")]] void pushColors(T*              data, int32_t len           ) { startWrite(); writePixels(data, len            ); endWrite(); }
    [[deprecated("use pushPixels")]] void pushColors(const void*     data, int32_t len           ) { startWrite(); writePixels(data, len, _swapBytes); endWrite(); }
    [[deprecated("use pushPixels")]] void pushColors(const uint16_t* data, int32_t len           ) { startWrite(); writePixels(data, len, _swapBytes); endWrite(); }
    [[deprecated("use pushPixels")]] void pushColors(const uint8_t*  data, int32_t len           ) { startWrite(); writePixels((const rgb332_t*)data, len); endWrite(); }
    [[deprecated("use pushPixels")]] void pushColors(const void*     data, int32_t len, bool swap) { startWrite(); writePixels(data, len, swap); endWrite(); }
    [[deprecated("use pushPixels")]] void pushColors(const uint16_t* data, int32_t len, bool swap) { startWrite(); writePixels(data, len, swap); endWrite(); }

    void prepareTmpTransaction(DataWrapper* data);
//----------------------------------------------------------------------------

  protected:

    virtual RGBColor* getPalette_impl(void) const { return nullptr; }

    IPanel* _panel = nullptr;

    int32_t _sx = 0, _sy = 0, _sw = 0, _sh = 0; // for scroll zone
    int32_t _clip_l = 0, _clip_r = -1, _clip_t = 0, _clip_b = -1; // clip rect

    uint32_t _base_rgb888 = 0;  // gap fill colour for clear and scroll zone
    raw_color_t _color = 0xFFFFFFU;

    color_conv_t _write_conv;
    color_conv_t _read_conv;

    uint16_t _palette_count = 0;

    float _xpivot = 0.0f;   // x pivot point coordinate
    float _ypivot = 0.0f;   // x pivot point coordinate

    bool _swapBytes = false;

    enum utf8_decode_state_t : uint8_t
    { utf8_state0 = 0
    , utf8_state1 = 1
    , utf8_state2 = 2
    };
    utf8_decode_state_t _decoderState = utf8_state0;   // UTF8 decoder state
    uint16_t _unicode_buffer = 0;   // Unicode code-point buffer

    int32_t _cursor_x = 0;  // print text cursor
    int32_t _cursor_y = 0;
    int32_t _filled_x = 0;  // print filled position

    TextStyle _text_style;
    FontMetrics _font_metrics = { 6, 6, 0, 8, 8, 0, 7 }; // Font0 default metric
    const IFont* _font = &fonts::Font0;

    std::shared_ptr<RunTimeFont> _runtime_font;  // run-time generated font
    std::shared_ptr<DataWrapper> _font_file;  // run-time font file
    PointerWrapper _font_data;

    std::shared_ptr<DataWrapperFactory> _data_wrapper_factory;
    DataWrapper* _create_data_wrapper(void) { if (nullptr == _data_wrapper_factory.get()) { clearFileStorage(); } return _data_wrapper_factory->create(); }

    bool _textwrap_x = true;
    bool _textwrap_y = false;
    bool _textscroll = false;

    LGFX_INLINE static bool _adjust_abs(int32_t& x, int32_t& w) { if (w < 0) { x += w; w = -w; } return !w; }
    static bool _adjust_width(int32_t& x, int32_t& dx, int32_t& dw, int32_t left, int32_t width)
    {
      if (x < left) { dx = -x; dw += x; x = left; }
      if (dw > left + width - x) dw = left + width  - x;
      return (dw <= 0);
    }

    bool _clipping(int32_t& x, int32_t& y, int32_t& w, int32_t& h)
    {
      auto cl = _clip_l;
      if (x < cl) { w += x - cl; x = cl; }
      auto cr = _clip_r + 1 - x;
      if (w > cr) w = cr;
      if (w < 1) return false;

      auto ct = _clip_t;
      if (y < ct) { h += y - ct; y = ct; }
      auto cb = _clip_b + 1 - y;
      if (h > cb) h = cb;
      if (h < 1) return false;

      return true;
    }

//----------------------------------------------------------------------------

    template<typename T>
    pixelcopy_t create_pc_fast(const T *data)
    {
      auto dst_depth = _write_conv.depth;
      pixelcopy_t pc(data, dst_depth, get_depth<T>::value, hasPalette());
      if (hasPalette() || pc.dst_bits < 8)
      {
        pc.fp_copy = pixelcopy_t::copy_bit_fast;
      }
      else
      if (pc.dst_bits > 16)
      {
        if (     dst_depth == rgb888_3Byte) { pc.fp_copy = pixelcopy_t::copy_rgb_fast<bgr888_t, T>; }
        else if (dst_depth == rgb666_3Byte) { pc.fp_copy = pixelcopy_t::copy_rgb_fast<bgr666_t, T>; }
        else                                { pc.fp_copy = pixelcopy_t::copy_rgb_fast<argb8888_t, T>; }
      }
      else
      {
        if (     dst_depth == rgb565_2Byte) { pc.fp_copy = pixelcopy_t::copy_rgb_fast<swap565_t, T>; }
        else if (dst_depth == rgb332_1Byte) { pc.fp_copy = pixelcopy_t::copy_rgb_fast<rgb332_t, T>; }
        else                                { pc.fp_copy = pixelcopy_t::copy_rgb_fast<grayscale_t, T>; }
      }
      return pc;
    }
    LGFX_INLINE pixelcopy_t create_pc_fast(const uint8_t*  data) { return create_pc_fast(reinterpret_cast<const rgb332_t*>(data)); }
    LGFX_INLINE pixelcopy_t create_pc_fast(const uint16_t* data) { return create_pc_fast(data, _swapBytes); }
    LGFX_INLINE pixelcopy_t create_pc_fast(const void*     data) { return create_pc_fast(data, _swapBytes); }
    LGFX_INLINE pixelcopy_t create_pc_fast(const uint16_t* data, bool swap)
    {
      return swap && !hasPalette() && _write_conv.bits >= 8
           ? create_pc_fast(reinterpret_cast<const rgb565_t* >(data))
           : create_pc_fast(reinterpret_cast<const swap565_t*>(data));
    }
    LGFX_INLINE pixelcopy_t create_pc_fast(const void *data, bool swap)
    {
      return swap && !hasPalette() && _write_conv.bits >= 8
           ? create_pc_fast(reinterpret_cast<const rgb888_t*>(data))
           : create_pc_fast(reinterpret_cast<const bgr888_t*>(data));
    }

    template<typename T>
    pixelcopy_t create_pc_fast(const void *data, const T *palette, lgfx::color_depth_t src_depth)
    {
      auto dst_depth = _write_conv.depth;
/*
      pixelcopy_t pc(data, dst_depth, src_depth, hasPalette(), palette);
/*/
      pixelcopy_t pc;
      pc.src_data  = data   ;
      pc.palette   = palette;
      //pc.src_bits  = src_depth > 8 ? (src_depth + 7) & ~7 : src_depth;
      //pc.dst_bits  = dst_depth > 8 ? (dst_depth + 7) & ~7 : dst_depth;
      pc.src_depth = src_depth;
      pc.dst_depth = dst_depth;
      pc.src_mask  = (1 << pc.src_bits) - 1 ;
      pc.dst_mask  = (1 << pc.dst_bits) - 1 ;
      pc.no_convert= src_depth == dst_depth;
//*/
      if (hasPalette() || pc.dst_bits < 8)
      {
        if (palette && (pc.src_bits == 8) && (pc.dst_bits == 8))
        {
          pc.fp_copy = pixelcopy_t::copy_rgb_fast<rgb332_t, rgb332_t>;
        }
        else
        {
          pc.fp_copy = pixelcopy_t::copy_bit_fast;
        }
      }
      else
      {
        if (pc.dst_bits > 16)
        {
          if (     dst_depth == rgb888_3Byte) { pc.fp_copy = pixelcopy_t::copy_palette_fast<bgr888_t, T>; }
          else if (dst_depth == rgb666_3Byte) { pc.fp_copy = pixelcopy_t::copy_palette_fast<bgr666_t, T>; }
          else                                { pc.fp_copy = pixelcopy_t::copy_palette_fast<argb8888_t, T>; }
        }
        else
        {
          if (     dst_depth == rgb565_2Byte) { pc.fp_copy = pixelcopy_t::copy_palette_fast<swap565_t, T>; }
          else if (dst_depth == rgb332_1Byte) { pc.fp_copy = pixelcopy_t::copy_palette_fast<rgb332_t, T>; }
          else                                { pc.fp_copy = pixelcopy_t::copy_palette_fast<grayscale_t, T>; }
        }
      }
      return pc;
    }


    template<typename T>
    pixelcopy_t create_pc(const T *data)
    {
      pixelcopy_t pc(data, _write_conv.depth, get_depth<T>::value, hasPalette());
      if (std::is_same<rgb565_t, T>::value || std::is_same<rgb888_t, T>::value || std::is_same<argb8888_t, T>::value || std::is_same<grayscale_t, T>::value)
      {
        pc.no_convert = false;
        pc.fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<T>(_write_conv.depth);
      }
      return pc;
    }

    LGFX_INLINE pixelcopy_t create_pc(const uint8_t*  data) { return create_pc(reinterpret_cast<const rgb332_t*>(data)); }
    LGFX_INLINE pixelcopy_t create_pc(const uint16_t* data) { return create_pc(data, _swapBytes); }
    LGFX_INLINE pixelcopy_t create_pc(const void*     data) { return create_pc(data, _swapBytes); }
    LGFX_INLINE pixelcopy_t create_pc(const uint16_t* data, bool swap)
    {
      return swap && !hasPalette() && _write_conv.bits >= 8
           ? create_pc(reinterpret_cast<const rgb565_t* >(data))
           : create_pc(reinterpret_cast<const swap565_t*>(data));
    }
    LGFX_INLINE pixelcopy_t create_pc(const void* data, bool swap)
    {
      return swap && !hasPalette() && _write_conv.bits >= 8
           ? create_pc(reinterpret_cast<const rgb888_t*>(data))
           : create_pc(reinterpret_cast<const bgr888_t*>(data));
    }

    template<typename T>
    pixelcopy_t create_pc_rawtr(const T *data, uint32_t raw_transparent)
    {
      if (std::is_same<rgb565_t, T>::value) { raw_transparent = getSwap16(raw_transparent); }
      if (std::is_same<rgb888_t, T>::value) { raw_transparent = getSwap24(raw_transparent); }
      pixelcopy_t pc(data, _write_conv.depth, get_depth<T>::value, hasPalette(), nullptr, raw_transparent);
      if (std::is_same<rgb565_t, T>::value || std::is_same<rgb888_t, T>::value || std::is_same<argb8888_t, T>::value || std::is_same<grayscale_t, T>::value)
      {
        pc.no_convert = false;
        pc.fp_copy = pixelcopy_t::get_fp_copy_rgb_affine<T>(_write_conv.depth);
        pc.fp_skip = pixelcopy_t::skip_rgb_affine<T>;
      }
      return pc;
    }

    template<typename T1, typename T2>
    pixelcopy_t create_pc_tr(const T1 *data, const T2& transparent)
    {
      return create_pc_rawtr( data, color_convert<T1, T2>((uint32_t)transparent));
    }

    template<typename T> pixelcopy_t create_pc_tr(const uint8_t*  data, const T& transparent) { return create_pc_tr(reinterpret_cast<const rgb332_t*>(data), transparent); }
    template<typename T> pixelcopy_t create_pc_tr(const uint16_t* data, const T& transparent) { return create_pc_tr(data, transparent, _swapBytes); }
    template<typename T> pixelcopy_t create_pc_tr(const void*     data, const T& transparent) { return create_pc_tr(data, transparent, _swapBytes); }
    pixelcopy_t create_pc_tr(const uint16_t* data, const uint16_t& transparent, bool swap)
    {
      return swap && _write_conv.bits >= 8 && !hasPalette()
           ? create_pc_rawtr(reinterpret_cast<const rgb565_t* >(data), getSwap16(transparent))
           : create_pc_rawtr(reinterpret_cast<const swap565_t*>(data),           transparent);
    }
    pixelcopy_t create_pc_tr(const void *data, const uint32_t& transparent, bool swap)
    {
      return swap && _write_conv.bits >= 8 && !hasPalette()
           ? create_pc_rawtr(reinterpret_cast<const rgb888_t*>(data), getSwap24(transparent))
           : create_pc_rawtr(reinterpret_cast<const bgr888_t*>(data),           transparent);
    }

    template<typename T> pixelcopy_t create_pc_tr(const uint16_t* data, const T& transparent, bool swap)
    {
      return swap && _write_conv.bits >= 8 && !hasPalette()
           ? create_pc_tr(reinterpret_cast<const rgb565_t* >(data), transparent)
           : create_pc_tr(reinterpret_cast<const swap565_t*>(data), transparent);
    }
    template<typename T> pixelcopy_t create_pc_tr(const void *data, const T& transparent, bool swap)
    {
      return swap && _write_conv.bits >= 8 && !hasPalette()
           ? create_pc_tr(reinterpret_cast<const rgb888_t*>(data), transparent)
           : create_pc_tr(reinterpret_cast<const bgr888_t*>(data), transparent);
    }

    pixelcopy_t create_pc_palette(const void *data, const bgr888_t *palette, lgfx::color_depth_t depth, uint32_t transparent = pixelcopy_t::NON_TRANSP)
    {
      return pixelcopy_t (data, _write_conv.depth, depth, hasPalette(), palette, transparent);
    }

    template<typename T>
    pixelcopy_t create_pc_palette(const void *data, const T *palette, lgfx::color_depth_t depth, uint32_t transparent = pixelcopy_t::NON_TRANSP)
    {
      pixelcopy_t pc(data, getColorDepth(), depth, hasPalette(), palette, transparent);
      if (!hasPalette() && palette && _write_conv.bits >= 8)
      {
        pc.fp_copy = pixelcopy_t::get_fp_copy_palette_affine<T>(getColorDepth());
      }
      return pc;
    }

    LGFX_INLINE pixelcopy_t create_pc_antialias(const uint8_t *data, uint32_t raw_transparent = pixelcopy_t::NON_TRANSP)
    {
      return create_pc_antialias(reinterpret_cast<const rgb332_t*>(data), raw_transparent);
    }
    LGFX_INLINE pixelcopy_t create_pc_antialias(const uint16_t *data, uint32_t raw_transparent = pixelcopy_t::NON_TRANSP)
    {
      return _swapBytes
           ? create_pc_antialias(reinterpret_cast<const rgb565_t* >(data), raw_transparent)
           : create_pc_antialias(reinterpret_cast<const swap565_t*>(data), raw_transparent);
    }
    LGFX_INLINE pixelcopy_t create_pc_antialias(const void *data, uint32_t raw_transparent = pixelcopy_t::NON_TRANSP)
    {
      return _swapBytes
           ? create_pc_antialias(reinterpret_cast<const rgb888_t*>(data), raw_transparent)
           : create_pc_antialias(reinterpret_cast<const bgr888_t*>(data), raw_transparent);
    }

    template<typename T>
    pixelcopy_t create_pc_antialias(const T* data, uint32_t raw_transparent = pixelcopy_t::NON_TRANSP)
    {
      pixelcopy_t pc(data, argb8888_t::depth, get_depth<T>::value, false, nullptr, raw_transparent);
      pc.src_data = data;
      pc.fp_copy = pixelcopy_t::copy_rgb_antialias<T>;
      return pc;
    }

    template<typename T1, typename T2>
    pixelcopy_t create_pc_tr_antialias(const T1* data, const T2& transparent)
    {
      return create_pc_antialias( data, color_convert<T1, T2>(transparent));
    }

    template<typename T>
    static pixelcopy_t create_pc_antialias(const void* data, const T* palette, lgfx::color_depth_t depth, uint32_t transparent = pixelcopy_t::NON_TRANSP)
    {
      pixelcopy_t pc(data, argb8888_t::depth, depth, false, palette, transparent);
      if (palette)
      {
        pc.fp_copy = pixelcopy_t::copy_palette_antialias<T>;
      }
      else if ((depth & color_depth_t::bit_mask) > 16)
      {
        if (depth == rgb888_3Byte) {
          pc.fp_copy = pixelcopy_t::copy_rgb_antialias<bgr888_t>;
        } else {
          pc.fp_copy = pixelcopy_t::copy_rgb_antialias<bgr666_t>;
        }
      }
      else
      {
        if (depth == rgb565_2Byte) {
          pc.fp_copy = pixelcopy_t::copy_rgb_antialias<swap565_t>;
        } else if (depth == rgb332_1Byte) {
          pc.fp_copy = pixelcopy_t::copy_rgb_antialias<rgb332_t>;
        } else {
          pc.fp_copy = pixelcopy_t::copy_rgb_antialias<grayscale_t>;
        }
      }
      return pc;
    }

    pixelcopy_t create_pc_gray(const uint8_t *image, lgfx::color_depth_t depth, uint32_t fore_rgb888, uint32_t back_rgb888);

//----------------------------------------------------------------------------

    static void make_rotation_matrix(float* result, float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y);

    void read_rect(int32_t x, int32_t y, int32_t w, int32_t h, void* dst, pixelcopy_t* param);

//----------------------------------------------------------------------------

    bool clampArea(int32_t *xlo, int32_t *ylo, int32_t *xhi, int32_t *yhi);

    rgb888_t map_gradient( float value, float start, float end, const rgb888_t *colors, uint32_t colors_count );
    rgb888_t map_gradient( float value, float start, float end, const colors_t gradient );

    void draw_gradient_line( int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t colorstart, uint32_t colorend );
    void draw_gradient_line( int32_t x0, int32_t y0, int32_t x1, int32_t y1, const colors_t gradient );

    void draw_wedgeline         (float x0, float y0, float x1, float y1, float r0, float r1, const uint32_t fg_color);
    void draw_gradient_wedgeline(float x0, float y0, float x1, float y1, float r0, float r1, const colors_t gradient );

    void fill_rect_radial_gradient(int32_t x, int32_t y, uint32_t w, uint32_t h, const colors_t gradient);
    void fill_rect_radial_gradient(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint32_t colorstart, const uint32_t colorend );
    void fill_rect_linear_gradient(int32_t x, int32_t y, uint32_t w, uint32_t h, const colors_t gradient, fill_style_t style=VLINEAR );
    void fill_rect_gradient(int32_t x, int32_t y, uint32_t w, uint32_t h, const colors_t gradient, fill_style_t style=RADIAL );
    void fill_rect_gradient(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint32_t colorstart, const uint32_t colorend, fill_style_t style=RADIAL );

//----------------------------------------------------------------------------

    void fill_arc_helper(int32_t cx, int32_t cy, int32_t oradius_x, int32_t iradius_x, int32_t oradius_y, int32_t iradius_y, float start, float end);
    void draw_bezier_helper(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2);
    void draw_bitmap(int32_t x, int32_t y, const uint8_t *bitmap, int32_t w, int32_t h, uint32_t fg_rawcolor, uint32_t bg_rawcolor = ~0u);
    void draw_xbitmap(int32_t x, int32_t y, const uint8_t *bitmap, int32_t w, int32_t h, uint32_t fg_rawcolor, uint32_t bg_rawcolor = ~0u);
    void push_grayimage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *image, color_depth_t depth, uint32_t fg_rgb888, uint32_t bg_rgb888);
    void push_grayimage_rotate_zoom(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, const uint8_t* image, color_depth_t depth, uint32_t fg_rgb888, uint32_t bg_rgb888);
    void push_grayimage_affine(const float* matrix, int32_t w, int32_t h, const uint8_t *image, color_depth_t depth, uint32_t fg_rgb888, uint32_t bg_rgb888);
    void push_image_rotate_zoom(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, pixelcopy_t* pc);
    void push_image_rotate_zoom_aa(float dst_x, float dst_y, float src_x, float src_y, float angle, float zoom_x, float zoom_y, int32_t w, int32_t h, pixelcopy_t* pc);
    void push_image_affine(const float* matrix, int32_t w, int32_t h, pixelcopy_t *pc);
    void push_image_affine(const float* matrix, pixelcopy_t *pc);
    void push_image_affine_aa(const float* matrix, int32_t w, int32_t h, pixelcopy_t *pc);
    void push_image_affine_aa(const float* matrix, pixelcopy_t *pre_pc, pixelcopy_t *post_pc);

    uint16_t decodeUTF8(uint8_t c);

    size_t printNumber(unsigned long n, uint8_t base);
    size_t printFloat(double number, uint8_t digits);
    size_t draw_string(const char *string, int32_t x, int32_t y, textdatum_t datum, const IFont* font = nullptr);
    int32_t text_width(const char *string, const IFont* font, FontMetrics* metrics);
    bool load_font(lgfx::DataWrapper* data);
    bool load_font_with_path(const char *path);

    static void tmpBeginTransaction(LGFXBase* lgfx)
    {
      if (lgfx->getStartCount()) { lgfx->beginTransaction(); }
    }

    static void tmpEndTransaction(LGFXBase* lgfx)
    {
      if (lgfx->getStartCount())
      {
        lgfx->endTransaction();
      }
    }

  };

//----------------------------------------------------------------------------

  /// @brief LovyanGFX class.
  /// that depend on the include order of the environment, such as file system, are inherited from LGFX_FILESYSTEM_Support.
  class LovyanGFX : public
  #ifdef LGFX_FILESYSTEM_SUPPORT_HPP_
      LGFX_FILESYSTEM_Support<
  #endif
       LGFXBase
  #ifdef LGFX_FILESYSTEM_SUPPORT_HPP_
      >
  #endif
  {
  };

//----------------------------------------------------------------------------

  struct Panel_Device;

  class LGFX_Device : public LovyanGFX
  {
  public:
    LGFX_Device(void);

    bool init(void)               { return init_impl(true , true); };
    bool begin(void)              { return init_impl(true , true); };
    bool init_without_reset(void) { return init_impl(false, false); };
    board_t getBoard(void) const { return _board; }
    void initBus(void);
    void releaseBus(void);
    void setPanel(Panel_Device* panel);

    void setEpdMode(epd_mode_t epd_mode) { _panel->setEpdMode(epd_mode); }
    epd_mode_t getEpdMode(void) const { return _panel->getEpdMode(); }
    inline void invertDisplay(bool i) { _panel->setInvert(i); }
    inline bool getInvert(void) const { return _panel->getInvert(); }

    inline void sleep(void) { _panel->setBrightness(0); _panel->setSleep(true); }
    inline void wakeup(void) { _panel->setSleep(false); _panel->setBrightness(_brightness); }
    inline void powerSave(bool flg) { _panel->setPowerSave(flg); }
    inline void powerSaveOn(void) { _panel->setPowerSave(true); }
    inline void powerSaveOff(void) { _panel->setPowerSave(false); }

    inline Panel_Device* panel(void) const { return reinterpret_cast<Panel_Device*>(_panel); }
    inline Panel_Device* getPanel(void) const { return reinterpret_cast<Panel_Device*>(_panel); }
    inline void panel(Panel_Device* panel) { setPanel(panel); }

    inline void writeCommand(  uint8_t  cmd) { _panel->writeCommand(             cmd , 1); } // AdafruitGFX compatible
    inline void writecommand(  uint8_t  cmd) { _panel->writeCommand(             cmd , 1); } // TFT_eSPI compatible
    inline void writeCommand16(uint16_t cmd) { _panel->writeCommand((cmd<<8)+(cmd>>8), 2); }
    inline void spiWrite(   uint8_t  data) { _panel->writeData(                  data , 1); } // AdafruitGFX compatible
    inline void writedata(  uint8_t  data) { _panel->writeData(                  data , 1); } // TFT_eSPI compatible
    inline void writeData(  uint8_t  data) { _panel->writeData(                  data , 1); }
    inline void writeData16(uint16_t data) { _panel->writeData((data<<8)+(data>>8), 2); }
    inline void writeData32(uint32_t data) { data = (data << 16) + (data >> 16); _panel->writeData(((data >> 8) & 0xFF00FF) + ((data & 0xFF00FF) << 8), 4); }

    inline uint8_t  readData8( uint8_t index=0) { return                   _panel->readData(index, 1) ; }
    inline uint16_t readData16(uint8_t index=0) { auto r = _panel->readData(index, 2); return (r<<8)+(r>>8); }
    inline uint32_t readData32(uint8_t index=0) { auto r = _panel->readData(index, 4); r=(r<<16)+(r>>16); return ((r>>8)&0xFF00FF)+((r&0xFF00FF)<<8); }

    inline ILight* light(void) const { return _panel ? panel()->light() : nullptr; }
    inline void setBrightness(uint8_t brightness) { _brightness = brightness; if (_panel) { _panel->setBrightness(brightness); } }
    inline uint8_t getBrightness(void) const { return _brightness; }

    inline ITouch* touch(void) const { return _panel ? panel()->touch() : nullptr; }
    inline uint_fast8_t getTouchRaw(touch_point_t *tp, uint_fast8_t count = 1) { return panel()->getTouchRaw(tp, count); }
    inline uint_fast8_t getTouch(touch_point_t *tp, uint_fast8_t count = 1) { return panel()->getTouch(tp, count); }
    inline void convertRawXY(touch_point_t *tp, uint_fast8_t count = 1) { panel()->convertRawXY(tp, count); }

    template <typename T>
    uint_fast8_t getTouchRaw(T *x, T *y, uint_fast8_t index = 0)
    {
      touch_point_t tp[index + 1];
      auto count = getTouchRaw(tp, index + 1);
      if (index >= count) return 0;
      if (x) *x = tp[index].x;
      if (y) *y = tp[index].y;
      return count;
    }

    template <typename T>
    uint_fast8_t getTouch(T *x, T *y, uint_fast8_t index = 0)
    {
      if (index >= 8) return 0;
      touch_point_t tp[8];
      auto count = getTouch(tp, index + 1);
      if (index >= count) return 0;
      if (x) *x = tp[index].x;
      if (y) *y = tp[index].y;
      return count;
    }

    template <typename T>
    void convertRawXY(T *x, T *y)
    {
      touch_point_t tp;
      tp.x = *x;
      tp.y = *y;
      panel()->convertRawXY(&tp, 1);
      *x = tp.x;
      *y = tp.y;
    }

    /// This requires a uint16_t array with 8 elements. ( or nullptr )
    template <typename T>
    void calibrateTouch(uint16_t *parameters, const T& color_fg, const T& color_bg, uint8_t size = 10)
    { // 第1引数 にuint16_t[8]のポインタを渡すことで、setTouchCalibrateで使用できるキャリブレーション値を得ることが出来る。;
      // この値をフラッシュ等に記録しておき、次回起動時にsetTouchCalibrateを使うことで、手作業によるキャリブレーションを省略できる。;
      calibrate_touch(parameters, _write_conv.convert(color_fg), _write_conv.convert(color_bg), size);
    }

    /// This requires a uint16_t array with 8 elements.
    /// calibrateTouchで得たキャリブレーション値を用いて設定を再現する。;
    void setTouchCalibrate(uint16_t *parameters)
    {
      panel()->setCalibrate(parameters);
    }

  protected:
    board_t _board = board_t::board_unknown;
    uint8_t _brightness = 127;

    virtual bool init_impl(bool use_reset, bool use_clear);

    void draw_calibrate_point(int32_t x, int32_t y, int32_t r, uint32_t fg_rawcolor, uint32_t bg_rawcolor);

    void calibrate_touch(uint16_t *parameters, uint32_t fg_rawcolor, uint32_t bg_rawcolor, uint8_t size);

//----------------------------------------------------------------------------

  };

//----------------------------------------------------------------------------
#undef LGFX_INLINE
#undef LGFX_INLINE_T

 }
}

using LovyanGFX = lgfx::LovyanGFX;
using LGFX_Device = lgfx::LGFX_Device;
