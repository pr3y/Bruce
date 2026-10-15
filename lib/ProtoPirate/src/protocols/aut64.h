#pragma once

#include <stdint.h>
#include <stdbool.h>

// uncomment to activate key validation, index boundary verifications, ...
//#define AUT64_ENABLE_VALIDATIONS

// uncomment to add compilation of aut64_pack (currently unused in the code)
//#define AUT64_PACK_SUPPORT

#define AUT64_NUM_ROUNDS      12
#define AUT64_BLOCK_SIZE      8
#define AUT64_KEY_SIZE        8
#define AUT64_PBOX_SIZE       8
#define AUT64_SBOX_SIZE       16
#define AUT64_PACKED_KEY_SIZE 16

// Internal helper table size (offset lookup table).
#define AUT64_OFFSET_TABLE_SIZE 256

// Status codes. Keep it simple and C-friendly.
#define AUT64_OK                 0
#define AUT64_ERR_INVALID_KEY    (-1)
#define AUT64_ERR_INVALID_PACKED (-2)
#define AUT64_ERR_NULL_POINTER   (-3)

struct aut64_key {
    uint8_t index;
    uint8_t key[AUT64_KEY_SIZE];
    uint8_t pbox[AUT64_PBOX_SIZE];
    uint8_t sbox[AUT64_SBOX_SIZE];
};

#ifdef AUT64_ENABLE_VALIDATIONS
// Optional helper if callers want to check keys up-front.
int aut64_validate_key(const struct aut64_key* key);
#endif

// Pointers are used for both the key and the message to avoid implicit copies.
// The message buffer must be at least AUT64_BLOCK_SIZE bytes.
int aut64_encrypt(const struct aut64_key* key, uint8_t* message);
int aut64_decrypt(const struct aut64_key* key, uint8_t* message);

// Packed key buffer must be at least AUT64_PACKED_KEY_SIZE bytes.
#ifdef AUT64_PACK_SUPPORT
int aut64_pack(uint8_t* dest, const struct aut64_key* src);
#endif
int aut64_unpack(struct aut64_key* dest, const uint8_t* src);
