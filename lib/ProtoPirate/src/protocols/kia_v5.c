#include "kia_v5.h"
#include "../pp_platform.h"
#include "protocols_common.h"
#include "keys.h"

#define TAG "KiaV5"

static const SubGhzBlockConst kia_protocol_v5_const = {
    .te_short = 400,
    .te_long = 800,
    .te_delta = 150,
    .min_count_bit_for_found = 64,
};

static void build_keystore_from_mfkey(uint8_t* result) {
    uint64_t mfkey = get_kia_v5_key();
    for(int i = 0; i < 8; i++) {
        result[i] = (uint8_t)((mfkey >> ((7 - i) * 8)) & 0xFFU);
    }
}

static uint8_t keystore_bytes[8] = {0};

static uint16_t mixer_decode(uint32_t encrypted) {
    uint8_t s0 = (encrypted & 0xFF);
    uint8_t s1 = (encrypted >> 8) & 0xFF;
    uint8_t s2 = (encrypted >> 16) & 0xFF;
    uint8_t s3 = (encrypted >> 24) & 0xFF;

    // Prepare key
    build_keystore_from_mfkey(keystore_bytes);

    int round_index = 1;
    for(size_t i = 0; i < 18; i++) {
        uint8_t r = keystore_bytes[round_index] & 0xFF;
        int steps = 8;
        while(steps > 0) {
            uint8_t base;
            if((s3 & 0x40) == 0) {
                base = (s3 & 0x02) == 0 ? 0x74 : 0x2E;
            } else {
                base = (s3 & 0x02) == 0 ? 0x3A : 0x5C;
            }

            if(s2 & 0x08) {
                base = (((base >> 4) & 0x0F) | ((base & 0x0F) << 4)) & 0xFF;
            }
            if(s1 & 0x01) {
                base = ((base & 0x3F) << 2) & 0xFF;
            }
            if(s0 & 0x01) {
                base = (base << 1) & 0xFF;
            }

            uint8_t temp = (s3 ^ s1) & 0xFF;
            s3 = ((s3 & 0x7F) << 1) & 0xFF;
            if(s2 & 0x80) {
                s3 |= 0x01;
            }
            s2 = ((s2 & 0x7F) << 1) & 0xFF;
            if(s1 & 0x80) {
                s2 |= 0x01;
            }
            s1 = ((s1 & 0x7F) << 1) & 0xFF;
            if(s0 & 0x80) {
                s1 |= 0x01;
            }
            s0 = ((s0 & 0x7F) << 1) & 0xFF;

            uint8_t chk = (base ^ (r ^ temp)) & 0xFF;
            if(chk & 0x80) {
                s0 |= 0x01;
            }
            r = ((r & 0x7F) << 1) & 0xFF;
            steps--;
        }
        round_index = (round_index - 1) & 0x7;
    }
    return (s0 + (s1 << 8)) & 0xFFFF;
}

#if PROTOPIRATE_WITH_ENCODER
static uint32_t mixer_encode(uint32_t serial, uint16_t counter, uint8_t button) {
    build_keystore_from_mfkey(keystore_bytes);

    uint8_t state_a = (uint8_t)(((serial >> 8) & 0x0FU) | ((button & 0x0FU) << 4));
    uint8_t state_b = (uint8_t)((counter >> 8) & 0xFFU);
    uint8_t state_c = (uint8_t)(serial & 0xFFU);
    uint8_t state_d = (uint8_t)(counter & 0xFFU);

    int ks_idx = 0;
    for(int round_i = 0; round_i < 18; round_i++) {
        uint8_t r = keystore_bytes[ks_idx] & 0xFFU;
        ks_idx = (ks_idx + 1) & 0x07;

        uint8_t running_d = state_d;
        for(int step = 0; step < 8; step++) {
            uint8_t base;
            if((state_a & 0x80U) == 0) {
                base = (state_a & 0x04U) == 0 ? 0x74U : 0x2EU;
            } else {
                base = (state_a & 0x04U) == 0 ? 0x3AU : 0x5CU;
            }

            if(state_c & 0x10U) {
                base = (uint8_t)(((base >> 4) & 0x0FU) | ((base & 0x0FU) << 4));
            }
            if(state_b & 0x02U) {
                base = (uint8_t)((base & 0x3FU) << 2);
            }

            uint8_t base_final = base;
            if(running_d & 0x02U) {
                base_final = (uint8_t)((base & 0x7FU) << 1);
            }

            const bool carry_b = (state_b & 0x01U) != 0;
            const bool carry_c = (state_c & 0x01U) != 0;
            const bool carry_a = (state_a & 0x01U) != 0;

            uint8_t new_d = (uint8_t)(running_d >> 1);
            if(carry_b) new_d |= 0x80U;

            running_d ^= state_c;

            state_b = (uint8_t)(state_b >> 1);
            if(carry_c) state_b |= 0x80U;

            state_c = (uint8_t)(state_c >> 1);
            if(carry_a) state_c |= 0x80U;

            const uint8_t feedback = (uint8_t)(((running_d ^ r) << 7) ^ base_final);
            state_a = (uint8_t)(state_a >> 1);
            if(feedback & 0x80U) state_a |= 0x80U;

            r = (uint8_t)(r >> 1);
            running_d = new_d;
        }
        state_d = running_d;
    }

    return ((uint32_t)state_a << 24) | ((uint32_t)state_c << 16) | ((uint32_t)state_b << 8) |
           (uint32_t)state_d;
}
#endif

