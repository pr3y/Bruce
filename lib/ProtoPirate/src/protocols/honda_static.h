#pragma once

#include <furi.h>
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/types.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include <lib/subghz/blocks/generic.h>
#include <flipper_format/flipper_format.h>

#include "../defines.h"

#define HONDA_STATIC_PROTOCOL_NAME "Honda Static"

typedef struct SubGhzProtocolDecoderHondaStatic SubGhzProtocolDecoderHondaStatic;
typedef struct SubGhzProtocolEncoderHondaStatic SubGhzProtocolEncoderHondaStatic;

extern const SubGhzProtocol honda_static_protocol;

void* subghz_protocol_decoder_honda_static_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_honda_static_free(void* context);
void subghz_protocol_decoder_honda_static_reset(void* context);
void subghz_protocol_decoder_honda_static_feed(void* context, bool level, uint32_t duration);
uint8_t subghz_protocol_decoder_honda_static_get_hash_data(void* context);
SubGhzProtocolStatus subghz_protocol_decoder_honda_static_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_honda_static_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_honda_static_get_string(void* context, FuriString* output);

#if PROTOPIRATE_WITH_ENCODER
void* subghz_protocol_encoder_honda_static_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    subghz_protocol_encoder_honda_static_deserialize(void* context, FlipperFormat* flipper_format);
#endif
