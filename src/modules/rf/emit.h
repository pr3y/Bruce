#ifndef RF_REPLAY_H
#define RF_REPLAY_H

#include "core/display.h"
#include "record.h"
#include "structs.h"
#include <vector>
void rf_raw_emit_draw(void *parameter);
void rf_raw_emit(RawRecording &recorded, bool &returnToMenu);
#endif
