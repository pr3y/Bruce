#include "chrysler_v0.h"
#include "protocols_common.h"

#include <string.h>

#define TAG "ChryslerV0"

#define CHRYSLER_V0_TE_SHORT         0x12C
#define CHRYSLER_V0_TE_DELTA         0x96
#define CHRYSLER_V0_TE_LONG_A        0xD48
#define CHRYSLER_V0_TE_LONG_B        0xE74
#define CHRYSLER_V0_TE_LONG_DELTA    0x190
#define CHRYSLER_V0_TE_GAP           0x1F40
#define CHRYSLER_V0_TE_ONE_SHORT     0x258
#define CHRYSLER_V0_FRAME_GAP        0x3CF0
#define CHRYSLER_V0_PREAMBLE_PAIRS   24U
#define CHRYSLER_V0_DECODE_BIT_COUNT 0x50

#define CHRYSLER_V0_UPLOAD_CAPACITY 0x200U
_Static_assert(
    CHRYSLER_V0_UPLOAD_CAPACITY <= PP_SHARED_UPLOAD_CAPACITY,
    "CHRYSLER_V0_UPLOAD_CAPACITY exceeds shared upload slab");

static const uint8_t chrysler_v0_xor_table[16] = {
    0x0F,
    0x02,
    0x40,
    0x0C,
    0x30,
    0x0E,
    0x70,
    0x08,
    0x10,
    0x0A,
    0x50,
    0xF4,
    0x2F,
    0xF6,
    0x6F,
    0xF0,
};

typedef enum {
    Chrysler_V0DecoderStepReset = 0,
    Chrysler_V0DecoderStepSeek = 1,
    Chrysler_V0DecoderStepData = 2,
} Chrysler_V0DecoderStep;

struct SubGhzProtocolDecoderChrysler_V0 {
    SubGhzProtocolDecoderBase base;
    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint16_t packet_bit_count;
    uint8_t decoded_button;

    uint32_t te_last;
    uint8_t plain_a[9];
    uint8_t plain_b[9];

    uint8_t plain_a_present;
    uint8_t plain_b_present;

    uint8_t check_ok;
    uint32_t sn_b;

    uint16_t data_2;
    uint8_t seed;
};

struct SubGhzProtocolEncoderChrysler_V0 {
    SubGhzProtocolEncoderBase base;
    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;

    uint8_t tx_button;
    uint8_t plain_header;

    uint8_t plain_a[9];
    uint8_t plain_b[9];

    uint16_t data_2;
    uint8_t seed;
};

static uint8_t chrysler_v0_reverse6(uint32_t value) {
    uint8_t out = 0;
    uint8_t bits = 6;

    while(bits--) {
        out = (uint8_t)((out << 1U) | (value & 1U));
        value >>= 1U;
    }

    return out;
}

static void
    chrysler_v0_transform_block(const uint8_t in[9], uint8_t out[9], uint32_t key, uint8_t button) {
    uint8_t mask = chrysler_v0_xor_table[key & 0x0FU];
    if(button == 1U) {
        mask ^= (key & 1U) ? 0xF0U : 0x0FU;
    }

    for(size_t i = 0; i < 9; i++) {
        out[i] = in[i] ^ mask;
    }
}

static bool chrysler_v0_is_short(uint32_t duration) {
    return DURATION_DIFF(duration, CHRYSLER_V0_TE_SHORT) <= CHRYSLER_V0_TE_DELTA;
}

static bool chrysler_v0_is_long_mark(uint32_t duration) {
    return (DURATION_DIFF(duration, CHRYSLER_V0_TE_LONG_A) <= CHRYSLER_V0_TE_LONG_DELTA) ||
           (DURATION_DIFF(duration, CHRYSLER_V0_TE_LONG_B) <= CHRYSLER_V0_TE_LONG_DELTA);
}

static const char* chrysler_v0_get_button_name(uint8_t button) {
    switch(button) {
    case 1:
        return "Lock";
    case 2:
        return "Unlock";
    default:
        return "??";
    }
}

static uint32_t chrysler_v0_get_sn_b(const SubGhzProtocolDecoderChrysler_V0* instance) {
    return instance->sn_b;
}

