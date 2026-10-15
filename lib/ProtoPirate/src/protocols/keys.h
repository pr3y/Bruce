#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <lib/subghz/environment.h>
#include <lib/subghz/subghz_keystore.h>

extern uint64_t kia_mf_key;
extern uint64_t kia_v6_a_key;
extern uint64_t kia_v6_b_key;
extern uint64_t kia_v5_key;

uint64_t get_kia_mf_key();

uint64_t get_kia_v6_keystore_a();

uint64_t get_kia_v6_keystore_b();

uint64_t get_kia_v5_key();

bool protopirate_keys_has_kia_mf_key(void);
bool protopirate_keys_has_kia_v6_keystore_a(void);
bool protopirate_keys_has_kia_v6_keystore_b(void);
bool protopirate_keys_has_kia_v5_key(void);

bool protopirate_keys_load(SubGhzEnvironment* environment);
