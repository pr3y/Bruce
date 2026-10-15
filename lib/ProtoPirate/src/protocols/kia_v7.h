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

#define KIA_PROTOCOL_V7_NAME "Kia V7"

typedef struct SubGhzProtocolDecoderKiaV7 SubGhzProtocolDecoderKiaV7;
typedef struct SubGhzProtocolEncoderKiaV7 SubGhzProtocolEncoderKiaV7;

extern const SubGhzProtocolDecoder kia_protocol_v7_decoder;
extern const SubGhzProtocolEncoder kia_protocol_v7_encoder;
extern const SubGhzProtocol kia_protocol_v7;

void* kia_protocol_decoder_v7_alloc(SubGhzEnvironment* environment);
void kia_protocol_decoder_v7_free(void* context);
void kia_protocol_decoder_v7_reset(void* context);
void kia_protocol_decoder_v7_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus kia_protocol_decoder_v7_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    kia_protocol_decoder_v7_deserialize(void* context, FlipperFormat* flipper_format);
void kia_protocol_decoder_v7_get_string(void* context, FuriString* output);

#if PROTOPIRATE_WITH_ENCODER
void* kia_protocol_encoder_v7_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    kia_protocol_encoder_v7_deserialize(void* context, FlipperFormat* flipper_format);
#endif
