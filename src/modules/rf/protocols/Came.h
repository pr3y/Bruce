#pragma once

#include "protocol.h"

class protocol_came : public c_rf_protocol {
public:
    protocol_came() {
        transposition_table['0'] = {-320, 640};
        transposition_table['1'] = {-640, 320};
        pilot_period = {-11520, 320};
        stop_bit = {};
    }
};
