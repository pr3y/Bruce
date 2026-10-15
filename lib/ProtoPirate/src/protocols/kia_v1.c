#include "kia_v1.h"
#include "protocols_common.h"
#include "../pp_platform.h"
#include <lib/toolbox/manchester_decoder.h>

#define TAG "KiaV1"

#define KIA_V1_TOTAL_BURSTS       3
#define KIA_V1_INTER_BURST_GAP_US 25000
#define KIA_V1_HEADER_PULSES      90
#define KIA_V1_UPLOAD_CAPACITY                                                     \
    ((KIA_V1_TOTAL_BURSTS * ((KIA_V1_HEADER_PULSES * 2) + 1 + (57U * 2))) + \
     (KIA_V1_TOTAL_BURSTS - 1))
_Static_assert(
    KIA_V1_UPLOAD_CAPACITY <= PP_SHARED_UPLOAD_CAPACITY,
    "KIA_V1_UPLOAD_CAPACITY exceeds shared upload slab");

static const SubGhzBlockConst kia_protocol_v1_const = {
    .te_short = 800,
    .te_long = 1600,
    .te_delta = 200,
    .min_count_bit_for_found = 57,
};

struct SubGhzProtocolDecoderKiaV1 {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint16_t header_count;
    ManchesterState manchester_saved_state;
    uint8_t crc;
    bool crc_check;
};

struct SubGhzProtocolEncoderKiaV1 {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    KiaV1DecoderStepReset = 0,
    KiaV1DecoderStepCheckPreamble,
    KiaV1DecoderStepDecodeData,
} KiaV1DecoderStep;

const SubGhzProtocolDecoder kia_protocol_v1_decoder = {
    .alloc = kia_protocol_decoder_v1_alloc,
    .free = pp_decoder_free_default,

    .feed = kia_protocol_decoder_v1_feed,
    .reset = kia_protocol_decoder_v1_reset,

    .get_hash_data = pp_decoder_hash_blocks,
    .serialize = kia_protocol_decoder_v1_serialize,
    .deserialize = kia_protocol_decoder_v1_deserialize,
    .get_string = kia_protocol_decoder_v1_get_string,
};

#if PROTOPIRATE_WITH_ENCODER
const SubGhzProtocolEncoder kia_protocol_v1_encoder = {
    .alloc = kia_protocol_encoder_v1_alloc,
    .free = pp_encoder_free,

    .deserialize = kia_protocol_encoder_v1_deserialize,
    .stop = pp_encoder_stop,
    .yield = pp_encoder_yield,
};
#else
const SubGhzProtocolEncoder kia_protocol_v1_encoder = {
    .alloc = NULL,
    .free = NULL,
    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};
#endif

const SubGhzProtocol kia_protocol_v1 = {
    .name = KIA_PROTOCOL_V1_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM |
            SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save |
            SubGhzProtocolFlag_Send,

#if PROTOPIRATE_WITH_DECODER

    .decoder = &kia_protocol_v1_decoder,

#else

    .decoder = NULL,

#endif
#if PROTOPIRATE_WITH_ENCODER
    .encoder = &kia_protocol_v1_encoder,
#else
    .encoder = NULL,
#endif
};

static void kia_v1_check_remote_controller(SubGhzProtocolDecoderKiaV1* instance);

static uint8_t kia_v1_crc4(const uint8_t* bytes, int count, uint8_t offset) {
    uint8_t crc = 0;

    for(int i = 0; i < count; i++) {
        uint8_t b = bytes[i];
        crc ^= ((b & 0x0F) ^ (b >> 4));
    }

    crc = (crc + offset) & 0x0F;
    return crc;
}

static void kia_v1_check_remote_controller(SubGhzProtocolDecoderKiaV1* instance) {
    instance->generic.serial = instance->generic.data >> 24;
    instance->generic.btn = (instance->generic.data >> 16) & 0xFF;
    instance->generic.cnt = ((instance->generic.data >> 4) & 0xF) << 8 |
                            ((instance->generic.data >> 8) & 0xFF);

    uint8_t cnt_high = (instance->generic.cnt >> 8) & 0xF;
    uint8_t char_data[7];
    char_data[0] = (instance->generic.serial >> 24) & 0xFF;
    char_data[1] = (instance->generic.serial >> 16) & 0xFF;
    char_data[2] = (instance->generic.serial >> 8) & 0xFF;
    char_data[3] = instance->generic.serial & 0xFF;
    char_data[4] = instance->generic.btn;
    char_data[5] = instance->generic.cnt & 0xFF;

    char_data[6] = cnt_high;
    uint8_t crc = kia_v1_crc4(char_data, 7, 1);

    instance->crc = crc;
    instance->crc_check = (crc == (instance->generic.data & 0xF));
}

