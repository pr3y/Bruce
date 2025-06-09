#pragma once

#include "protocol.h"

class protocol_nice_flo : public c_rf_protocol {
public:
    uint16_t timing_high = 1400;
    uint16_t timing_low = 700;
};
