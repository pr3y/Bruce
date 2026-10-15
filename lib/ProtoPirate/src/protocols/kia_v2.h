#pragma once

#include "kia_generic.h"
#include <lib/toolbox/manchester_decoder.h>
#include "protocols_common.h"

#include "../defines.h"

#define KIA_PROTOCOL_V2_NAME "Kia V2"

typedef struct SubGhzProtocolDecoderKiaV2 SubGhzProtocolDecoderKiaV2;
typedef struct SubGhzProtocolEncoderKiaV2 SubGhzProtocolEncoderKiaV2;

extern const SubGhzProtocolDecoder kia_protocol_v2_decoder;
extern const SubGhzProtocolEncoder kia_protocol_v2_encoder;
extern const SubGhzProtocol kia_protocol_v2;

void* kia_protocol_decoder_v2_alloc(SubGhzEnvironment* environment);
void kia_protocol_decoder_v2_free(void* context);
void kia_protocol_decoder_v2_reset(void* context);
void kia_protocol_decoder_v2_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus kia_protocol_decoder_v2_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    kia_protocol_decoder_v2_deserialize(void* context, FlipperFormat* flipper_format);
void kia_protocol_decoder_v2_get_string(void* context, FuriString* output);

void* kia_protocol_encoder_v2_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    kia_protocol_encoder_v2_deserialize(void* context, FlipperFormat* flipper_format);
