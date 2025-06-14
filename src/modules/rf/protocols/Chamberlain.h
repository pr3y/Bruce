#pragma once

#include "protocol.h"

class protocol_chamberlain : public c_rf_protocol {
public:
    uint16_t timing_high = 3000;
    uint16_t timing_low = 1000;
};
