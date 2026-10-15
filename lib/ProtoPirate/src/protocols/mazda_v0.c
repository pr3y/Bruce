#include "mazda_v0.h"
#include "protocols_common.h"

#include <string.h>

// =============================================================================
// PROTOCOL CONSTANTS
// =============================================================================

static const SubGhzBlockConst subghz_protocol_mazda_v0_const = {
    .te_short = 250,
    .te_long = 500,
    .te_delta = 100,
    .min_count_bit_for_found = 64,
};

#define MAZDA_V0_UPLOAD_CAPACITY (((12U + 3U + 8U + 1U) * 16U) + 2U)
_Static_assert(
    MAZDA_V0_UPLOAD_CAPACITY <= PP_SHARED_UPLOAD_CAPACITY,
    "MAZDA_V0_UPLOAD_CAPACITY exceeds shared upload slab");
#define MAZDA_V0_GAP_US        0xCB20
#define MAZDA_V0_SYNC_BYTE     0xD7
#define MAZDA_V0_TAIL_BYTE     0x5A
#define MAZDA_V0_PREAMBLE_ONES 16

// =============================================================================
// STRUCT DEFINITIONS
// =============================================================================

typedef struct SubGhzProtocolDecoderMazdaV0 {
    SubGhzProtocolDecoderBase base;
    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    ManchesterState manchester_state;
    uint16_t preamble_count;
    uint8_t preamble_pattern;

    uint32_t serial;
    uint8_t button;
    uint32_t count;
} SubGhzProtocolDecoderMazdaV0;

#if PROTOPIRATE_WITH_ENCODER
typedef struct SubGhzProtocolEncoderMazdaV0 {
    SubGhzProtocolEncoderBase base;
    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;

    uint32_t serial;
    uint8_t button;
    uint32_t count;
} SubGhzProtocolEncoderMazdaV0;
#endif

typedef enum {
    MazdaV0DecoderStepReset = 0,
    MazdaV0DecoderStepPreamble = 5,
    MazdaV0DecoderStepData = 6,
} MazdaV0DecoderStep;

// =============================================================================
// FUNCTION PROTOTYPES
// =============================================================================

static bool mazda_v0_get_event(uint32_t duration, bool level, ManchesterEvent* event);
static void mazda_v0_decode_key(SubGhzBlockGeneric* generic);
#if PROTOPIRATE_WITH_ENCODER
static uint64_t mazda_v0_encode_key(uint32_t serial, uint8_t button, uint32_t counter);
static bool mazda_v0_encoder_add_level(
    SubGhzProtocolEncoderMazdaV0* instance,
    size_t* index,
    bool level,
    uint32_t duration);
static bool
    mazda_v0_append_byte(SubGhzProtocolEncoderMazdaV0* instance, size_t* index, uint8_t value);
static bool mazda_v0_build_upload(SubGhzProtocolEncoderMazdaV0* instance);
#endif
static SubGhzProtocolStatus mazda_v0_write_display(
    FlipperFormat* flipper_format,
    const char* protocol_name,
    uint8_t button);

// =============================================================================
// PROTOCOL INTERFACE DEFINITIONS
// =============================================================================

const SubGhzProtocolDecoder subghz_protocol_mazda_v0_decoder = {
    .alloc = subghz_protocol_decoder_mazda_v0_alloc,
    .free = pp_decoder_free_default,
    .feed = subghz_protocol_decoder_mazda_v0_feed,
    .reset = subghz_protocol_decoder_mazda_v0_reset,
    .get_hash_data = pp_decoder_hash_blocks,
    .serialize = subghz_protocol_decoder_mazda_v0_serialize,
    .deserialize = subghz_protocol_decoder_mazda_v0_deserialize,
    .get_string = subghz_protocol_decoder_mazda_v0_get_string,
};

#if PROTOPIRATE_WITH_ENCODER
const SubGhzProtocolEncoder subghz_protocol_mazda_v0_encoder = {
    .alloc = subghz_protocol_encoder_mazda_v0_alloc,
    .free = pp_encoder_free,
    .deserialize = subghz_protocol_encoder_mazda_v0_deserialize,
    .stop = pp_encoder_stop,
    .yield = pp_encoder_yield,
};
#else
const SubGhzProtocolEncoder subghz_protocol_mazda_v0_encoder = {
    .alloc = NULL,
    .free = NULL,
    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};