static const char* kia_v1_get_button_name(uint8_t btn) {
    const char* name;
    switch(btn) {
    case 0x1:
        name = "Close";
        break;
    case 0x2:
        name = "Open";
        break;
    case 0x3:
        name = "Boot";
        break;
    default:
        name = "??";
        break;
    }
    return name;
}
#if PROTOPIRATE_WITH_ENCODER
void* kia_protocol_encoder_v1_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderKiaV1* instance = calloc(1, sizeof(SubGhzProtocolEncoderKiaV1));
    if(!instance) {
        return NULL;
    }

    instance->base.protocol = &kia_protocol_v1;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->encoder.repeat = 10;
    instance->encoder.size_upload = 0;
    instance->encoder.upload = NULL;
    instance->encoder.is_running = false;
    instance->encoder.front = 0;
    return instance;
}

#endif
#if PROTOPIRATE_WITH_ENCODER

static void kia_protocol_encoder_v1_get_upload(SubGhzProtocolEncoderKiaV1* instance) {
    if(instance->encoder.upload == NULL) return; // lazy buffer not yet allocated
    size_t index = 0;
    LevelDuration* up = instance->encoder.upload;
    const size_t cap = KIA_V1_UPLOAD_CAPACITY;

    uint8_t cnt_high = (instance->generic.cnt >> 8) & 0xF;
    uint8_t char_data[7];
    char_data[0] = (instance->generic.serial >> 24) & 0xFF;
    char_data[1] = (instance->generic.serial >> 16) & 0xFF;
    char_data[2] = (instance->generic.serial >> 8) & 0xFF;
    char_data[3] = instance->generic.serial & 0xFF;
    char_data[4] = instance->generic.btn;
    char_data[5] = instance->generic.cnt & 0xFF;

    char_data[6] = cnt_high;
    uint8_t crc = kia_v1_crc4(char_data, 7, 1);

    instance->generic.data = (1ULL << 56) | (uint64_t)instance->generic.serial << 24 |
                             instance->generic.btn << 16 | (instance->generic.cnt & 0xFF) << 8 |
                             ((instance->generic.cnt >> 8) & 0xF) << 4 | crc;

    const uint32_t te_short = (uint32_t)kia_protocol_v1_const.te_short;
    const uint32_t te_long = (uint32_t)kia_protocol_v1_const.te_long;

    for(uint8_t burst = 0; burst < KIA_V1_TOTAL_BURSTS; burst++) {
        if(burst > 0) {
            index = pp_emit(up, index, cap, false, KIA_V1_INTER_BURST_GAP_US);
        }

        for(int i = 0; i < KIA_V1_HEADER_PULSES; i++) {
            index = pp_emit(up, index, cap, false, te_long);
            index = pp_emit(up, index, cap, true, te_long);
        }

        index = pp_emit(up, index, cap, false, te_short);

        // Explicit leading Manchester 1. Without this, a zero MSB merges with
        // the short LOW delimiter and the receiver starts inside the payload.
        index = pp_emit(up, index, cap, true, te_short);
        index = pp_emit(up, index, cap, false, te_short);

        for(uint8_t i = instance->generic.data_count_bit; i > 1; i--) {
            bool bit = bit_read(instance->generic.data, i - 2);
            index = pp_emit(up, index, cap, bit, te_short);
            index = pp_emit(up, index, cap, !bit, te_short);
        }
    }

    instance->encoder.size_upload = index;
    instance->encoder.front = 0;

    FURI_LOG_I(
        TAG,
        "Upload built: %d bursts, size_upload=%zu, data_count_bit=%u, data=0x%016llX",
        KIA_V1_TOTAL_BURSTS,
        instance->encoder.size_upload,
        instance->generic.data_count_bit,
        instance->generic.data);
}

#endif
#if PROTOPIRATE_WITH_ENCODER