struct SubGhzProtocolDecoderKiaV5 {
    SubGhzProtocolDecoderBase base;
    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;
    uint16_t header_count;

    ManchesterState manchester_state;
    uint64_t decoded_data;
    uint64_t saved_key;
    uint8_t bit_count;
    uint64_t yek;
    uint8_t crc;
};

struct SubGhzProtocolEncoderKiaV5 {
    SubGhzProtocolEncoderBase base;
    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;

    uint64_t replay_data;
    uint8_t replay_crc;
};

#define KIA_V5_PREAMBLE_PAIRS 200U
#define KIA_V5_SYNC_ENTRIES   4U
#define KIA_V5_DATA_BITS      64U
#define KIA_V5_CRC_BITS       3U
#define KIA_V5_END_ENTRIES    2U
#define KIA_V5_UPLOAD_CAPACITY                          \
    (KIA_V5_PREAMBLE_PAIRS * 2U + KIA_V5_SYNC_ENTRIES + \
     (KIA_V5_DATA_BITS + KIA_V5_CRC_BITS) * 2U + KIA_V5_END_ENTRIES)
_Static_assert(
    KIA_V5_UPLOAD_CAPACITY <= PP_SHARED_UPLOAD_CAPACITY,
    "KIA_V5_UPLOAD_CAPACITY exceeds shared upload slab");

typedef enum {
    KiaV5DecoderStepReset = 0,
    KiaV5DecoderStepCheckPreamble,
    KiaV5DecoderStepData,
} KiaV5DecoderStep;

const SubGhzProtocolDecoder kia_protocol_v5_decoder = {
    .alloc = kia_protocol_decoder_v5_alloc,
    .free = pp_decoder_free_default,
    .feed = kia_protocol_decoder_v5_feed,
    .reset = kia_protocol_decoder_v5_reset,
    .get_hash_data = pp_decoder_hash_blocks,
    .serialize = kia_protocol_decoder_v5_serialize,
    .deserialize = kia_protocol_decoder_v5_deserialize,
    .get_string = kia_protocol_decoder_v5_get_string,
};

#if PROTOPIRATE_WITH_ENCODER
const SubGhzProtocolEncoder kia_protocol_v5_encoder = {
    .alloc = kia_protocol_encoder_v5_alloc,
    .free = pp_encoder_free,
    .deserialize = kia_protocol_encoder_v5_deserialize,
    .stop = pp_encoder_stop,
    .yield = pp_encoder_yield,
};
#else
const SubGhzProtocolEncoder kia_protocol_v5_encoder = {
    .alloc = NULL,
    .free = NULL,
    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};
#endif

const SubGhzProtocol kia_protocol_v5 = {
    .name = KIA_PROTOCOL_V5_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_315 | SubGhzProtocolFlag_433 | SubGhzProtocolFlag_FM |
            SubGhzProtocolFlag_Decodable
#if PROTOPIRATE_WITH_ENCODER
            | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Send
#endif
    ,
#if PROTOPIRATE_WITH_DECODER
    .decoder = &kia_protocol_v5_decoder,
#else
    .decoder = NULL,
#endif
#if PROTOPIRATE_WITH_ENCODER
    .encoder = &kia_protocol_v5_encoder,
#else
    .encoder = NULL,
#endif
};