static void chrysler_v0_set_sn_b(SubGhzProtocolDecoderChrysler_V0* instance, uint32_t sn_b) {
    instance->sn_b = sn_b;
}

static void chrysler_v0_decode_packet(SubGhzProtocolDecoderChrysler_V0* instance) {
    uint8_t key[8];
    uint8_t encoded[9];
    uint8_t decoded[9];
    const uint16_t key2 = instance->data_2;

    pp_u64_to_bytes_be(instance->generic.data, key);
    instance->seed = chrysler_v0_reverse6(key[0] >> 2U);

    const uint8_t b1_xor_b6 = key[6] ^ key[1];
    const bool msb_set = (key[0] & 0x80U) != 0U;

    if(msb_set) {
        const uint8_t key2_low = (uint8_t)(key2 & 0xFFU);
        instance->check_ok = (key[1] == key[5]) && (b1_xor_b6 == 0x62U);
        instance->decoded_button = (((uint8_t)(key2_low ^ key[4])) == 0x10U) ? 2U : 1U;
    } else {
        instance->check_ok = 0U;
        instance->decoded_button = 1U;

        if(((uint8_t)(key[1] ^ 0xC3U)) == key[5]) {
            if(b1_xor_b6 == 0x04U) {
                instance->check_ok = 1U;
            } else {
                instance->check_ok = (b1_xor_b6 == 0x08U);
                if(b1_xor_b6 == 0x08U) {
                    instance->decoded_button = 2U;
                } else {
                    FURI_LOG_D(TAG, "BtnDetect: unknown b1^b6=%02X (MSB=0)", b1_xor_b6);
                }
            }
        } else {
            if(b1_xor_b6 == 0x08U) {
                instance->decoded_button = 2U;
            } else if(b1_xor_b6 != 0x04U) {
                FURI_LOG_D(TAG, "BtnDetect: unknown b1^b6=%02X (MSB=0)", b1_xor_b6);
            }
        }
    }

    encoded[0] = key[1];
    encoded[1] = key[2];
    encoded[2] = key[3];
    encoded[3] = key[4];
    encoded[4] = key[5];
    encoded[5] = key[6];
    encoded[6] = key[7];
    encoded[7] = (uint8_t)(key2 >> 8U);
    encoded[8] = (uint8_t)(key2 & 0xFFU);
    chrysler_v0_transform_block(encoded, decoded, instance->seed, instance->decoded_button);

    if(instance->seed & 1U) {
        memcpy(instance->plain_b, decoded, sizeof(instance->plain_b));
        instance->plain_b_present = 1U;

        const uint32_t sn_b = ((uint32_t)decoded[0] << 24U) | ((uint32_t)decoded[1] << 16U) |
                              ((uint32_t)decoded[2] << 8U) | (uint32_t)decoded[7];
        chrysler_v0_set_sn_b(instance, sn_b);
    } else {
        memcpy(instance->plain_a, decoded, sizeof(instance->plain_a));
        instance->plain_a_present = 1U;

        instance->generic.cnt = ((uint32_t)decoded[0] << 24U) | ((uint32_t)decoded[1] << 16U) |
                                ((uint32_t)decoded[2] << 8U) | (uint32_t)decoded[3];
    }

    instance->generic.btn = instance->decoded_button;
}

static void chrysler_v0_decoder_commit(SubGhzProtocolDecoderChrysler_V0* instance) {
    instance->packet_bit_count = CHRYSLER_V0_DECODE_BIT_COUNT;
    instance->decoder.decode_count_bit = CHRYSLER_V0_DECODE_BIT_COUNT;
    instance->generic.data_count_bit = CHRYSLER_V0_DECODE_BIT_COUNT;
    chrysler_v0_decode_packet(instance);

    if(instance->check_ok && instance->base.callback) {
        instance->base.callback(&instance->base, instance->base.context);
    }
}

#if PROTOPIRATE_WITH_ENCODER

static uint8_t chrysler_v0_payload_get_bit(const uint8_t payload[10], uint8_t index) {
    const uint8_t byte = payload[index >> 3U];
    const uint8_t shift = 7U - (index & 7U);
    return (byte >> shift) & 1U;
}