SubGhzProtocolStatus
    kia_protocol_encoder_v1_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolEncoderKiaV1* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;

    flipper_format_rewind(flipper_format);

    do {
        if(pp_verify_protocol_name(flipper_format, instance->base.protocol->name) !=
           SubGhzProtocolStatusOk) {
            FURI_LOG_E(TAG, "Missing or wrong Protocol");
            break;
        }

        uint32_t bits = 0;
        if(pp_encoder_read_bit(flipper_format, NULL, 0, &bits) != SubGhzProtocolStatusOk) break;

        instance->generic.data_count_bit = kia_protocol_v1_const.min_count_bit_for_found;

        uint64_t key = 0;
        if(!pp_flipper_read_hex_u64(flipper_format, FF_KEY, &key)) break;

        instance->generic.data = key;
        if(instance->generic.data == 0) break;

        uint32_t serial = UINT32_MAX;
        uint32_t btn = UINT32_MAX;
        uint32_t cnt = UINT32_MAX;
        // A full-width serial (0xFFFFFFFF) is data, not a missing-field sentinel.
        flipper_format_rewind(flipper_format);
        if(!flipper_format_read_uint32(flipper_format, FF_SERIAL, &serial, 1)) break;
        flipper_format_rewind(flipper_format);
        if(!flipper_format_read_uint32(flipper_format, FF_BTN, &btn, 1)) break;
        flipper_format_rewind(flipper_format);
        if(!flipper_format_read_uint32(flipper_format, FF_CNT, &cnt, 1)) break;
        if(btn > UINT8_MAX || cnt > 0xFFFU) break;
        instance->generic.serial = serial;
        instance->generic.btn = (uint8_t)btn;
        instance->generic.cnt = (uint16_t)cnt;

        instance->encoder.repeat = (int32_t)pp_encoder_read_repeat(flipper_format, 10);

        pp_encoder_buffer_ensure(instance, KIA_V1_UPLOAD_CAPACITY);
        kia_protocol_encoder_v1_get_upload(instance);

        uint8_t updated_key[8];
        pp_u64_to_bytes_be(instance->generic.data, updated_key);
        if(!flipper_format_insert_or_update_hex(flipper_format, FF_KEY, updated_key, 8)) break;

        instance->encoder.is_running = true;

        FURI_LOG_I(
            TAG,
            "Encoder deserialized: repeat=%u, size_upload=%zu, is_running=%d, front=%zu",
            instance->encoder.repeat,
            instance->encoder.size_upload,
            instance->encoder.is_running,
            instance->encoder.front);

        ret = SubGhzProtocolStatusOk;
    } while(false);

    return ret;
}

#endif
#if PROTOPIRATE_WITH_ENCODER

void kia_protocol_encoder_v1_set_button(void* context, uint8_t button) {
    SubGhzProtocolEncoderKiaV1* instance = context;
    instance->generic.btn = button & 0xFF;
    kia_protocol_encoder_v1_get_upload(instance);
    FURI_LOG_I(TAG, "Button set to 0x%02X, upload rebuilt with new CRC", instance->generic.btn);
}

#endif
#if PROTOPIRATE_WITH_ENCODER

void kia_protocol_encoder_v1_set_counter(void* context, uint16_t counter) {
    SubGhzProtocolEncoderKiaV1* instance = context;
    instance->generic.cnt = counter & 0xFFF;
    kia_protocol_encoder_v1_get_upload(instance);
    FURI_LOG_I(
        TAG,
        "Counter set to 0x%03X, upload rebuilt with new CRC",
        (uint16_t)instance->generic.cnt);
}

#endif
#if PROTOPIRATE_WITH_ENCODER

void kia_protocol_encoder_v1_increment_counter(void* context) {
    SubGhzProtocolEncoderKiaV1* instance = context;
    instance->generic.cnt = (instance->generic.cnt + 1) & 0xFFF;
    kia_protocol_encoder_v1_get_upload(instance);
    FURI_LOG_I(
        TAG,
        "Counter incremented to 0x%03X, upload rebuilt with new CRC",
        (uint16_t)instance->generic.cnt);
}

#endif
#if PROTOPIRATE_WITH_ENCODER

uint16_t kia_protocol_encoder_v1_get_counter(void* context) {
    SubGhzProtocolEncoderKiaV1* instance = context;
    return instance->generic.cnt;
}

#endif
#if PROTOPIRATE_WITH_ENCODER

uint8_t kia_protocol_encoder_v1_get_button(void* context) {
    SubGhzProtocolEncoderKiaV1* instance = context;
    return instance->generic.btn;
}

#endif

void* kia_protocol_decoder_v1_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderKiaV1* instance = calloc(1, sizeof(SubGhzProtocolDecoderKiaV1));
    if(!instance) {
        return NULL;
    }
    instance->base.protocol = &kia_protocol_v1;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void kia_protocol_decoder_v1_reset(void* context) {
    SubGhzProtocolDecoderKiaV1* instance = context;
    instance->decoder.parser_step = KiaV1DecoderStepReset;
}