#endif

const SubGhzProtocol mazda_v0_protocol = {
    .name = MAZDA_PROTOCOL_V0_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM |
            SubGhzProtocolFlag_FM | SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load |
            SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,
#if PROTOPIRATE_WITH_DECODER
    .decoder = &subghz_protocol_mazda_v0_decoder,
#else
    .decoder = NULL,
#endif
#if PROTOPIRATE_WITH_ENCODER
    .encoder = &subghz_protocol_mazda_v0_encoder,
#else
    .encoder = NULL,
#endif
};

// =============================================================================
// HELPERS
// =============================================================================

static uint8_t mazda_v0_calculate_checksum(uint32_t serial, uint8_t button, uint32_t counter) {
    counter &= 0xFFFFFU;
    return (uint8_t)(((serial >> 24) & 0xFF) + ((serial >> 16) & 0xFF) + ((serial >> 8) & 0xFF) +
                     (serial & 0xFF) + ((counter >> 8) & 0xFF) + (counter & 0xFF) +
                     ((((counter >> 16) & 0x0F) | ((button & 0x0F) << 4)) & 0xFF));
}

static const char* mazda_v0_get_button_name(uint8_t button) {
    switch(button) {
    case 0x01:
        return "Lock";
    case 0x02:
        return "Unlock";
    case 0x04:
        return "Trunk";
    case 0x08:
        return "Remote";
    default:
        return "??";
    }
}

static bool mazda_v0_get_event(uint32_t duration, bool level, ManchesterEvent* event) {
    const uint32_t tol = (uint32_t)subghz_protocol_mazda_v0_const.te_delta + 20U;

    if((uint32_t)DURATION_DIFF(duration, subghz_protocol_mazda_v0_const.te_short) < tol) {
        *event = level ? ManchesterEventShortLow : ManchesterEventShortHigh;
        return true;
    }

    if((uint32_t)DURATION_DIFF(duration, subghz_protocol_mazda_v0_const.te_long) < tol) {
        *event = level ? ManchesterEventLongLow : ManchesterEventLongHigh;
        return true;
    }

    return false;
}

static void mazda_v0_decode_key(SubGhzBlockGeneric* generic) {
    uint8_t data[8];
    pp_u64_to_bytes_be(generic->data, data);

    const bool parity = subghz_protocol_blocks_parity8(data[7]) != 0;
    const uint8_t limit = parity ? 6 : 5;
    const uint8_t mask = data[limit];

    for(uint8_t i = 0; i < limit; i++) {
        data[i] ^= mask;
    }

    if(!parity) {
        data[6] ^= mask;
    }

    const uint8_t counter_lo = (data[5] & 0x55) | (data[6] & 0xAA);
    const uint8_t counter_mid = (data[6] & 0x55) | (data[5] & 0xAA);

    generic->serial = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                      ((uint32_t)data[2] << 8) | (uint32_t)data[3];
    generic->btn = (data[4] >> 4) & 0x0F;
    generic->cnt = (((uint32_t)data[4] & 0x0F) << 16) | ((uint32_t)counter_mid << 8) |
                   (uint32_t)counter_lo;
    generic->data_count_bit = subghz_protocol_mazda_v0_const.min_count_bit_for_found;
}

#if PROTOPIRATE_WITH_ENCODER
static uint64_t mazda_v0_encode_key(uint32_t serial, uint8_t button, uint32_t counter) {
    uint8_t data[8];

    counter &= 0xFFFFFU;
    button &= 0x0F;

    data[0] = (serial >> 24) & 0xFF;
    data[1] = (serial >> 16) & 0xFF;
    data[2] = (serial >> 8) & 0xFF;
    data[3] = serial & 0xFF;
    data[4] = (button << 4) | ((counter >> 16) & 0x0F);
    data[5] = (counter >> 8) & 0xFF;
    data[6] = counter & 0xFF;
    data[7] = mazda_v0_calculate_checksum(serial, button, counter);

    const uint8_t stored_5 = (data[6] & 0x55) | (data[5] & 0xAA);
    const uint8_t stored_6 = (data[6] & 0xAA) | (data[5] & 0x55);
    const uint8_t xor_mask = stored_5 ^ stored_6;
    const bool replace_second = subghz_protocol_blocks_parity8(data[7]) == 0;
    const uint8_t forward_mask = replace_second ? stored_5 : stored_6;

    data[5] = replace_second ? stored_5 : xor_mask;
    data[6] = replace_second ? xor_mask : stored_6;

    for(size_t i = 0; i < 5; i++) {
        data[i] ^= forward_mask;
    }

    return pp_bytes_to_u64_be(data);
}

