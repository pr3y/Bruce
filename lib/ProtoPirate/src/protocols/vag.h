#pragma once

#include <furi.h>
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/types.h>
#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include <lib/subghz/blocks/generic.h>
#include <lib/subghz/blocks/math.h>
#include <lib/toolbox/manchester_decoder.h>
#include <flipper_format/flipper_format.h>

#include "../defines.h"

#define VAG_PROTOCOL_NAME "VAG"

extern const SubGhzProtocol vag_protocol;

// Decoder functions
void* subghz_protocol_decoder_vag_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_vag_free(void* context);
void subghz_protocol_decoder_vag_reset(void* context);
void subghz_protocol_decoder_vag_feed(void* context, bool level, uint32_t duration);
uint8_t subghz_protocol_decoder_vag_get_hash_data(void* context);
SubGhzProtocolStatus subghz_protocol_decoder_vag_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_vag_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_vag_get_string(void* context, FuriString* output);

// Encoder functions
void* subghz_protocol_encoder_vag_alloc(SubGhzEnvironment* environment);
void subghz_protocol_encoder_vag_free(void* context);
SubGhzProtocolStatus
    subghz_protocol_encoder_vag_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_encoder_vag_stop(void* context);
LevelDuration subghz_protocol_encoder_vag_yield(void* context);