#if PROTOPIRATE_WITH_ENCODER

static uint8_t kia_v5_calculate_crc(uint64_t data) {
    uint8_t crc = 0;
    for(int i = 63; i >= 0; i--) {
        const uint8_t bit = (data >> i) & 1U;
        const uint8_t shifted_out = (crc >> 1U) & 1U;
        crc = (uint8_t)(((crc & 1U) << 1U) | bit);
        if(shifted_out) {
            crc ^= 3U;
        }
    }
    return (uint8_t)(crc & 3U);
}

void* kia_protocol_encoder_v5_alloc(SubGhzEnvironment* environment) {
    if(environment) {
        protopirate_keys_load(environment);
    }
    if(!protopirate_keys_has_kia_v5_key()) {
        FURI_LOG_E(TAG, "Kia V5 encoder missing KIA_KEY4 keystore entry");
        return NULL;
    }

    SubGhzProtocolEncoderKiaV5* instance = calloc(1, sizeof(SubGhzProtocolEncoderKiaV5));

    instance->base.protocol = &kia_protocol_v5;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->encoder.repeat = 6;
    instance->encoder.size_upload = 0;
    instance->encoder.upload = NULL;
    instance->encoder.is_running = false;
    return instance;
}

static size_t kia_v5_emit_manchester_bit(LevelDuration* up, size_t i, size_t cap, bool bit_value) {
    const uint32_t te = kia_protocol_v5_const.te_short;
    if(bit_value) {
        i = pp_emit(up, i, cap, false, te);
        i = pp_emit(up, i, cap, true, te);
    } else {
        i = pp_emit(up, i, cap, true, te);
        i = pp_emit(up, i, cap, false, te);
    }
    return i;
}

static void kia_protocol_encoder_v5_get_upload(SubGhzProtocolEncoderKiaV5* instance) {
    LevelDuration* upload = instance->encoder.upload;
    const size_t cap = KIA_V5_UPLOAD_CAPACITY;
    const uint32_t te_short = kia_protocol_v5_const.te_short;
    const uint32_t te_long = kia_protocol_v5_const.te_long;
    size_t i = 0;

    for(size_t p = 0; p < KIA_V5_PREAMBLE_PAIRS; p++) {
        i = pp_emit(upload, i, cap, true, te_short);
        i = pp_emit(upload, i, cap, false, te_short);
    }

    i = pp_emit(upload, i, cap, false, te_short);
    i = pp_emit(upload, i, cap, true, te_long);
    i = pp_emit(upload, i, cap, false, te_short);
    i = pp_emit(upload, i, cap, true, te_short);

    for(int b = (int)KIA_V5_DATA_BITS - 1; b >= 0; b--) {
        const bool bit_value = ((instance->replay_data >> b) & 1ULL) != 0ULL;
        i = kia_v5_emit_manchester_bit(upload, i, cap, bit_value);
    }

    i = kia_v5_emit_manchester_bit(upload, i, cap, false);
    i = kia_v5_emit_manchester_bit(upload, i, cap, ((instance->replay_crc >> 1U) & 1U) != 0U);
    i = kia_v5_emit_manchester_bit(upload, i, cap, (instance->replay_crc & 1U) != 0U);

    i = pp_emit(upload, i, cap, false, te_short);
    i = pp_emit(upload, i, cap, true, te_short);

    instance->encoder.size_upload = i;
    instance->encoder.front = 0;
}

