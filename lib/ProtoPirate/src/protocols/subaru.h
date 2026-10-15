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

#define SUBARU_PROTOCOL_NAME "Subaru"

extern const SubGhzProtocol subaru_protocol;

// Decoder functions
void* subghz_protocol_decoder_subaru_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_subaru_free(void* context);
void subghz_protocol_decoder_subaru_reset(void* context);
void subghz_protocol_decoder_subaru_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus subghz_protocol_decoder_subaru_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_subaru_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_subaru_get_string(void* context, FuriString* output);

// Encoder functions
void* subghz_protocol_encoder_subaru_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    subghz_protocol_encoder_subaru_deserialize(void* context, FlipperFormat* flipper_format);