void kia_protocol_decoder_v1_feed(void* context, bool level, uint32_t duration) {
    SubGhzProtocolDecoderKiaV1* instance = context;

    ManchesterEvent event = ManchesterEventReset;

    switch(instance->decoder.parser_step) {
    case KiaV1DecoderStepReset:
        if((level) && (DURATION_DIFF(duration, kia_protocol_v1_const.te_long) <
                       kia_protocol_v1_const.te_delta)) {
            instance->decoder.parser_step = KiaV1DecoderStepCheckPreamble;
            instance->decoder.te_last = duration;
            instance->header_count = 0;
            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
            manchester_advance(
                instance->manchester_saved_state,
                ManchesterEventReset,
                &instance->manchester_saved_state,
                NULL);
        }
        break;

    case KiaV1DecoderStepCheckPreamble:
        if(!level) {
            if((DURATION_DIFF(duration, kia_protocol_v1_const.te_long) <
                kia_protocol_v1_const.te_delta) &&
               (DURATION_DIFF(instance->decoder.te_last, kia_protocol_v1_const.te_long) <
                kia_protocol_v1_const.te_delta)) {
                instance->header_count++;
                instance->decoder.te_last = duration;
            } else {
                instance->decoder.parser_step = KiaV1DecoderStepReset;
            }
        }
        if(instance->header_count > 70) {
            if((!level) &&
               (DURATION_DIFF(duration, kia_protocol_v1_const.te_short) <
                kia_protocol_v1_const.te_delta) &&
               (DURATION_DIFF(instance->decoder.te_last, kia_protocol_v1_const.te_long) <
                kia_protocol_v1_const.te_delta)) {
                // add_bit increments the count; the seeded 1 is one bit.
                instance->decoder.decode_count_bit = 0;
                subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                instance->header_count = 0;
                instance->decoder.parser_step = KiaV1DecoderStepDecodeData;
            }
        }
        break;

    case KiaV1DecoderStepDecodeData:
        event = pp_manchester_event(duration, level, &kia_protocol_v1_const);

        if(event != ManchesterEventReset) {
            bool data;
            bool data_ok = manchester_advance(
                instance->manchester_saved_state, event, &instance->manchester_saved_state, &data);
            if(data_ok) {
                instance->decoder.decode_data = (instance->decoder.decode_data << 1) | data;
                instance->decoder.decode_count_bit++;
            }
        }

        if(instance->decoder.decode_count_bit == kia_protocol_v1_const.min_count_bit_for_found) {
            instance->generic.data = instance->decoder.decode_data;
            instance->generic.data_count_bit = instance->decoder.decode_count_bit;
            if(instance->base.callback)
                instance->base.callback(&instance->base, instance->base.context);

            instance->decoder.decode_data = 0;
            instance->decoder.decode_count_bit = 0;
            instance->decoder.parser_step = KiaV1DecoderStepReset;
        }
        break;
    }
}

SubGhzProtocolStatus kia_protocol_decoder_v1_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    SubGhzProtocolDecoderKiaV1* instance = context;

    kia_v1_check_remote_controller(instance);

    SubGhzProtocolStatus ret =
        subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
    if(ret != SubGhzProtocolStatusOk) return ret;

    return pp_serialize_fields(
        flipper_format,
        PP_FIELD_SERIAL | PP_FIELD_BTN | PP_FIELD_CNT,
        instance->generic.serial,
        instance->generic.btn,
        instance->generic.cnt,
        0);
}

SubGhzProtocolStatus
    kia_protocol_decoder_v1_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolDecoderKiaV1* instance = context;
    flipper_format_rewind(flipper_format);
    return subghz_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, kia_protocol_v1_const.min_count_bit_for_found);
}

void kia_protocol_decoder_v1_get_string(void* context, FuriString* output) {
    SubGhzProtocolDecoderKiaV1* instance = context;

    kia_v1_check_remote_controller(instance);
    uint32_t code_found_hi = instance->generic.data >> 32;
    uint32_t code_found_lo = instance->generic.data & 0xFFFFFFFF;

    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:%06lX%08lX\r\n"
        "Serial:%08lX\r\n"
        "Cnt:%03lX CRC:%01X %s\r\n"
        "Btn:%02X:%s\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        code_found_hi,
        code_found_lo,
        instance->generic.serial,
        instance->generic.cnt,
        instance->crc,
        instance->crc_check ? "OK" : "WRONG",
        instance->generic.btn,
        kia_v1_get_button_name(instance->generic.btn));
}