SubGhzProtocolStatus
    kia_protocol_encoder_v5_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolEncoderKiaV5* instance = context;

    instance->encoder.is_running = false;
    instance->encoder.front = 0;

    if(pp_verify_protocol_name(flipper_format, instance->base.protocol->name) !=
       SubGhzProtocolStatusOk) {
        FURI_LOG_E(TAG, "V5 enc: protocol mismatch");
        return SubGhzProtocolStatusError;
    }

    SubGhzProtocolStatus status =
        subghz_block_generic_deserialize(&instance->generic, flipper_format);
    if(status != SubGhzProtocolStatusOk) {
        FURI_LOG_E(TAG, "V5 enc: generic deserialize failed (%d)", status);
        return status;
    }
    if(instance->generic.data_count_bit < kia_protocol_v5_const.min_count_bit_for_found) {
        FURI_LOG_E(
            TAG, "V5 enc: bit count too low: %u", (unsigned)instance->generic.data_count_bit);
        return SubGhzProtocolStatusErrorValueBitCount;
    }

    uint32_t serial_v = UINT32_MAX;
    uint32_t btn_v = UINT32_MAX;
    uint32_t cnt_v = UINT32_MAX;
    pp_encoder_read_fields(flipper_format, &serial_v, &btn_v, &cnt_v, NULL);

    uint64_t yek_captured = 0;
    for(int i = 0; i < 8; i++) {
        const uint8_t b = (uint8_t)((instance->generic.data >> (i * 8)) & 0xFFU);
        yek_captured |= ((uint64_t)pp_reverse_bits8(b) << ((7 - i) * 8));
    }
    if(serial_v == UINT32_MAX) {
        serial_v = (uint32_t)((yek_captured >> 32) & 0x0FFFFFFFU);
    }
    if(btn_v == UINT32_MAX) {
        btn_v = (uint32_t)((yek_captured >> 60) & 0x0FU);
    }
    if(cnt_v == UINT32_MAX) {
        cnt_v = mixer_decode((uint32_t)(yek_captured & 0xFFFFFFFFU));
    }

    serial_v &= 0x0FFFFFFFU;
    btn_v &= 0x0FU;
    cnt_v &= 0xFFFFU;

    const uint32_t mixer = mixer_encode(serial_v, (uint16_t)cnt_v, (uint8_t)btn_v);
    const uint64_t yek_new = ((uint64_t)btn_v << 60) | ((uint64_t)serial_v << 32) |
                             (uint64_t)mixer;

    uint64_t data_new = 0;
    for(int i = 0; i < 8; i++) {
        const uint8_t b = (uint8_t)((yek_new >> (i * 8)) & 0xFFU);
        data_new |= ((uint64_t)pp_reverse_bits8(b) << ((7 - i) * 8));
    }

    instance->replay_data = data_new;
    instance->generic.data = data_new;
    instance->generic.serial = serial_v;
    instance->generic.btn = (uint8_t)btn_v;
    instance->generic.cnt = (uint16_t)cnt_v;
    instance->replay_crc = kia_v5_calculate_crc(instance->replay_data);

    FURI_LOG_I(
        TAG,
        "V5 enc reencrypt: sn=%07lX btn=%X cnt=%04lX mixer=%08lX",
        (unsigned long)serial_v,
        (unsigned)btn_v,
        (unsigned long)cnt_v,
        (unsigned long)mixer);

    instance->encoder.repeat = (int32_t)pp_encoder_read_repeat(flipper_format, 6);

    pp_encoder_buffer_ensure(instance, KIA_V5_UPLOAD_CAPACITY);
    kia_protocol_encoder_v5_get_upload(instance);

    instance->encoder.is_running = true;

    FURI_LOG_I(
        TAG,
        "V5 enc ready: data=%08lX%08lX crc=%X repeat=%u size=%zu",
        (uint32_t)(instance->replay_data >> 32),
        (uint32_t)(instance->replay_data & 0xFFFFFFFFULL),
        instance->replay_crc,
        (unsigned)instance->encoder.repeat,
        instance->encoder.size_upload);
    FURI_LOG_I(
        TAG,
        "V5 enc preamble[0..3]: %s%lu %s%lu %s%lu %s%lu",
        level_duration_get_level(instance->encoder.upload[0]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[0]),
        level_duration_get_level(instance->encoder.upload[1]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[1]),
        level_duration_get_level(instance->encoder.upload[2]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[2]),
        level_duration_get_level(instance->encoder.upload[3]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[3]));
    FURI_LOG_I(
        TAG,
        "V5 enc sync[400..403]: %s%lu %s%lu %s%lu %s%lu",
        level_duration_get_level(instance->encoder.upload[400]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[400]),
        level_duration_get_level(instance->encoder.upload[401]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[401]),
        level_duration_get_level(instance->encoder.upload[402]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[402]),
        level_duration_get_level(instance->encoder.upload[403]) ? "H" : "L",
        (unsigned long)level_duration_get_duration(instance->encoder.upload[403]));

    return SubGhzProtocolStatusOk;
}

#endif

static void kia_v5_add_bit(SubGhzProtocolDecoderKiaV5* instance, bool bit) {
    instance->decoded_data = (instance->decoded_data << 1) | (bit ? 1 : 0);
    instance->bit_count++;
}

