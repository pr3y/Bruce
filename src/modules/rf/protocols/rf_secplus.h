// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <stdint.h>
#include <vector>

// Build four identical frame pairs for one button press. No counter mutation.
bool rf_secplus_durations(bool v2, uint32_t rolling, uint64_t fixed, std::vector<int> &out);
