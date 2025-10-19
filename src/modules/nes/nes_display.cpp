extern "C" {
#include <nes/nes.h>
}

#include "core/display.h"
#include "hw_config.h"

int16_t frame_scaling;
static int16_t frame_x, frame_y, frame_x_offset, frame_y_offset, frame_width, frame_height, frame_line_pixels;
extern uint16_t myPalette[];

extern void display_begin() {}

extern "C" {

void display_init() {
    frame_scaling = 0;

    frame_x = 0;
    frame_y = 0;
    frame_x_offset = 0;
    frame_y_offset = 90;
    frame_width = tftWidth;
    frame_height = tftHeight;
    frame_line_pixels = frame_width;
}

void display_write_frame(const uint8_t *data[]) {
    static uint16_t lineBuf[NES_SCREEN_WIDTH]; // max TFT line width

    tft.startWrite();
    tft.setWindow(frame_x, frame_y, frame_width - 1, frame_height - 1);

    if (frame_scaling == 0) {
        for (int32_t y = 0; y < frame_height; y++) {
            int srcY = (y * NES_SCREEN_HEIGHT) / frame_height; // scale Y: 135 -> 240

            for (int32_t x = 0; x < frame_width; x++) {
                int srcX = (x * NES_SCREEN_WIDTH) / frame_width; // scale X: 240 -> 256
                lineBuf[x] = myPalette[data[srcY][srcX]];
            }
            tft.pushPixels(lineBuf, frame_width);
        }
    } else {
        for (int32_t i = frame_y_offset; i < frame_height + frame_y_offset; i++) {
            for (int32_t j = 0; j < frame_width; j++) {
                lineBuf[j] = myPalette[data[i][j]]; // lookup into pre-converted palette }
            }
            tft.pushPixels(lineBuf, frame_width);
        }
    }
    tft.endWrite();
}

void display_clear() { tft.fillScreen(TFT_BLACK); }
}