static void chrysler_v0_build_payload(
    const uint8_t plain[9],
    uint8_t counter,
    uint8_t button,
    uint8_t header_low2,
    uint8_t out[10]) {
    uint8_t transformed[9];
    chrysler_v0_transform_block(plain, transformed, counter, button);

    out[0] = (uint8_t)((chrysler_v0_reverse6(counter) << 2U) | (header_low2 & 0x03U));
    memcpy(&out[1], transformed, sizeof(transformed));
}

static size_t chrysler_v0_build_upload(
    SubGhzProtocolEncoderChrysler_V0* instance,
    const uint8_t payload_a[10],
    const uint8_t payload_b[10]) {
    size_t i = 0;
    LevelDuration* upload = instance->encoder.upload;
    const size_t cap = CHRYSLER_V0_UPLOAD_CAPACITY;

    for(size_t preamble = 0; preamble < CHRYSLER_V0_PREAMBLE_PAIRS; preamble++) {
        i = pp_emit(upload, i, cap, true, CHRYSLER_V0_TE_SHORT);
        i = pp_emit(upload, i, cap, false, CHRYSLER_V0_TE_LONG_B);
    }

    i = pp_emit(upload, i, cap, true, CHRYSLER_V0_TE_SHORT);
    i = pp_emit(upload, i, cap, false, CHRYSLER_V0_FRAME_GAP);

    for(uint8_t bit = 0; bit < 80; bit++) {
        const bool value = chrysler_v0_payload_get_bit(payload_a, bit);
        i = pp_emit(upload, i, cap, true, value ? CHRYSLER_V0_TE_ONE_SHORT : CHRYSLER_V0_TE_SHORT);
        i = pp_emit(upload, i, cap, false, value ? CHRYSLER_V0_TE_LONG_A : CHRYSLER_V0_TE_LONG_B);
    }

    i = pp_emit(upload, i, cap, true, CHRYSLER_V0_TE_SHORT);
    i = pp_emit(upload, i, cap, false, CHRYSLER_V0_FRAME_GAP);

    for(size_t preamble = 0; preamble < CHRYSLER_V0_PREAMBLE_PAIRS; preamble++) {
        i = pp_emit(upload, i, cap, true, CHRYSLER_V0_TE_SHORT);
        i = pp_emit(upload, i, cap, false, CHRYSLER_V0_TE_LONG_B);
    }

    i = pp_emit(upload, i, cap, true, CHRYSLER_V0_TE_SHORT);
    i = pp_emit(upload, i, cap, false, CHRYSLER_V0_FRAME_GAP);

    for(uint8_t bit = 0; bit < 80; bit++) {
        const bool value = chrysler_v0_payload_get_bit(payload_b, bit);
        i = pp_emit(upload, i, cap, true, value ? CHRYSLER_V0_TE_ONE_SHORT : CHRYSLER_V0_TE_SHORT);
        i = pp_emit(upload, i, cap, false, value ? CHRYSLER_V0_TE_LONG_A : CHRYSLER_V0_TE_LONG_B);
    }

    i = pp_emit(upload, i, cap, true, CHRYSLER_V0_TE_SHORT);
    i = pp_emit(upload, i, cap, false, CHRYSLER_V0_FRAME_GAP);

    instance->encoder.size_upload = i;
    return i;
}

#endif

const SubGhzProtocolDecoder subghz_protocol_chrysler_v0_decoder = {
    .alloc = subghz_protocol_decoder_chrysler_v0_alloc,
    .free = pp_decoder_free_default,
    .feed = subghz_protocol_decoder_chrysler_v0_feed,
    .reset = subghz_protocol_decoder_chrysler_v0_reset,
    .get_hash_data = pp_decoder_hash_blocks,
    .serialize = subghz_protocol_decoder_chrysler_v0_serialize,
    .deserialize = subghz_protocol_decoder_chrysler_v0_deserialize,
    .get_string = subghz_protocol_decoder_chrysler_v0_get_string,
};

#if PROTOPIRATE_WITH_ENCODER
const SubGhzProtocolEncoder subghz_protocol_chrysler_v0_encoder = {
    .alloc = subghz_protocol_encoder_chrysler_v0_alloc,
    .free = pp_encoder_free,
    .deserialize = subghz_protocol_encoder_chrysler_v0_deserialize,
    .stop = pp_encoder_stop,
    .yield = pp_encoder_yield,
};
#else
const SubGhzProtocolEncoder subghz_protocol_chrysler_v0_encoder = {
    .alloc = NULL,
    .free = NULL,
    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};
