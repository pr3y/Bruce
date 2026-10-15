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

#define CHRYSLER_PROTOCOL_V0_NAME "Chrysler V0"

typedef struct SubGhzProtocolDecoderChrysler_V0 SubGhzProtocolDecoderChrysler_V0;
typedef struct SubGhzProtocolEncoderChrysler_V0 SubGhzProtocolEncoderChrysler_V0;

extern const SubGhzProtocol chrysler_protocol_v0;

void* subghz_protocol_decoder_chrysler_v0_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_chrysler_v0_reset(void* context);
void subghz_protocol_decoder_chrysler_v0_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus subghz_protocol_decoder_chrysler_v0_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_chrysler_v0_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_chrysler_v0_get_string(void* context, FuriString* output);

#if PROTOPIRATE_WITH_ENCODER
void* subghz_protocol_encoder_chrysler_v0_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    subghz_protocol_encoder_chrysler_v0_deserialize(void* context, FlipperFormat* flipper_format);
#endif
