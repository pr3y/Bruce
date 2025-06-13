#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <map>
#include <stdint.h>
#include <vector>

class c_rf_protocol {
public:
    std::map<char, std::vector<int>> transposition_table;
    std::vector<int> pilot_period;
    std::vector<int> stop_bit;

    c_rf_protocol() = default;
    virtual ~c_rf_protocol() = default;
};

#endif