static bool mazda_v0_encoder_add_level(
    SubGhzProtocolEncoderMazdaV0* instance,
    size_t* index,
    bool level,
    uint32_t duration) {
    size_t before = *index;
    *index = pp_emit(instance->encoder.upload, before, MAZDA_V0_UPLOAD_CAPACITY, level, duration);
    return *index > before;
}

static bool
    mazda_v0_append_byte(SubGhzProtocolEncoderMazdaV0* instance, size_t* index, uint8_t value) {
    if(*index + 16 > MAZDA_V0_UPLOAD_CAPACITY) {
        return false;
    }
    *index = pp_emit_byte_manchester(
        instance->encoder.upload,
        *index,
        MAZDA_V0_UPLOAD_CAPACITY,
        value,
        subghz_protocol_mazda_v0_const.te_short);
    return true;
}

static bool mazda_v0_build_upload(SubGhzProtocolEncoderMazdaV0* instance) {
    size_t index = 0;
    const uint64_t key64 = instance->generic.data;

    for(size_t r = 0; r < 12; r++) {
        if(!mazda_v0_append_byte(instance, &index, 0xFF)) {
            return false;
        }
    }

    if(!mazda_v0_encoder_add_level(instance, &index, false, MAZDA_V0_GAP_US)) {
        return false;
    }

    if(!mazda_v0_append_byte(instance, &index, 0xFF) ||
       !mazda_v0_append_byte(instance, &index, 0xFF) ||
       !mazda_v0_append_byte(instance, &index, MAZDA_V0_SYNC_BYTE)) {
        return false;
    }

    for(int bi = 0; bi < 8; bi++) {
        const uint8_t raw = (uint8_t)((key64 >> (56 - bi * 8)) & 0xFF);
        const uint8_t air = (uint8_t)~raw;
        if(!mazda_v0_append_byte(instance, &index, air)) {
            return false;
        }
    }

    if(!mazda_v0_append_byte(instance, &index, MAZDA_V0_TAIL_BYTE)) {
        return false;
    }

    if(!mazda_v0_encoder_add_level(instance, &index, false, MAZDA_V0_GAP_US)) {
        return false;
    }

    instance->encoder.front = 0;
    instance->encoder.size_upload = index;

    return true;
}
#endif

static SubGhzProtocolStatus mazda_v0_write_display(
    FlipperFormat* flipper_format,
    const char* protocol_name,
    uint8_t button) {
    return pp_write_display(flipper_format, protocol_name, mazda_v0_get_button_name(button));
}

// =============================================================================
// ENCODER
// =============================================================================

#if PROTOPIRATE_WITH_ENCODER
void* subghz_protocol_encoder_mazda_v0_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);

    SubGhzProtocolEncoderMazdaV0* instance = calloc(1, sizeof(SubGhzProtocolEncoderMazdaV0));

    instance->base.protocol = &mazda_v0_protocol;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->encoder.repeat = 10;
    instance->encoder.front = 0;
    instance->encoder.is_running = false;
    pp_encoder_buffer_ensure(instance, MAZDA_V0_UPLOAD_CAPACITY);

    return instance;
}

