#pragma once

#include "protocol.h"

class protocol_linear : public c_rf_protocol {
public:
    protocol_linear() {
        transposition_table['0'] = {500, -1500};
        transposition_table['1'] = {1500, -500};
        pilot_period = {};
        stop_bit = {1, -21500};
    }
};
