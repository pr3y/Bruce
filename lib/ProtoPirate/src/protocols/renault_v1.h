#pragma once

#include <furi.h>
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/types.h>
#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include <lib/subghz/blocks/generic.h>
#include <lib/subghz/blocks/math.h>
#include <flipper_format/flipper_format.h>

#include "../defines.h"

#define RENAULT_PROTOCOL_V1_NAME "Renault V1"

typedef struct SubGhzProtocolDecoderRenaultV1 SubGhzProtocolDecoderRenaultV1;
typedef struct SubGhzProtocolEncoderRenaultV1 SubGhzProtocolEncoderRenaultV1;

#define HITAG2_BF_STATUS_IDLE      0
#define HITAG2_BF_STATUS_RUNNING   1
#define HITAG2_BF_STATUS_FOUND     2
#define HITAG2_BF_STATUS_NOT_FOUND 3
#define HITAG2_BF_STATUS_CANCELLED 4

typedef struct Hitag2BfState Hitag2BfState;
struct Hitag2BfState {
    volatile uint8_t cancel;
    volatile uint32_t progress_current;
    volatile uint8_t status;
    volatile uint32_t progress_total;
    uint8_t frame[11];
    uint8_t iv[4];
    void (*on_done)(void* context);
    void* on_done_ctx;
};

int32_t hitag2_brute_force_thread_entry(void* arg);
bool hitag2_bf_state_from_flipper_format(Hitag2BfState* state, FlipperFormat* ff);
bool hitag2_bf_needs_bruteforce(FlipperFormat* ff);
bool hitag2_bf_patch_flipper_format_on_success(FlipperFormat* ff, const Hitag2BfState* state);
bool hitag2_bf_patch_flipper_format_on_miss(FlipperFormat* ff);
bool hitag2_flipper_format_get_string(FlipperFormat* ff, FuriString* output);

extern const SubGhzProtocol renault_v1_protocol;

void* subghz_protocol_decoder_renault_v1_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_renault_v1_reset(void* context);
void subghz_protocol_decoder_renault_v1_feed(void* context, bool level, uint32_t duration);
uint8_t subghz_protocol_decoder_renault_v1_get_hash_data(void* context);
SubGhzProtocolStatus subghz_protocol_decoder_renault_v1_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_renault_v1_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_renault_v1_get_string(void* context, FuriString* output);

#if PROTOPIRATE_WITH_ENCODER
void* subghz_protocol_encoder_renault_v1_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    subghz_protocol_encoder_renault_v1_deserialize(void* context, FlipperFormat* flipper_format);
#endif
