#pragma once

#include "protocol.h"

class protocol_ansonic : public c_rf_protocol {
public:
    uint16_t timing_high = 1111;
    uint16_t timing_low = 555;
};