#endif

const SubGhzProtocol chrysler_protocol_v0 = {
    .name = CHRYSLER_PROTOCOL_V0_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM |
            SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Load
#if PROTOPIRATE_WITH_ENCODER
            | SubGhzProtocolFlag_Send
#endif
    ,
#if PROTOPIRATE_WITH_DECODER
    .decoder = &subghz_protocol_chrysler_v0_decoder,
#else
    .decoder = NULL,
#endif
#if PROTOPIRATE_WITH_ENCODER
    .encoder = &subghz_protocol_chrysler_v0_encoder,
#else
    .encoder = NULL,
#endif
};

#if PROTOPIRATE_WITH_ENCODER

void* subghz_protocol_encoder_chrysler_v0_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);

    SubGhzProtocolEncoderChrysler_V0* instance =
        calloc(1, sizeof(SubGhzProtocolEncoderChrysler_V0));

    instance->base.protocol = &chrysler_protocol_v0;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->encoder.repeat = 2;
    instance->encoder.size_upload = 0;
    instance->encoder.upload = NULL;
    instance->encoder.is_running = false;

    return instance;
}

SubGhzProtocolStatus
    subghz_protocol_encoder_chrysler_v0_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolEncoderChrysler_V0* instance = context;
    if(pp_verify_protocol_name(flipper_format, instance->base.protocol->name) !=
       SubGhzProtocolStatusOk) {
        return SubGhzProtocolStatusError;
    }

    SubGhzProtocolStatus status = subghz_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, CHRYSLER_V0_DECODE_BIT_COUNT);
    if(status != SubGhzProtocolStatusOk) {
        return status;
    }

    if(!flipper_format_rewind(flipper_format)) {
        return SubGhzProtocolStatusError;
    }

    uint16_t key2 = 0U;
    if(!flipper_format_read_hex(flipper_format, "Key_2", (uint8_t*)&key2, 2)) {
        return SubGhzProtocolStatusError;
    }

    key2 = __builtin_bswap16(key2);
    instance->data_2 = key2;

    uint8_t key[8];
    pp_u64_to_bytes_be(instance->generic.data, key);
    const uint8_t b0 = key[0];

    instance->seed = chrysler_v0_reverse6(((uint32_t)(instance->generic.data >> 56U)) >> 2U);
    instance->plain_header = (uint8_t)((instance->generic.data >> 56U) & 0x03U);

    if((b0 & 0x80U) == 0U) {
        instance->tx_button = (((uint8_t)(key[1] ^ key[6])) == 0x08U) ? 2U : 1U;
    } else {
        instance->tx_button = (((uint8_t)(key2 & 0xFFU) ^ key[4]) == 0x10U) ? 2U : 1U;
    }

    const uint8_t original_button = instance->tx_button;

    uint8_t encoded[9];
    uint8_t generated[9];
    encoded[0] = key[1];
    encoded[1] = key[2];
    encoded[2] = key[3];
    encoded[3] = key[4];
    encoded[4] = key[5];
    encoded[5] = key[6];
    encoded[6] = key[7];
    encoded[7] = (uint8_t)(key2 >> 8U);
    encoded[8] = (uint8_t)(key2 & 0xFFU);
    chrysler_v0_transform_block(encoded, generated, instance->seed, instance->tx_button);

    if(flipper_format_rewind(flipper_format) &&
       flipper_format_read_hex(flipper_format, "Plain_A", instance->plain_a, 9)) {
        if(!(flipper_format_rewind(flipper_format) &&
             flipper_format_read_hex(flipper_format, "Plain_B", instance->plain_b, 9))) {
            memcpy(instance->plain_b, instance->plain_a, sizeof(instance->plain_b));
        }
    } else if(
        flipper_format_rewind(flipper_format) &&
        flipper_format_read_hex(flipper_format, "Plain_B", instance->plain_b, 9)) {
        memcpy(instance->plain_a, instance->plain_b, sizeof(instance->plain_a));
    } else {
        memcpy(instance->plain_a, generated, sizeof(instance->plain_a));
        memcpy(instance->plain_b, generated, sizeof(instance->plain_b));
    }

    uint32_t btn_u32 = 0;
    uint32_t cnt_u32 = instance->seed & 0x3FU;
    pp_encoder_read_fields(flipper_format, NULL, &btn_u32, &cnt_u32, NULL);

    uint8_t tx_button = original_button;
    if(btn_u32 == 1U || btn_u32 == 2U) {
        tx_button = (uint8_t)btn_u32;
    }

    instance->tx_button = tx_button;
    if(tx_button != original_button) {
        instance->plain_a[5] ^= 0x0CU;
        instance->plain_b[3] ^= 0x30U;
    }

    instance->encoder.repeat = pp_encoder_read_repeat(flipper_format, 2);

    uint32_t counter = cnt_u32 & 0x3FU;

    uint8_t counter_a = (uint8_t)(counter & 0x3FU);
    if(counter_a & 1U) {
        counter_a = (uint8_t)((counter_a - 1U) & 0x3FU);
    }
    instance->seed = counter_a;
    const uint8_t counter_b = (counter_a == 0U) ? 0x3FU : (uint8_t)(counter_a - 1U);

    uint8_t payload_a[10];
    uint8_t payload_b[10];
    chrysler_v0_build_payload(
        instance->plain_a, counter_a, instance->tx_button, instance->plain_header, payload_a);
    chrysler_v0_build_payload(
        instance->plain_b, counter_b, instance->tx_button, instance->plain_header, payload_b);

    pp_encoder_buffer_ensure(instance, CHRYSLER_V0_UPLOAD_CAPACITY);
    chrysler_v0_build_upload(instance, payload_a, payload_b);

    instance->generic.data = pp_bytes_to_u64_be(payload_a);
    instance->data_2 = ((uint16_t)payload_a[8] << 8U) | payload_a[9];

    if(!flipper_format_rewind(flipper_format)) {
        return SubGhzProtocolStatusError;
    }
    if(!flipper_format_update_hex(flipper_format, FF_KEY, payload_a, 8)) {
        return SubGhzProtocolStatusError;
    }

    if(!flipper_format_rewind(flipper_format)) {
        return SubGhzProtocolStatusError;
    }

    uint16_t key2_out = __builtin_bswap16(instance->data_2);
    if(!flipper_format_update_hex(flipper_format, "Key_2", (uint8_t*)&key2_out, 2)) {
        return SubGhzProtocolStatusError;
    }

    instance->encoder.front = 0;
    instance->encoder.is_running = true;
    return status;
}

