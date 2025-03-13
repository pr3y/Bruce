#ifndef RF_REPLAY_H
#define RF_REPLAY_H

#include "core/display.h"
#include "rf.h"
#include <driver/rmt.h>
#include <vector>

void rf_raw_emit(struct RawRecording recorded);

#endif