SubGhzProtocolStatus
    subghz_protocol_encoder_mazda_v0_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolEncoderMazdaV0* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;

    instance->encoder.is_running = false;
    instance->encoder.front = 0;

    do {
        if(pp_verify_protocol_name(flipper_format, instance->base.protocol->name) !=
           SubGhzProtocolStatusOk) {
            break;
        }

        flipper_format_rewind(flipper_format);
        SubGhzProtocolStatus load_st = subghz_block_generic_deserialize_check_count_bit(
            &instance->generic,
            flipper_format,
            subghz_protocol_mazda_v0_const.min_count_bit_for_found);
        if(load_st != SubGhzProtocolStatusOk) {
            break;
        }

        mazda_v0_decode_key(&instance->generic);

        uint32_t serial = instance->generic.serial;
        uint32_t btn = instance->generic.btn;
        uint32_t cnt = instance->generic.cnt;
        pp_encoder_read_fields(flipper_format, &serial, &btn, &cnt, NULL);
        instance->generic.serial = serial;
        instance->generic.btn = (uint8_t)btn & 0x0FU;
        instance->generic.cnt = cnt & 0xFFFFFU;

        instance->encoder.repeat = pp_encoder_read_repeat(flipper_format, 10);

        instance->generic.data = mazda_v0_encode_key(
            instance->generic.serial, instance->generic.btn, instance->generic.cnt);
        instance->generic.data_count_bit = subghz_protocol_mazda_v0_const.min_count_bit_for_found;

        instance->serial = instance->generic.serial;
        instance->button = instance->generic.btn;
        instance->count = instance->generic.cnt;

        if(!mazda_v0_build_upload(instance)) {
            break;
        }
        if(instance->encoder.size_upload == 0) {
            break;
        }

        flipper_format_rewind(flipper_format);
        uint8_t key_data[sizeof(uint64_t)];
        pp_u64_to_bytes_be(instance->generic.data, key_data);
        if(!flipper_format_update_hex(flipper_format, FF_KEY, key_data, sizeof(key_data))) {
            break;
        }

        uint32_t chk =
            mazda_v0_calculate_checksum(instance->serial, instance->button, instance->count);
        flipper_format_rewind(flipper_format);
        flipper_format_insert_or_update_uint32(flipper_format, "Checksum", &chk, 1);

        instance->encoder.is_running = true;

        ret = SubGhzProtocolStatusOk;
    } while(false);

    return ret;
}

#endif

// =============================================================================
// DECODER
// =============================================================================

void* subghz_protocol_decoder_mazda_v0_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);

    SubGhzProtocolDecoderMazdaV0* instance = calloc(1, sizeof(SubGhzProtocolDecoderMazdaV0));

    instance->base.protocol = &mazda_v0_protocol;
    instance->generic.protocol_name = instance->base.protocol->name;

    return instance;
}

void subghz_protocol_decoder_mazda_v0_reset(void* context) {
    SubGhzProtocolDecoderMazdaV0* instance = context;

    instance->decoder.parser_step = MazdaV0DecoderStepReset;
    instance->decoder.te_last = 0;
    instance->decoder.decode_data = 0;
    instance->decoder.decode_count_bit = 0;
    instance->manchester_state = ManchesterStateStart1;
    instance->preamble_count = 0;
    instance->preamble_pattern = 0;
}

void subghz_protocol_decoder_mazda_v0_feed(void* context, bool level, uint32_t duration) {
    SubGhzProtocolDecoderMazdaV0* instance = context;
    ManchesterEvent event = ManchesterEventReset;
    bool data = false;

    switch(instance->decoder.parser_step) {
    case MazdaV0DecoderStepReset:
        if(level && ((uint32_t)DURATION_DIFF(duration, subghz_protocol_mazda_v0_const.te_short) <
                     (uint32_t)subghz_protocol_mazda_v0_const.te_delta + 20U)) {
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
            instance->decoder.parser_step = MazdaV0DecoderStepPreamble;
            instance->manchester_state = ManchesterStateMid1;
            instance->preamble_count = 0;
            instance->preamble_pattern = 0;
        }
        break;

    case MazdaV0DecoderStepPreamble:
        if(!mazda_v0_get_event(duration, level, &event)) {
            instance->decoder.parser_step = MazdaV0DecoderStepReset;
            break;
        }

        if(manchester_advance(
               instance->manchester_state, event, &instance->manchester_state, &data)) {
            instance->preamble_pattern = (instance->preamble_pattern << 1) | (data ? 1 : 0);

            if(data) {
                instance->preamble_count++;
            } else if(instance->preamble_count <= MAZDA_V0_PREAMBLE_ONES - 1U) {
                instance->preamble_count = 0;
                instance->preamble_pattern = 0;
                break;
            }

            if((instance->preamble_pattern == MAZDA_V0_SYNC_BYTE) &&
               (instance->preamble_count > MAZDA_V0_PREAMBLE_ONES - 1U)) {
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.parser_step = MazdaV0DecoderStepData;
            }
        }
        break;

    case MazdaV0DecoderStepData:
        if(!mazda_v0_get_event(duration, level, &event)) {
            instance->decoder.parser_step = MazdaV0DecoderStepReset;
            break;
        }

        if(manchester_advance(
               instance->manchester_state, event, &instance->manchester_state, &data)) {
            subghz_protocol_blocks_add_bit(&instance->decoder, data);

            if(instance->decoder.decode_count_bit ==
               subghz_protocol_mazda_v0_const.min_count_bit_for_found) {
                instance->generic.data = ~instance->decoder.decode_data;
                mazda_v0_decode_key(&instance->generic);

                if(mazda_v0_calculate_checksum(
                       instance->generic.serial, instance->generic.btn, instance->generic.cnt) ==
                   (uint8_t)instance->generic.data) {
                    instance->serial = instance->generic.serial;
                    instance->button = instance->generic.btn;
                    instance->count = instance->generic.cnt;

                    if(instance->base.callback) {
                        instance->base.callback(&instance->base, instance->base.context);
                    }
                }

                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->preamble_count = 0;
                instance->preamble_pattern = 0;
                instance->manchester_state = ManchesterStateStart1;
                instance->decoder.te_last = 0;
                instance->decoder.parser_step = MazdaV0DecoderStepReset;
            }
        }
        break;
    }
}

