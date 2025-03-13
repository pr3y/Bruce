#ifndef RF_RECORD_H
#define RF_RECORD_H

#include "record.h"
#include "emit.h"
#include "rf.h"
#include "structs.h"
#include "core/display.h"

void sine_wave_animation();
void rf_raw_record_draw(RawRecordingStatus status);
void rf_raw_record();
void rf_raw_record_options(RawRecording recorded);

#endif