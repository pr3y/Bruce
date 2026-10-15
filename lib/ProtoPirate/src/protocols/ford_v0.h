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
#include <lib/toolbox/manchester_decoder.h>

#include "../defines.h"

#define FORD_PROTOCOL_V0_NAME "Ford V0"

extern const SubGhzProtocol ford_protocol_v0;

// Decoder functions
void* subghz_protocol_decoder_ford_v0_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_ford_v0_free(void* context);
void subghz_protocol_decoder_ford_v0_reset(void* context);
void subghz_protocol_decoder_ford_v0_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus subghz_protocol_decoder_ford_v0_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_ford_v0_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_ford_v0_get_string(void* context, FuriString* output);

// Encoder functions
void* subghz_protocol_encoder_ford_v0_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    subghz_protocol_encoder_ford_v0_deserialize(void* context, FlipperFormat* flipper_format);