#endif

void* subghz_protocol_decoder_chrysler_v0_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);

    SubGhzProtocolDecoderChrysler_V0* instance =
        calloc(1, sizeof(SubGhzProtocolDecoderChrysler_V0));

    instance->base.protocol = &chrysler_protocol_v0;
    instance->generic.protocol_name = instance->base.protocol->name;

    return instance;
}

void subghz_protocol_decoder_chrysler_v0_reset(void* context) {
    SubGhzProtocolDecoderChrysler_V0* instance = context;
    instance->decoder.decode_data = 0;
    instance->data_2 = 0;
    instance->seed = 0;
    instance->decoder.parser_step = Chrysler_V0DecoderStepReset;
    instance->decoder.decode_count_bit = 0;
    instance->packet_bit_count = 0;
    instance->te_last = 0;
    instance->plain_a_present = 0;
    instance->plain_b_present = 0;
    instance->sn_b = 0;
}

void subghz_protocol_decoder_chrysler_v0_feed(void* context, bool level, uint32_t duration) {
    SubGhzProtocolDecoderChrysler_V0* instance = context;

    switch(instance->decoder.parser_step) {
    case Chrysler_V0DecoderStepReset:
        if(level && chrysler_v0_is_short(duration)) {
            instance->packet_bit_count = 0;
            instance->te_last = duration;
            instance->decoder.parser_step = Chrysler_V0DecoderStepSeek;
        }
        break;

    case Chrysler_V0DecoderStepSeek:
        if(level) {
            instance->te_last = duration;
            break;
        }

        if(chrysler_v0_is_long_mark(duration)) {
            if(chrysler_v0_is_short(instance->te_last)) {
                instance->packet_bit_count++;
            } else if(instance->packet_bit_count > 0x0F) {
                instance->data_2 = 0;
                instance->decoder.parser_step = Chrysler_V0DecoderStepData;
                instance->decoder.decode_data = 1;
                instance->decoder.decode_count_bit = 1;
            } else {
                instance->packet_bit_count = 0;
                instance->decoder.parser_step = Chrysler_V0DecoderStepSeek;
            }
            break;
        }

        if((duration > CHRYSLER_V0_TE_GAP) && (instance->packet_bit_count > 0x0F)) {
            instance->decoder.decode_data = 0;
            instance->data_2 = 0;
            instance->decoder.decode_count_bit = 0;
            instance->decoder.parser_step = Chrysler_V0DecoderStepData;
            break;
        }

        instance->decoder.parser_step = Chrysler_V0DecoderStepReset;
        instance->packet_bit_count = 0;
        break;

    case Chrysler_V0DecoderStepData: {
        if(level) {
            instance->te_last = duration;
            break;
        }

        const uint8_t count = instance->decoder.decode_count_bit;
        if(duration > CHRYSLER_V0_TE_GAP) {
            if(count > 0x4FU) {
                instance->generic.data = instance->decoder.decode_data;
                chrysler_v0_decoder_commit(instance);
            }

            instance->decoder.parser_step = Chrysler_V0DecoderStepReset;
            instance->packet_bit_count = 0;
            break;
        }

        uint8_t bit_value = 0;
        if(instance->te_last < CHRYSLER_V0_TE_SHORT) {
            if(!chrysler_v0_is_short(instance->te_last) || !chrysler_v0_is_long_mark(duration)) {
                if(count > 0x4FU) {
                    instance->generic.data = instance->decoder.decode_data;
                    chrysler_v0_decoder_commit(instance);
                }
                instance->decoder.parser_step = Chrysler_V0DecoderStepReset;
                instance->packet_bit_count = 0;
                break;
            }

            bit_value = 1U;
        } else {
            if(instance->te_last > 0x2EEU || !chrysler_v0_is_long_mark(duration)) {
                if(count > 0x4FU) {
                    instance->generic.data = instance->decoder.decode_data;
                    chrysler_v0_decoder_commit(instance);
                }
                instance->decoder.parser_step = Chrysler_V0DecoderStepReset;
                instance->packet_bit_count = 0;
                break;
            }

            bit_value = chrysler_v0_is_short(instance->te_last) ? 1U : 0U;
        }

        const uint8_t bit = bit_value ^ 1U;
        const uint8_t new_count = (uint8_t)(count + 1U);
        if(count <= 0x3FU) {
            instance->decoder.decode_data = (instance->decoder.decode_data << 1U) | bit;
            instance->decoder.decode_count_bit = new_count;
            break;
        }

        instance->data_2 = (uint16_t)((instance->data_2 << 1U) | bit);
        instance->decoder.decode_count_bit = new_count;
        if(new_count != CHRYSLER_V0_DECODE_BIT_COUNT) {
            break;
        }

        instance->generic.data = instance->decoder.decode_data;
        chrysler_v0_decoder_commit(instance);
        instance->decoder.decode_data = 0;
        instance->data_2 = 0;
        instance->decoder.decode_count_bit = 0;
        instance->decoder.parser_step = Chrysler_V0DecoderStepReset;
        instance->packet_bit_count = 0;
        break;
    }

    default:
        instance->decoder.parser_step = Chrysler_V0DecoderStepReset;
        break;
    }
}

