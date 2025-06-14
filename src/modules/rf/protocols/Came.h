#pragma once

#include "protocol.h"

class protocol_came : public c_rf_protocol {
public:
    uint16_t timing_high = 640;
    uint16_t timing_low = 320;
};
