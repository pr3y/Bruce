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
#include <lib/toolbox/level_duration.h>
#include <lib/toolbox/manchester_decoder.h>

#include "../defines.h"

#define MAZDA_PROTOCOL_V0_NAME "Mazda V0"

extern const SubGhzProtocol mazda_v0_protocol;

void* subghz_protocol_decoder_mazda_v0_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_mazda_v0_free(void* context);
void subghz_protocol_decoder_mazda_v0_reset(void* context);
void subghz_protocol_decoder_mazda_v0_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus subghz_protocol_decoder_mazda_v0_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_mazda_v0_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_mazda_v0_get_string(void* context, FuriString* output);

void* subghz_protocol_encoder_mazda_v0_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    subghz_protocol_encoder_mazda_v0_deserialize(void* context, FlipperFormat* flipper_format);
