#include "kia_v2.h"
#include "../pp_platform.h"
#include <lib/toolbox/manchester_encoder.h>
#include <furi.h>

#define TAG "KiaV2"

#define KIA_V2_HEADER_PAIRS 252
#define KIA_V2_TOTAL_BURSTS 2
#define KIA_V2_UPLOAD_CAPACITY \
    (KIA_V2_TOTAL_BURSTS * ((KIA_V2_HEADER_PAIRS * 2) + 1 + (53U * 2)))
_Static_assert(
    KIA_V2_UPLOAD_CAPACITY <= PP_SHARED_UPLOAD_CAPACITY,
    "KIA_V2_UPLOAD_CAPACITY exceeds shared upload slab");

static const SubGhzBlockConst kia_protocol_v2_const = {
    .te_short = 500,
    .te_long = 1000,
    .te_delta = 150,
    .min_count_bit_for_found = 53,
};

struct SubGhzProtocolDecoderKiaV2 {
    SubGhzProtocolDecoderBase base;
    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
    uint16_t header_count;

    ManchesterState manchester_state;
};

struct SubGhzProtocolEncoderKiaV2 {
    SubGhzProtocolEncoderBase base;
    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;
};

typedef enum {
    KiaV2DecoderStepReset = 0,
    KiaV2DecoderStepCheckPreamble,
    KiaV2DecoderStepCollectRawBits,
} KiaV2DecoderStep;

const SubGhzProtocolDecoder kia_protocol_v2_decoder = {
    .alloc = kia_protocol_decoder_v2_alloc,
    .free = pp_decoder_free_default,
    .feed = kia_protocol_decoder_v2_feed,
    .reset = kia_protocol_decoder_v2_reset,
    .get_hash_data = pp_decoder_hash_blocks,
    .serialize = kia_protocol_decoder_v2_serialize,
    .deserialize = kia_protocol_decoder_v2_deserialize,
    .get_string = kia_protocol_decoder_v2_get_string,
};

#if PROTOPIRATE_WITH_ENCODER
const SubGhzProtocolEncoder kia_protocol_v2_encoder = {
    .alloc = kia_protocol_encoder_v2_alloc,
    .free = pp_encoder_free,
    .deserialize = kia_protocol_encoder_v2_deserialize,
    .stop = pp_encoder_stop,
    .yield = pp_encoder_yield,
};
#else
const SubGhzProtocolEncoder kia_protocol_v2_encoder = {
    .alloc = NULL,
    .free = NULL,
    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};
#endif

const SubGhzProtocol kia_protocol_v2 = {
    .name = KIA_PROTOCOL_V2_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM |
            SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save |
            SubGhzProtocolFlag_Send,
#if PROTOPIRATE_WITH_DECODER
    .decoder = &kia_protocol_v2_decoder,
#else
    .decoder = NULL,
#endif
#if PROTOPIRATE_WITH_ENCODER
    .encoder = &kia_protocol_v2_encoder,
#else
    .encoder = NULL,
#endif
};

static uint8_t kia_v2_calculate_crc(uint64_t data) {
    // Remove the CRC nibble (last 4 bits) to get the actual data
    uint64_t data_without_crc = data >> 4;

    // Extract 6 bytes from the data
    uint8_t bytes[6];
    bytes[0] = (uint8_t)(data_without_crc);
    bytes[1] = (uint8_t)(data_without_crc >> 8);
    bytes[2] = (uint8_t)(data_without_crc >> 16);
    bytes[3] = (uint8_t)(data_without_crc >> 24);
    bytes[4] = (uint8_t)(data_without_crc >> 32);
    bytes[5] = (uint8_t)(data_without_crc >> 40);

    uint8_t crc = 0;
    for(int i = 0; i < 6; i++) {
        crc ^= (bytes[i] & 0x0F) ^ (bytes[i] >> 4);
    }

    return (crc + 1) & 0x0F;
}
#if PROTOPIRATE_WITH_ENCODER

static void kia_protocol_encoder_v2_get_upload(SubGhzProtocolEncoderKiaV2* instance) {
    if(instance->encoder.upload == NULL) return;
    size_t index = 0;
    LevelDuration* up = instance->encoder.upload;
    const size_t cap = KIA_V2_UPLOAD_CAPACITY;

    const uint32_t te_short = (uint32_t)kia_protocol_v2_const.te_short;
    const uint32_t te_long = (uint32_t)kia_protocol_v2_const.te_long;

    uint8_t crc = kia_v2_calculate_crc(instance->generic.data);
    instance->generic.data = (instance->generic.data & ~0x0FULL) | crc;

    for(uint8_t burst = 0; burst < KIA_V2_TOTAL_BURSTS; burst++) {
        for(int i = 0; i < KIA_V2_HEADER_PAIRS; i++) {
            index = pp_emit(up, index, cap, false, te_long);
            index = pp_emit(up, index, cap, true, te_long);
        }

        index = pp_emit(up, index, cap, false, te_short);

        // Include the leading Manchester 1 before the 52 payload bits. Its
        // short HIGH is the decoder's preamble-to-data boundary.
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
        KIA_V2_TOTAL_BURSTS,
        instance->encoder.size_upload,
        instance->generic.data_count_bit,
        instance->generic.data);
}

