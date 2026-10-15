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

#define SUBGHZ_PROTOCOL_SCHER_KHAN_NAME "Scher-Khan"

typedef struct SubGhzProtocolDecoderScherKhan SubGhzProtocolDecoderScherKhan;

extern const SubGhzProtocolDecoder subghz_protocol_scher_khan_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_scher_khan_encoder;
extern const SubGhzProtocol subghz_protocol_scher_khan;

/**
 * Allocate SubGhzProtocolDecoderScherKhan.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderScherKhan* pointer to a SubGhzProtocolDecoderScherKhan instance
 */
void* subghz_protocol_decoder_scher_khan_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderScherKhan.
 * @param context Pointer to a SubGhzProtocolDecoderScherKhan instance
 */
void subghz_protocol_decoder_scher_khan_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderScherKhan.
 * @param context Pointer to a SubGhzProtocolDecoderScherKhan instance
 */
void subghz_protocol_decoder_scher_khan_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderScherKhan instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_scher_khan_feed(void* context, bool level, uint32_t duration);

/**
 * Serialize data SubGhzProtocolDecoderScherKhan.
 * @param context Pointer to a SubGhzProtocolDecoderScherKhan instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_scher_khan_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data SubGhzProtocolDecoderScherKhan.
 * @param context Pointer to a SubGhzProtocolDecoderScherKhan instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    subghz_protocol_decoder_scher_khan_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderScherKhan instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_scher_khan_get_string(void* context, FuriString* output);
