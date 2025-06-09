#pragma once

#include "protocol.h"

class protocol_liftmaster : public c_rf_protocol {
public:
    uint16_t timing_high = 800;
    uint16_t timing_low = 400;
};
