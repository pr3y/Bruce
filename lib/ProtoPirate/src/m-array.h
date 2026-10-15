#pragma once
#include <lib/subghz/subghz_keystore.h>
#define M_EACH(it, arr, type) (SubGhzKey *it = (arr).items; (arr).items && it < (arr).items + (arr).count; ++it)
