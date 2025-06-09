#pragma once

#include "protocol.h"

class protocol_ansonic : public c_rf_protocol {
public:
    protocol_ansonic() {
        transposition_table['0'] = {-1111, 555};
        transposition_table['1'] = {-555, 1111};
        pilot_period = {-19425, 555};
        stop_bit = {};
    }
};