SubGhzProtocolStatus subghz_protocol_decoder_chrysler_v0_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    SubGhzProtocolDecoderChrysler_V0* instance = context;
    SubGhzProtocolStatus status =
        subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
    if(status != SubGhzProtocolStatusOk) {
        return status;
    }

    if(!flipper_format_rewind(flipper_format)) {
        return SubGhzProtocolStatusErrorParserOthers;
    }

    const uint16_t key2 = __builtin_bswap16(instance->data_2);
    if(!flipper_format_write_hex(flipper_format, "Key_2", (const uint8_t*)&key2, 2)) {
        return SubGhzProtocolStatusErrorParserOthers;
    }

    if(instance->plain_a_present) {
        if(!flipper_format_write_hex(flipper_format, "Plain_A", instance->plain_a, 9)) {
            return SubGhzProtocolStatusErrorParserOthers;
        }
    }

    if(instance->plain_b_present) {
        if(!flipper_format_write_hex(flipper_format, "Plain_B", instance->plain_b, 9)) {
            return SubGhzProtocolStatusErrorParserOthers;
        }
    }

    if(!instance->plain_a_present && !instance->plain_b_present) {
        pp_write_display(
            flipper_format,
            instance->generic.protocol_name,
            chrysler_v0_get_button_name(instance->decoded_button));
    }

    const uint32_t serial_value = instance->plain_b_present ? chrysler_v0_get_sn_b(instance) :
                                                              instance->generic.cnt;
    pp_flipper_update_or_insert_u32(flipper_format, FF_SERIAL, serial_value);
    pp_flipper_update_or_insert_u32(flipper_format, FF_BTN, instance->decoded_button);
    pp_flipper_update_or_insert_u32(flipper_format, FF_CNT, instance->seed);

    return status;
}

