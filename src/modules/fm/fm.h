#ifndef __FM_H__
#define __FM_H__
#ifndef LITE_VERSION
#include "core/display.h"
#include "core/mykeyboard.h"
#include <Adafruit_Si4713.h>
#include <globals.h>

void fm_live_run(bool reserved = true);

void fm_ta_run();

bool fm_begin();

bool fm_setup(bool traffic_alert = false, bool silent = false);

void fm_stop();

void fm_spectrum();

#endif
#endif
