#pragma once

#include "kia_generic.h"
#include <lib/toolbox/manchester_decoder.h>

#include "../defines.h"

#define KIA_PROTOCOL_V5_NAME "Kia V5"

typedef struct SubGhzProtocolDecoderKiaV5 SubGhzProtocolDecoderKiaV5;
typedef struct SubGhzProtocolEncoderKiaV5 SubGhzProtocolEncoderKiaV5;

extern const SubGhzProtocolDecoder kia_protocol_v5_decoder;
extern const SubGhzProtocolEncoder kia_protocol_v5_encoder;
extern const SubGhzProtocol kia_protocol_v5;

void* kia_protocol_decoder_v5_alloc(SubGhzEnvironment* environment);
void kia_protocol_decoder_v5_free(void* context);
void kia_protocol_decoder_v5_reset(void* context);
void kia_protocol_decoder_v5_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus kia_protocol_decoder_v5_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    kia_protocol_decoder_v5_deserialize(void* context, FlipperFormat* flipper_format);
void kia_protocol_decoder_v5_get_string(void* context, FuriString* output);

void* kia_protocol_encoder_v5_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    kia_protocol_encoder_v5_deserialize(void* context, FlipperFormat* flipper_format);
