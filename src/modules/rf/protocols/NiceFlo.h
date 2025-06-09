#pragma once

#include "protocol.h"

class protocol_nice_flo : public c_rf_protocol {
public:
    protocol_nice_flo() {
        transposition_table['0'] = {-700, 1400};
        transposition_table['1'] = {-1400, 700};
        pilot_period = {-25200, 700};
        stop_bit = {};
    }
};