SubGhzProtocolStatus subghz_protocol_decoder_mazda_v0_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    SubGhzProtocolDecoderMazdaV0* instance = context;

    mazda_v0_decode_key(&instance->generic);
    instance->serial = instance->generic.serial;
    instance->button = instance->generic.btn;
    instance->count = instance->generic.cnt;

    SubGhzProtocolStatus ret =
        subghz_block_generic_serialize(&instance->generic, flipper_format, preset);

    if(ret == SubGhzProtocolStatusOk) {
        uint32_t chk =
            mazda_v0_calculate_checksum(instance->serial, instance->button, instance->count);
        flipper_format_write_uint32(flipper_format, "Checksum", &chk, 1);

        pp_serialize_fields(
            flipper_format,
            PP_FIELD_SERIAL | PP_FIELD_BTN | PP_FIELD_CNT,
            instance->serial,
            instance->button,
            instance->count,
            0);

        ret = mazda_v0_write_display(
            flipper_format, instance->generic.protocol_name, instance->button);
    }

    return ret;
}

SubGhzProtocolStatus
    subghz_protocol_decoder_mazda_v0_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolDecoderMazdaV0* instance = context;

    SubGhzProtocolStatus ret = subghz_block_generic_deserialize_check_count_bit(
        &instance->generic,
        flipper_format,
        subghz_protocol_mazda_v0_const.min_count_bit_for_found);

    if(ret == SubGhzProtocolStatusOk) {
        uint32_t serial = instance->serial;
        uint32_t btn = instance->button;
        uint32_t cnt = instance->count;
        pp_encoder_read_fields(flipper_format, &serial, &btn, &cnt, NULL);
        instance->serial = serial;
        instance->button = (uint8_t)btn;
        instance->count = cnt;
        instance->generic.serial = instance->serial;
        instance->generic.btn = instance->button;
        instance->generic.cnt = instance->count;
    }

    return ret;
}

void subghz_protocol_decoder_mazda_v0_get_string(void* context, FuriString* output) {
    SubGhzProtocolDecoderMazdaV0* instance = context;

    mazda_v0_decode_key(&instance->generic);

    const uint8_t raw_crc = instance->generic.data & 0xFF;
    const uint8_t calc_crc = mazda_v0_calculate_checksum(
        instance->generic.serial, instance->generic.btn, instance->generic.cnt);

    furi_string_cat_printf(
        output,
        "%s %dbit CRC:%s\r\n"
        "Key: %016llX\r\n"
        "Sn: %08lX  Btn: %02X - %s\r\n"
        "Cnt: %05lX  Chk: %02X\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        (raw_crc == calc_crc) ? "OK" : "BAD",
        (unsigned long long)instance->generic.data,
        (unsigned long)instance->generic.serial,
        instance->generic.btn,
        mazda_v0_get_button_name(instance->generic.btn),
        (unsigned long)(instance->generic.cnt & 0xFFFFFU),
        raw_crc);
}