void* kia_protocol_decoder_v5_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolDecoderKiaV5* instance = malloc(sizeof(SubGhzProtocolDecoderKiaV5));

    if(environment) {
        protopirate_keys_load(environment);
    }

    instance->base.protocol = &kia_protocol_v5;
    instance->generic.protocol_name = instance->base.protocol->name;
    return instance;
}

void kia_protocol_decoder_v5_reset(void* context) {
    SubGhzProtocolDecoderKiaV5* instance = context;
    instance->decoder.parser_step = KiaV5DecoderStepReset;
    instance->header_count = 0;
    instance->bit_count = 0;
    instance->decoded_data = 0;
    instance->saved_key = 0;
    instance->yek = 0;
    instance->crc = 0;
    instance->manchester_state = ManchesterStateMid1;
}

void kia_protocol_decoder_v5_feed(void* context, bool level, uint32_t duration) {
    SubGhzProtocolDecoderKiaV5* instance = context;

    switch(instance->decoder.parser_step) {
    case KiaV5DecoderStepReset:
        if((level) && (DURATION_DIFF(duration, kia_protocol_v5_const.te_short) <
                       kia_protocol_v5_const.te_delta)) {
            instance->decoder.parser_step = KiaV5DecoderStepCheckPreamble;
            instance->decoder.te_last = duration;
            instance->header_count = 1;
            instance->bit_count = 0;
            instance->decoded_data = 0;
            manchester_advance(
                instance->manchester_state,
                ManchesterEventReset,
                &instance->manchester_state,
                NULL);
        }
        break;

    case KiaV5DecoderStepCheckPreamble:
        if(level) {
            if(DURATION_DIFF(duration, kia_protocol_v5_const.te_long) <
               kia_protocol_v5_const.te_delta) {
                if(instance->header_count > 40) {
                    instance->decoder.parser_step = KiaV5DecoderStepData;
                    instance->bit_count = 0;
                    instance->decoded_data = 0;
                    instance->saved_key = 0;
                    instance->header_count = 0;
                } else {
                    instance->decoder.te_last = duration;
                }
            } else if(
                DURATION_DIFF(duration, kia_protocol_v5_const.te_short) <
                kia_protocol_v5_const.te_delta) {
                instance->decoder.te_last = duration;
            } else {
                instance->decoder.parser_step = KiaV5DecoderStepReset;
            }
        } else {
            if((DURATION_DIFF(duration, kia_protocol_v5_const.te_short) <
                kia_protocol_v5_const.te_delta) &&
               (DURATION_DIFF(instance->decoder.te_last, kia_protocol_v5_const.te_short) <
                kia_protocol_v5_const.te_delta)) {
                instance->header_count++;
            } else if(
                (DURATION_DIFF(duration, kia_protocol_v5_const.te_long) <
                 kia_protocol_v5_const.te_delta) &&
                (DURATION_DIFF(instance->decoder.te_last, kia_protocol_v5_const.te_short) <
                 kia_protocol_v5_const.te_delta)) {
                instance->header_count++;
            } else if(
                DURATION_DIFF(instance->decoder.te_last, kia_protocol_v5_const.te_long) <
                kia_protocol_v5_const.te_delta) {
                instance->header_count++;
            } else {
                instance->decoder.parser_step = KiaV5DecoderStepReset;
            }
            instance->decoder.te_last = duration;
        }
        break;

    case KiaV5DecoderStepData: {
        ManchesterEvent event;

        if(DURATION_DIFF(duration, kia_protocol_v5_const.te_short) <
           kia_protocol_v5_const.te_delta) {
            event = level ? ManchesterEventShortHigh : ManchesterEventShortLow;
        } else if(
            DURATION_DIFF(duration, kia_protocol_v5_const.te_long) <
            kia_protocol_v5_const.te_delta) {
            event = level ? ManchesterEventLongHigh : ManchesterEventLongLow;
        } else {
            if(instance->bit_count >= kia_protocol_v5_const.min_count_bit_for_found) {
                instance->generic.data = instance->saved_key;
                instance->generic.data_count_bit =
                    (instance->bit_count > 67) ? 67 : instance->bit_count;

                instance->crc = (uint8_t)(instance->decoded_data & 0x07);

                instance->yek = 0;
                for(int i = 0; i < 8; i++) {
                    uint8_t byte = (instance->generic.data >> (i * 8)) & 0xFF;
                    uint8_t reversed = 0;
                    for(int b = 0; b < 8; b++) {
                        if(byte & (1 << b)) reversed |= (1 << (7 - b));
                    }
                    instance->yek |= ((uint64_t)reversed << ((7 - i) * 8));
                }

                instance->generic.serial = (uint32_t)((instance->yek >> 32) & 0x0FFFFFFF);
                instance->generic.btn = (uint8_t)((instance->yek >> 60) & 0x0F);

                uint32_t encrypted = (uint32_t)(instance->yek & 0xFFFFFFFF);
                instance->generic.cnt = mixer_decode(encrypted);

                FURI_LOG_I(
                    TAG,
                    "Key=%08lX%08lX Sn=%07lX Btn=%X Cnt=%04lX CRC=%X",
                    (uint32_t)(instance->generic.data >> 32),
                    (uint32_t)(instance->generic.data & 0xFFFFFFFF),
                    instance->generic.serial,
                    instance->generic.btn,
                    instance->generic.cnt,
                    instance->crc);

                if(instance->base.callback)
                    instance->base.callback(&instance->base, instance->base.context);
            }

            instance->decoder.parser_step = KiaV5DecoderStepReset;
            break;
        }

        bool data_bit;
        if(instance->bit_count <= 66 &&
           manchester_advance(
               instance->manchester_state, event, &instance->manchester_state, &data_bit)) {
            kia_v5_add_bit(instance, data_bit);

            if(instance->bit_count == 64) {
                instance->saved_key = instance->decoded_data;
                instance->decoded_data = 0;
            }
        }

        instance->decoder.te_last = duration;
        break;
    }
    }
}

