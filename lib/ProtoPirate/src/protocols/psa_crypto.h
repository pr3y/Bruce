#pragma once

#include "bruteforce_types.h"
#include "../defines.h"
#include <stdint.h>

#define PSA_CRYPTO_BF1_CONST_U4 0x0E0F5C41U
#define PSA_CRYPTO_BF1_CONST_U5 0x0F5C4123U

#define PSA_CRYPTO_BF1_START 0x23000000U
#define PSA_CRYPTO_BF1_END   0x24000000U
#define PSA_CRYPTO_BF2_START 0xF3000000U
#define PSA_CRYPTO_BF2_END   0xF4000000U

extern const uint32_t psa_crypto_bf1_key_schedule[4];
extern const uint32_t psa_crypto_bf2_key_schedule[4];

void psa_crypto_setup_byte_buffer(
    uint8_t* buffer,
    uint32_t key1_low,
    uint32_t key1_high,
    uint32_t key2_low);
void psa_crypto_prepare_tea_data(const uint8_t* buffer, uint32_t* w0, uint32_t* w1);
uint8_t psa_crypto_tea_crc(uint32_t v0, uint32_t v1);
uint16_t psa_crypto_crc16_bf2(uint8_t* buffer, int length);
void psa_crypto_unpack_tea_result_to_buffer(uint8_t* buffer, uint32_t v0, uint32_t v1);

#if PROTOPIRATE_WITH_ENCODER
void psa_crypto_tea_encrypt(uint32_t* v0, uint32_t* v1, const uint32_t* key);
#endif
