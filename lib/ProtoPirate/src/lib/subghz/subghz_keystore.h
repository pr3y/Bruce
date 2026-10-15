#pragma once
#include <furi.h>
#include "environment.h"
typedef struct { FuriString *name; uint64_t key; uint16_t type; } SubGhzKey;
typedef struct { SubGhzKey *items; size_t count; } SubGhzKeyArray_t;
#define M_EACH(it, arr, type) (SubGhzKey *it = (arr).items; (arr).items && it < (arr).items + (arr).count; ++it)
SubGhzKeyArray_t *subghz_keystore_get_data(SubGhzKeystore *k);
bool subghz_keystore_raw_get_data(const char *name, size_t offset, uint8_t *out, size_t length);