#endif
#if PROTOPIRATE_WITH_ENCODER

void* kia_protocol_encoder_v2_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolEncoderKiaV2* instance = calloc(1, sizeof(SubGhzProtocolEncoderKiaV2));
    if(!instance) {
        return NULL;
    }

    instance->base.protocol = &kia_protocol_v2;
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

SubGhzProtocolStatus
    kia_protocol_encoder_v2_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolEncoderKiaV2* instance = context;
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

        instance->generic.data_count_bit = kia_protocol_v2_const.min_count_bit_for_found;

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
        if(btn > 0xFU || cnt > 0xFFFU) break;
        instance->generic.serial = serial;
        instance->generic.btn = (uint8_t)btn;
        instance->generic.cnt = (uint16_t)cnt;

        uint64_t new_data = 0;

        new_data |= 1ULL << 52;

        new_data |= ((uint64_t)instance->generic.serial << 20) & 0xFFFFFFFFF00000ULL;

        uint32_t uVar6 = ((uint32_t)(instance->generic.cnt & 0xFF) << 8) |
                         ((uint32_t)(instance->generic.btn & 0x0F) << 16) |
                         ((uint32_t)(instance->generic.cnt >> 4) & 0xF0);

        new_data |= (uint64_t)uVar6;

        instance->generic.data = new_data;
        instance->generic.data_count_bit = 53;

        FURI_LOG_I(
            TAG,
            "Encoder reconstruct: serial=0x%08lX, btn=0x%X, cnt=0x%03lX, uVar6=0x%05lX, data=0x%016llX",
            (unsigned long)instance->generic.serial,
            (unsigned int)instance->generic.btn,
            (unsigned long)instance->generic.cnt,
            (unsigned long)uVar6,
            (unsigned long long)instance->generic.data);

        instance->encoder.repeat = (int32_t)pp_encoder_read_repeat(flipper_format, 10);

        pp_encoder_buffer_ensure(instance, KIA_V2_UPLOAD_CAPACITY);
        kia_protocol_encoder_v2_get_upload(instance);

        uint8_t updated_key[8];
        pp_u64_to_bytes_be(instance->generic.data, updated_key);
        if(!flipper_format_insert_or_update_hex(flipper_format, FF_KEY, updated_key, 8)) break;
        pp_flipper_update_or_insert_u32(flipper_format, "CRC", instance->generic.data & 0x0F);
        pp_flipper_update_or_insert_u32(flipper_format, "RawCnt", (instance->generic.data >> 4) & 0xFFF);

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

