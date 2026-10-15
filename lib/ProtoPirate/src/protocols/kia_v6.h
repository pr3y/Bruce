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
#include "kia_generic.h"

#include "../defines.h"

#define KIA_PROTOCOL_V6_NAME "Kia V6"

typedef struct SubGhzProtocolDecoderKiaV6 SubGhzProtocolDecoderKiaV6;
typedef struct SubGhzProtocolEncoderKiaV6 SubGhzProtocolEncoderKiaV6;

extern const SubGhzProtocolDecoder kia_protocol_v6_decoder;
extern const SubGhzProtocolEncoder kia_protocol_v6_encoder;
extern const SubGhzProtocol kia_protocol_v6;

// Decoder functions
void* kia_protocol_decoder_v6_alloc(SubGhzEnvironment* environment);
void kia_protocol_decoder_v6_free(void* context);
void kia_protocol_decoder_v6_reset(void* context);
void kia_protocol_decoder_v6_feed(void* context, bool level, uint32_t duration);
uint8_t kia_protocol_decoder_v6_get_hash_data(void* context);
SubGhzProtocolStatus kia_protocol_decoder_v6_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    kia_protocol_decoder_v6_deserialize(void* context, FlipperFormat* flipper_format);
void kia_protocol_decoder_v6_get_string(void* context, FuriString* output);

#if PROTOPIRATE_WITH_ENCODER
void* kia_protocol_encoder_v6_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    kia_protocol_encoder_v6_deserialize(void* context, FlipperFormat* flipper_format);
#endif
