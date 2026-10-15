#pragma once

#include <furi.h>
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/types.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include <lib/subghz/blocks/generic.h>
#include <flipper_format/flipper_format.h>

#include "../defines.h"

#define HONDA_V1_PROTOCOL_NAME "Honda V1"

typedef struct SubGhzProtocolDecoderHondaV1 SubGhzProtocolDecoderHondaV1;
typedef struct SubGhzProtocolEncoderHondaV1 SubGhzProtocolEncoderHondaV1;

extern const SubGhzProtocol honda_v1_protocol;

void* subghz_protocol_decoder_honda_v1_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_honda_v1_reset(void* context);
void subghz_protocol_decoder_honda_v1_feed(void* context, bool level, uint32_t duration);
uint8_t subghz_protocol_decoder_honda_v1_get_hash_data(void* context);
SubGhzProtocolStatus subghz_protocol_decoder_honda_v1_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_honda_v1_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_honda_v1_get_string(void* context, FuriString* output);

#if PROTOPIRATE_WITH_ENCODER
void* subghz_protocol_encoder_honda_v1_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    subghz_protocol_encoder_honda_v1_deserialize(void* context, FlipperFormat* flipper_format);
#endif
