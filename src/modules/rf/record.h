#ifndef RF_RECORD_H
#define RF_RECORD_H

#include "core/display.h"
#include "emit.h"
#include "save.h"
#include "structs.h"

void rf_raw_record();
int rf_raw_record_options();
void rf_raw_record_create(RawRecording &recorded, bool &returnToMenu);

#endif