SubGhzProtocolStatus
    subghz_protocol_decoder_chrysler_v0_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolDecoderChrysler_V0* instance = context;
    SubGhzProtocolStatus status =
        subghz_block_generic_deserialize(&instance->generic, flipper_format);
    if(status != SubGhzProtocolStatusOk) {
        return status;
    }

    if(!flipper_format_rewind(flipper_format)) {
        return SubGhzProtocolStatusError;
    }

    uint16_t key2 = 0U;
    if(!flipper_format_read_hex(flipper_format, "Key_2", (uint8_t*)&key2, 2)) {
        return SubGhzProtocolStatusError;
    }

    key2 = __builtin_bswap16(key2);
    instance->data_2 = key2;
    instance->packet_bit_count = CHRYSLER_V0_DECODE_BIT_COUNT;
    instance->decoder.decode_count_bit = CHRYSLER_V0_DECODE_BIT_COUNT;
    instance->generic.data_count_bit = CHRYSLER_V0_DECODE_BIT_COUNT;

    chrysler_v0_decode_packet(instance);

    if(flipper_format_rewind(flipper_format) &&
       flipper_format_read_hex(flipper_format, "Plain_A", instance->plain_a, 9)) {
        instance->plain_a_present = 1U;
        uint32_t sn_a = 0;
        memcpy(&sn_a, instance->plain_a, sizeof(sn_a));
        instance->generic.cnt = __builtin_bswap32(sn_a);
    }

    if(flipper_format_rewind(flipper_format) &&
       flipper_format_read_hex(flipper_format, "Plain_B", instance->plain_b, 9)) {
        instance->plain_b_present = 1U;
        const uint32_t sn_b =
            ((uint32_t)instance->plain_b[0] << 24U) | ((uint32_t)instance->plain_b[1] << 16U) |
            ((uint32_t)instance->plain_b[2] << 8U) | (uint32_t)instance->plain_b[7];
        chrysler_v0_set_sn_b(instance, sn_b);
    }

    instance->generic.protocol_name = instance->base.protocol->name;

    return status;
}

void subghz_protocol_decoder_chrysler_v0_get_string(void* context, FuriString* output) {
    SubGhzProtocolDecoderChrysler_V0* instance = context;

    furi_string_cat_printf(
        output,
        "%s %dbit\r\n%016llX%04X\r\n",
        instance->generic.protocol_name,
        instance->packet_bit_count,
        instance->generic.data,
        instance->data_2);

    if(instance->plain_a_present) {
        if(instance->plain_b_present) {
            furi_string_cat_printf(
                output,
                "SnA:%08lX\r\nSnB:%08lX\r\n",
                instance->generic.cnt,
                chrysler_v0_get_sn_b(instance));
        } else {
            furi_string_cat_printf(output, "SnA:%08lX\r\n", instance->generic.cnt);
        }
    } else if(instance->plain_b_present) {
        furi_string_cat_printf(output, "SnB:%08lX\r\n", chrysler_v0_get_sn_b(instance));
    }

    furi_string_cat_printf(
        output,
        "Btn:%02X [%s] Cnt:%02X\r\nChk:%s",
        instance->decoded_button,
        chrysler_v0_get_button_name(instance->decoded_button),
        instance->seed,
        instance->check_ok ? "OK" : "ERR");
}
