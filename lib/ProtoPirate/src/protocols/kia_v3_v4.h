#pragma once

#include "kia_generic.h"

#include "../defines.h"

#define KIA_PROTOCOL_V3_V4_NAME "Kia V3/V4"

extern const SubGhzProtocol kia_protocol_v3_v4;

// Decoder functions
void* kia_protocol_decoder_v3_v4_alloc(SubGhzEnvironment* environment);
void kia_protocol_decoder_v3_v4_free(void* context);
void kia_protocol_decoder_v3_v4_reset(void* context);
void kia_protocol_decoder_v3_v4_feed(void* context, bool level, uint32_t duration);
SubGhzProtocolStatus kia_protocol_decoder_v3_v4_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);
SubGhzProtocolStatus
    kia_protocol_decoder_v3_v4_deserialize(void* context, FlipperFormat* flipper_format);
void kia_protocol_decoder_v3_v4_get_string(void* context, FuriString* output);

// Encoder functions
void* kia_protocol_encoder_v3_v4_alloc(SubGhzEnvironment* environment);
SubGhzProtocolStatus
    kia_protocol_encoder_v3_v4_deserialize(void* context, FlipperFormat* flipper_format);
void kia_protocol_encoder_v3_v4_stop(void* context);
LevelDuration kia_protocol_encoder_v3_v4_yield(void* context);

// Encoder helper functions for UI
void kia_protocol_encoder_v3_v4_set_button(void* context, uint8_t button);
void kia_protocol_encoder_v3_v4_set_counter(void* context, uint16_t counter);
void kia_protocol_encoder_v3_v4_increment_counter(void* context);
uint16_t kia_protocol_encoder_v3_v4_get_counter(void* context);
uint8_t kia_protocol_encoder_v3_v4_get_button(void* context);