void* kia_protocol_decoder_v2_alloc(SubGhzEnvironment* environment) {
    UNUSED(environment);
    SubGhzProtocolDecoderKiaV2* instance = calloc(1, sizeof(SubGhzProtocolDecoderKiaV2));
    if(!instance) {
        return NULL;
    }
    instance->base.protocol = &kia_protocol_v2;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void kia_protocol_decoder_v2_reset(void* context) {
    SubGhzProtocolDecoderKiaV2* instance = context;
    instance->decoder.parser_step = KiaV2DecoderStepReset;
    instance->header_count = 0;
    instance->manchester_state = ManchesterStateMid1;
    instance->decoder.decode_data = 0;
    instance->decoder.decode_count_bit = 0;
}

void kia_protocol_decoder_v2_feed(void* context, bool level, uint32_t duration) {
    SubGhzProtocolDecoderKiaV2* instance = context;

    switch(instance->decoder.parser_step) {
    case KiaV2DecoderStepReset:
        if((level) && (DURATION_DIFF(duration, kia_protocol_v2_const.te_long) <
                       kia_protocol_v2_const.te_delta)) {
            instance->decoder.parser_step = KiaV2DecoderStepCheckPreamble;
            instance->decoder.te_last = duration;
            instance->header_count = 0;
            manchester_advance(
                instance->manchester_state,
                ManchesterEventReset,
                &instance->manchester_state,
                NULL);
        }
        break;

    case KiaV2DecoderStepCheckPreamble:
        if(level) // HIGH pulse
        {
            if(DURATION_DIFF(duration, kia_protocol_v2_const.te_long) <
               kia_protocol_v2_const.te_delta) {
                instance->decoder.te_last = duration;
                instance->header_count++;
            } else if(
                DURATION_DIFF(duration, kia_protocol_v2_const.te_short) <
                kia_protocol_v2_const.te_delta) {
                if(instance->header_count >= 100) {
                    instance->header_count = 0;
                    instance->decoder.decode_data = 0;
                    // add_bit already counts the seeded leading bit.
                    instance->decoder.decode_count_bit = 0;
                    instance->decoder.parser_step = KiaV2DecoderStepCollectRawBits;
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                } else {
                    instance->decoder.te_last = duration;
                }
            } else {
                instance->decoder.parser_step = KiaV2DecoderStepReset;
            }
        } else {
            if(DURATION_DIFF(duration, kia_protocol_v2_const.te_long) <
               kia_protocol_v2_const.te_delta) {
                instance->header_count++;
                instance->decoder.te_last = duration;
            } else if(
                DURATION_DIFF(duration, kia_protocol_v2_const.te_short) <
                kia_protocol_v2_const.te_delta) {
                instance->decoder.te_last = duration;
            } else {
                instance->decoder.parser_step = KiaV2DecoderStepReset;
            }
        }
        break;

    case KiaV2DecoderStepCollectRawBits: {
        ManchesterEvent event = pp_manchester_event(duration, level, &kia_protocol_v2_const);
        if(event == ManchesterEventReset) {
            instance->decoder.parser_step = KiaV2DecoderStepReset;
            break;
        }

        bool data_bit;
        if(manchester_advance(
               instance->manchester_state, event, &instance->manchester_state, &data_bit)) {
            instance->decoder.decode_data = (instance->decoder.decode_data << 1) | data_bit;
            instance->decoder.decode_count_bit++;

            if(instance->decoder.decode_count_bit == 53) {
                instance->generic.data = instance->decoder.decode_data;
                instance->generic.data_count_bit = instance->decoder.decode_count_bit;

                instance->generic.serial = (uint32_t)((instance->generic.data >> 20) & 0xFFFFFFFF);
                instance->generic.btn = (uint8_t)((instance->generic.data >> 16) & 0x0F);

                uint16_t raw_count = (uint16_t)((instance->generic.data >> 4) & 0xFFF);
                instance->generic.cnt = ((raw_count >> 4) | (raw_count << 8)) & 0xFFF;

                if(instance->base.callback)
                    instance->base.callback(&instance->base, instance->base.context);

                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->header_count = 0;
                instance->decoder.parser_step = KiaV2DecoderStepReset;
            }
        }
        break;
    }
    }
}

SubGhzProtocolStatus kia_protocol_decoder_v2_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    SubGhzProtocolDecoderKiaV2* instance = context;

    SubGhzProtocolStatus ret =
        subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
    if(ret != SubGhzProtocolStatusOk) return ret;

    ret = pp_serialize_fields(
        flipper_format,
        PP_FIELD_SERIAL | PP_FIELD_BTN | PP_FIELD_CNT,
        instance->generic.serial,
        instance->generic.btn,
        instance->generic.cnt,
        0);
    if(ret != SubGhzProtocolStatusOk) return ret;

    uint32_t crc = instance->generic.data & 0x0F;
    if(!flipper_format_write_uint32(flipper_format, "CRC", &crc, 1)) {
        return SubGhzProtocolStatusErrorParserOthers;
    }
    uint32_t raw_count = (uint16_t)((instance->generic.data >> 4) & 0xFFF);
    if(!flipper_format_write_uint32(flipper_format, "RawCnt", &raw_count, 1)) {
        return SubGhzProtocolStatusErrorParserOthers;
    }
    return SubGhzProtocolStatusOk;
}

SubGhzProtocolStatus
    kia_protocol_decoder_v2_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolDecoderKiaV2* instance = context;
    SubGhzProtocolStatus status = subghz_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, kia_protocol_v2_const.min_count_bit_for_found);
    if(status == SubGhzProtocolStatusOk) {
        instance->generic.serial = (uint32_t)(instance->generic.data >> 20);
        instance->generic.btn = (uint8_t)((instance->generic.data >> 16) & 0x0F);
        uint16_t raw_count = (uint16_t)((instance->generic.data >> 4) & 0xFFF);
        instance->generic.cnt = ((raw_count >> 4) | (raw_count << 8)) & 0xFFF;
    }
    return status;
}

void kia_protocol_decoder_v2_get_string(void* context, FuriString* output) {
    SubGhzProtocolDecoderKiaV2* instance = context;

    uint8_t crc = instance->generic.data & 0x0F;

    bool crc_valid = crc == kia_v2_calculate_crc(instance->generic.data);

    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:%013llX\r\n"
        "Sn:%08lX Btn:%X\r\n"
        "Cnt:%03lX CRC:%X - %s\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        instance->generic.data,
        instance->generic.serial,
        instance->generic.btn,
        instance->generic.cnt,
        crc,
        crc_valid ? "OK" : "BAD");
}