SubGhzProtocolStatus kia_protocol_decoder_v5_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    SubGhzProtocolDecoderKiaV5* instance = context;

    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;

    ret = subghz_block_generic_serialize(&instance->generic, flipper_format, preset);

    if(ret == SubGhzProtocolStatusOk) {
        // Save decoded fields
        pp_serialize_fields(
            flipper_format,
            PP_FIELD_SERIAL | PP_FIELD_BTN | PP_FIELD_CNT,
            instance->generic.serial,
            instance->generic.btn,
            instance->generic.cnt,
            0);

        uint32_t crc_temp = instance->crc;
        flipper_format_write_uint32(flipper_format, "CRC", &crc_temp, 1);

        // Save raw bit data for exact reproduction (since V5 has complex bit reversal)
        uint32_t raw_high = (uint32_t)(instance->generic.data >> 32);
        uint32_t raw_low = (uint32_t)(instance->generic.data & 0xFFFFFFFF);
        flipper_format_write_uint32(flipper_format, "DataHi", &raw_high, 1);
        flipper_format_write_uint32(flipper_format, "DataLo", &raw_low, 1);
        uint32_t yek_high = (uint32_t)(instance->yek >> 32);
        uint32_t yek_low = (uint32_t)(instance->yek & 0xFFFFFFFF);
        flipper_format_write_uint32(flipper_format, "YekHi", &yek_high, 1);
        flipper_format_write_uint32(flipper_format, "YekLo", &yek_low, 1);
    }

    return ret;
}

SubGhzProtocolStatus
    kia_protocol_decoder_v5_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolDecoderKiaV5* instance = context;
    return subghz_block_generic_deserialize_check_count_bit(
        &instance->generic, flipper_format, kia_protocol_v5_const.min_count_bit_for_found);
}

void kia_protocol_decoder_v5_get_string(void* context, FuriString* output) {
    SubGhzProtocolDecoderKiaV5* instance = context;

    uint32_t code_found_hi = instance->generic.data >> 32;
    uint32_t code_found_lo = instance->generic.data & 0x00000000ffffffff;
    uint32_t yek_hi = (uint32_t)(instance->yek >> 32);
    uint32_t yek_lo = (uint32_t)(instance->yek & 0xFFFFFFFF);

    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:%08lX%08lX\r\n"
        "Yek:%08lX%08lX\r\n"
        "Sn:%07lX Btn:%X Cnt:%04lX\r\n"
        "CRC:%X\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        code_found_hi,
        code_found_lo,
        yek_hi,
        yek_lo,
        instance->generic.serial,
        instance->generic.btn,
        instance->generic.cnt,
        instance->crc);
}
