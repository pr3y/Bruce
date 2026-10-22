// SPDX-License-Identifier: AGPL-3.0-or-later
#include "rf_secplus.h"
#include <secplus.h>

static void pulse(std::vector<int> &out, bool high, int us) {
    int value = high ? us : -us;
    if (!out.empty() && (out.back() > 0) == high) out.back() += value;
    else out.push_back(value);
}

bool rf_secplus_durations(bool v2, uint32_t rolling, uint64_t fixed, std::vector<int> &out) {
    out.clear();
    uint8_t first[8] = {}, second[8] = {};
    uint8_t symbols1[20] = {}, symbols2[20] = {};
    if (v2) {
        if (encode_v2(rolling, fixed, 0, 0, first, second) < 0) return false;
    } else {
        if (fixed >= 3486784401ULL || encode_v1(rolling, (uint32_t)fixed, symbols1, symbols2) < 0)
            return false;
    }
    out.reserve(1100);
    for (int repeat = 0; repeat < 4; ++repeat) {
        for (int half = 0; half < 2; ++half) {
            if (v2) {
                auto bit = [&](bool one) {
                    pulse(out, !one, 250);
                    pulse(out, one, 250);
                };
                // 20-bit preamble, 2-bit packet ID, 40-bit short payload.
                for (int i = 0; i < 20; ++i) bit(i >= 16);
                bit(false);
                bit(half != 0);
                const uint8_t *packet = half ? second : first;
                for (int i = 0; i < 40; ++i) bit((packet[i / 8] >> (7 - i % 8)) & 1);
                // Reference slow framing: each packet occupies 100 ms.
                pulse(out, false, (400 - 124) * 250);
            } else {
                const uint8_t *symbols = half ? symbols2 : symbols1;
                auto symbol = [&](uint8_t value) {
                    pulse(out, false, 1500 - 500 * value);
                    pulse(out, true, 500 + 500 * value);
                };
                symbol(half ? 2 : 0);
                for (int i = 0; i < 20; ++i) symbol(symbols[i]);
                pulse(out, false, 58000);
            }
        }
    }
    return true;
}
