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

#define FORD_PROTOCOL_V3_NAME "Ford V3"

extern const SubGhzProtocol ford_protocol_v3;

void* subghz_protocol_decoder_ford_v3_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_ford_v3_reset(void* context);
void subghz_protocol_decoder_ford_v3_feed(void* context, bool level, uint32_t duration);
uint8_t subghz_protocol_decoder_ford_v3_get_hash_data(void* context);
SubGhzProtocolStatus subghz_protocol_decoder_ford_v3_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_ford_v3_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_ford_v3_get_string(void* context, FuriString* output);
