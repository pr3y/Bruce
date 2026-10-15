#pragma once

#include "kia_generic.h"

#include "../defines.h"

#define KIA_PROTOCOL_V0_NAME "Kia V0"

typedef struct SubGhzProtocolDecoderKIA SubGhzProtocolDecoderKIA;
typedef struct SubGhzProtocolEncoderKIA SubGhzProtocolEncoderKIA;

extern const SubGhzProtocolDecoder subghz_protocol_kia_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_kia_encoder;
extern const SubGhzProtocol kia_protocol_v0;

// Decoder functions
void* subghz_protocol_decoder_kia_alloc(SubGhzEnvironment* environment);
void subghz_protocol_decoder_kia_free(void* context);
void subghz_protocol_decoder_kia_reset(void* context);
void subghz_protocol_decoder_kia_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus subghz_protocol_decoder_kia_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    subghz_protocol_decoder_kia_deserialize(void* context, FlipperFormat* flipper_format);
void subghz_protocol_decoder_kia_get_string(void* context, FuriString* output);

// Encoder helper functions
void subghz_protocol_encoder_kia_set_button(void* context, uint8_t button);
void subghz_protocol_encoder_kia_set_counter(void* context, uint16_t counter);
void subghz_protocol_encoder_kia_increment_counter(void* context);
uint16_t subghz_protocol_encoder_kia_get_counter(void* context);
uint8_t subghz_protocol_encoder_kia_get_button(void* context);
