#include "star_line.h"
#include "../pp_platform.h"
#include "keeloq_common.h"

#include <lib/subghz/subghz_keystore.h>

#include "protocols_common.h"

#define TAG "SubGhzProtocolStarLine"

static const SubGhzBlockConst subghz_protocol_star_line_const = {
    .te_short = 250,
    .te_long = 500,
    .te_delta = 120,
    .min_count_bit_for_found = 64,
};

#define STAR_LINE_MIN_COUNT_BIT   64U
#define STAR_LINE_UPLOAD_CAPACITY ((6U * 2U) + (STAR_LINE_MIN_COUNT_BIT * 2U))
_Static_assert(
    STAR_LINE_UPLOAD_CAPACITY <= PP_SHARED_UPLOAD_CAPACITY,
    "STAR_LINE_UPLOAD_CAPACITY exceeds shared upload slab");

struct SubGhzProtocolDecoderStarLine {
    SubGhzProtocolDecoderBase base;

    SubGhzBlockDecoder decoder;
    SubGhzBlockGeneric generic;

    uint16_t header_count;
    SubGhzKeystore* keystore;
    const char* manufacture_name;

    FuriString* manufacture_from_file;
};

struct SubGhzProtocolEncoderStarLine {
    SubGhzProtocolEncoderBase base;

    SubGhzProtocolBlockEncoder encoder;
    SubGhzBlockGeneric generic;

    SubGhzKeystore* keystore;
    const char* manufacture_name;

    FuriString* manufacture_from_file;
};

typedef enum {
    StarLineDecoderStepReset = 0,
    StarLineDecoderStepCheckPreambula,
    StarLineDecoderStepSaveDuration,
    StarLineDecoderStepCheckDuration,
} StarLineDecoderStep;

const SubGhzProtocolDecoder subghz_protocol_star_line_decoder = {
    .alloc = subghz_protocol_decoder_star_line_alloc,
    .free = subghz_protocol_decoder_star_line_free,

    .feed = subghz_protocol_decoder_star_line_feed,
    .reset = subghz_protocol_decoder_star_line_reset,

    .get_hash_data = pp_decoder_hash_blocks,
    .serialize = subghz_protocol_decoder_star_line_serialize,
    .deserialize = subghz_protocol_decoder_star_line_deserialize,
    .get_string = subghz_protocol_decoder_star_line_get_string,
};

#if PROTOPIRATE_WITH_ENCODER
const SubGhzProtocolEncoder subghz_protocol_star_line_encoder = {
    .alloc = subghz_protocol_encoder_star_line_alloc,
    .free = subghz_protocol_encoder_star_line_free,

    .deserialize = subghz_protocol_encoder_star_line_deserialize,
    .stop = pp_encoder_stop,
    .yield = pp_encoder_yield,
};
#else
const SubGhzProtocolEncoder subghz_protocol_star_line_encoder = {
    .alloc = NULL,
    .free = NULL,
    .deserialize = NULL,
    .stop = NULL,
    .yield = NULL,
};
#endif

const SubGhzProtocol subghz_protocol_star_line = {
    .name = SUBGHZ_PROTOCOL_STAR_LINE_NAME,
    .type = SubGhzProtocolTypeDynamic,
    .flag = SubGhzProtocolFlag_433 | SubGhzProtocolFlag_AM | SubGhzProtocolFlag_Decodable |
            SubGhzProtocolFlag_Load | SubGhzProtocolFlag_Save | SubGhzProtocolFlag_Send,

#if PROTOPIRATE_WITH_DECODER

    .decoder = &subghz_protocol_star_line_decoder,

#else

    .decoder = NULL,

#endif
#if PROTOPIRATE_WITH_ENCODER
    .encoder = &subghz_protocol_star_line_encoder,
#else
    .encoder = NULL,
#endif
};

/** 
 * Analysis of received data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 * @param keystore Pointer to a SubGhzKeystore* instance
 * @param manufacture_name
 */
static void subghz_protocol_star_line_check_remote_controller(
    SubGhzBlockGeneric* instance,
    SubGhzKeystore* keystore,
    const char** manufacture_name);
#if PROTOPIRATE_WITH_ENCODER

void* subghz_protocol_encoder_star_line_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolEncoderStarLine* instance = calloc(1, sizeof(SubGhzProtocolEncoderStarLine));

    instance->base.protocol = &subghz_protocol_star_line;
    instance->generic.protocol_name = instance->base.protocol->name;
    instance->keystore = subghz_environment_get_keystore(environment);

    instance->manufacture_from_file = furi_string_alloc();

    instance->encoder.repeat = 40;
    pp_encoder_buffer_ensure(instance, STAR_LINE_UPLOAD_CAPACITY);
    instance->encoder.is_running = false;

    return instance;
}

#endif
#if PROTOPIRATE_WITH_ENCODER

void subghz_protocol_encoder_star_line_free(void* context) {
    SubGhzProtocolEncoderStarLine* instance = context;
    furi_string_free(instance->manufacture_from_file);
    free(instance);
}

#endif

/** 
 * Key generation from simple data
 * @param instance Pointer to a SubGhzProtocolEncoderKeeloq* instance
 * @param btn Button number, 4 bit
 */
static bool
    subghz_protocol_star_line_gen_data(SubGhzProtocolEncoderStarLine* instance, uint8_t btn) {
    // Increase counter
    if((instance->generic.cnt + 1) > 0xFFFF) {
        instance->generic.cnt = 0;
    } else {
        instance->generic.cnt += 1;
    }

    uint32_t fix = btn << 24 | instance->generic.serial;
    uint32_t decrypt = btn << 24 | (instance->generic.serial & 0xFF) << 16 | instance->generic.cnt;
    uint32_t hop = 0;
    uint64_t man = 0;
    uint64_t code_found_reverse;
    int res = 0;

    if(instance->manufacture_name == 0x0) {
        instance->manufacture_name = "";
    }

    if(strcmp(instance->manufacture_name, "Unknown") == 0) {
        code_found_reverse = subghz_protocol_blocks_reverse_key(
            instance->generic.data, instance->generic.data_count_bit);
        hop = code_found_reverse & 0x00000000ffffffff;
    } else {
        uint8_t kl_type_en = instance->keystore->kl_type;
        for
            M_EACH(
                manufacture_code,
                *subghz_keystore_get_data(instance->keystore),
                SubGhzKeyArray_t) {
                res = strcmp(
                    furi_string_get_cstr(manufacture_code->name), instance->manufacture_name);
                if(res == 0) {
                    switch(manufacture_code->type) {
                    case KEELOQ_LEARNING_SIMPLE:
                        //Simple Learning
                        hop =
                            subghz_protocol_keeloq_common_encrypt(decrypt, manufacture_code->key);
                        break;
                    case KEELOQ_LEARNING_NORMAL:
                        //Normal Learning
                        man = subghz_protocol_keeloq_common_normal_learning(
                            fix, manufacture_code->key);
                        hop = subghz_protocol_keeloq_common_encrypt(decrypt, man);
                        break;
                    case KEELOQ_LEARNING_UNKNOWN:
                        if(kl_type_en == 1) {
                            hop = subghz_protocol_keeloq_common_encrypt(
                                decrypt, manufacture_code->key);
                        }
                        if(kl_type_en == 2) {
                            man = subghz_protocol_keeloq_common_normal_learning(
                                fix, manufacture_code->key);
                            hop = subghz_protocol_keeloq_common_encrypt(decrypt, man);
                        }
                        break;
                    }
                    break;
                }
            }
    }
    if(hop) {
        uint64_t yek = (uint64_t)fix << 32 | hop;
        instance->generic.data =
            subghz_protocol_blocks_reverse_key(yek, instance->generic.data_count_bit);
        return true;
    } else {
        instance->manufacture_name = "Unknown";
        return false;
    }
}

bool subghz_protocol_star_line_create_data(
    void* context,
    FlipperFormat* flipper_format,
    uint32_t serial,
    uint8_t btn,
    uint16_t cnt,
    const char* manufacture_name,
    SubGhzRadioPreset* preset) {
    SubGhzProtocolEncoderStarLine* instance = context;
    instance->generic.serial = serial;
    instance->generic.cnt = cnt;
    instance->manufacture_name = manufacture_name;
    instance->generic.data_count_bit = 64;
    bool res = subghz_protocol_star_line_gen_data(instance, btn);
    if(res) {
        return SubGhzProtocolStatusOk ==
               subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
    }
    return res;
}

/**
 * Generating an upload from data.
 * @param instance Pointer to a SubGhzProtocolEncoderKeeloq instance
 * @return true On success
 */
#if PROTOPIRATE_WITH_ENCODER
static bool subghz_protocol_encoder_star_line_get_upload(
    SubGhzProtocolEncoderStarLine* instance,
    uint8_t btn) {
    // Gen new key
    if(!subghz_protocol_star_line_gen_data(instance, btn)) {
        return false;
    }

    size_t index = 0;
    size_t size_upload = 6 * 2 + (instance->generic.data_count_bit * 2);
    if(size_upload > instance->encoder.size_upload) {
        FURI_LOG_E(TAG, "Size upload exceeds allocated encoder buffer.");
        return false;
    } else {
        instance->encoder.size_upload = size_upload;
    }

    LevelDuration* up = instance->encoder.upload;
    const size_t cap = STAR_LINE_UPLOAD_CAPACITY;
    const uint32_t te_short = (uint32_t)subghz_protocol_star_line_const.te_short;
    const uint32_t te_long = (uint32_t)subghz_protocol_star_line_const.te_long;

    index = pp_emit_short_pairs(up, index, cap, te_long * 2U, 6);

    for(uint8_t i = instance->generic.data_count_bit; i > 0; i--) {
        const uint32_t te = bit_read(instance->generic.data, i - 1) ? te_long : te_short;
        index = pp_emit(up, index, cap, true, te);
        index = pp_emit(up, index, cap, false, te);
    }

    return true;
}

#endif
#if PROTOPIRATE_WITH_ENCODER

static SubGhzProtocolStatus subghz_protocol_encoder_star_line_serialize(
    SubGhzProtocolEncoderStarLine* instance,
    FlipperFormat* flipper_format) {
    subghz_protocol_star_line_check_remote_controller(
        &instance->generic, instance->keystore, &instance->manufacture_name);

    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;

    do {
        if(!flipper_format_insert_or_update_string_cstr(
               flipper_format, FF_PROTOCOL, instance->generic.protocol_name)) {
            break;
        }

        uint32_t bits = instance->generic.data_count_bit;
        if(!flipper_format_insert_or_update_uint32(flipper_format, FF_BIT, &bits, 1)) {
            break;
        }

        char key_str[20];
        snprintf(key_str, sizeof(key_str), "%016llX", instance->generic.data);
        if(!flipper_format_insert_or_update_string_cstr(flipper_format, FF_KEY, key_str)) {
            break;
        }

        if(!flipper_format_insert_or_update_uint32(
               flipper_format, FF_SERIAL, &instance->generic.serial, 1)) {
            break;
        }

        uint32_t temp = instance->generic.btn;
        if(!flipper_format_insert_or_update_uint32(flipper_format, FF_BTN, &temp, 1)) {
            break;
        }

        if(!flipper_format_insert_or_update_uint32(
               flipper_format, FF_CNT, &instance->generic.cnt, 1)) {
            break;
        }

        if(!flipper_format_insert_or_update_string_cstr(
               flipper_format, FF_MANUFACTURE, instance->manufacture_name)) {
            break;
        }

        ret = SubGhzProtocolStatusOk;
    } while(false);

    return ret;
    //SubGhzProtocolStatus ret = subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
}

#endif
#if PROTOPIRATE_WITH_ENCODER

SubGhzProtocolStatus
    subghz_protocol_encoder_star_line_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolEncoderStarLine* instance = context;

    if(pp_verify_protocol_name(flipper_format, instance->base.protocol->name) !=
       SubGhzProtocolStatusOk) {
        return SubGhzProtocolStatusError;
    }

    uint32_t bits = 0;
    if(pp_encoder_read_bit(flipper_format, NULL, 0, &bits) != SubGhzProtocolStatusOk) {
        return SubGhzProtocolStatusErrorValueBitCount;
    }
    instance->generic.data_count_bit = subghz_protocol_star_line_const.min_count_bit_for_found;

    uint64_t key = 0;
    if(!pp_flipper_read_hex_u64(flipper_format, FF_KEY, &key)) {
        return SubGhzProtocolStatusError;
    }
    instance->generic.data = key;
    if(instance->generic.data == 0) return SubGhzProtocolStatusError;

    uint32_t serial = UINT32_MAX;
    uint32_t btn = UINT32_MAX;
    uint32_t cnt = UINT32_MAX;
    pp_encoder_read_fields(flipper_format, &serial, &btn, &cnt, NULL);
    if(serial == UINT32_MAX || btn == UINT32_MAX || cnt == UINT32_MAX) {
        return SubGhzProtocolStatusError;
    }
    instance->generic.serial = serial;
    instance->generic.btn = (uint8_t)btn;
    instance->generic.cnt = (uint16_t)cnt;

    instance->encoder.repeat = pp_encoder_read_repeat(flipper_format, 40);

    flipper_format_rewind(flipper_format);
    if(flipper_format_read_string(
           flipper_format, FF_MANUFACTURE, instance->manufacture_from_file)) {
        instance->manufacture_name = furi_string_get_cstr(instance->manufacture_from_file);
        instance->keystore->mfname = instance->manufacture_name;
    }

    subghz_protocol_star_line_check_remote_controller(
        &instance->generic, instance->keystore, &instance->manufacture_name);

    subghz_protocol_encoder_star_line_get_upload(instance, instance->generic.btn);
    subghz_protocol_encoder_star_line_serialize(instance, flipper_format);

    instance->encoder.is_running = true;
    return SubGhzProtocolStatusOk;
}

#endif
void* subghz_protocol_decoder_star_line_alloc(SubGhzEnvironment* environment) {
    SubGhzProtocolDecoderStarLine* instance = malloc(sizeof(SubGhzProtocolDecoderStarLine));
    instance->base.protocol = &subghz_protocol_star_line;
    instance->generic.protocol_name = instance->base.protocol->name;

    instance->manufacture_from_file = furi_string_alloc();

    instance->keystore = subghz_environment_get_keystore(environment);

    return instance;
}

void subghz_protocol_decoder_star_line_free(void* context) {
    SubGhzProtocolDecoderStarLine* instance = context;
    furi_string_free(instance->manufacture_from_file);

    free(instance);
}

void subghz_protocol_decoder_star_line_reset(void* context) {
    SubGhzProtocolDecoderStarLine* instance = context;
    instance->decoder.parser_step = StarLineDecoderStepReset;
    // TODO
    instance->keystore->mfname = "";
    instance->keystore->kl_type = 0;
}

void subghz_protocol_decoder_star_line_feed(void* context, bool level, uint32_t duration) {
    SubGhzProtocolDecoderStarLine* instance = context;

    switch(instance->decoder.parser_step) {
    case StarLineDecoderStepReset:
        if(level) {
            if(DURATION_DIFF(duration, subghz_protocol_star_line_const.te_long * 2) <
               subghz_protocol_star_line_const.te_delta * 2) {
                instance->decoder.parser_step = StarLineDecoderStepCheckPreambula;
                instance->header_count++;
            } else if(instance->header_count > 4) {
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->decoder.te_last = duration;
                instance->decoder.parser_step = StarLineDecoderStepCheckDuration;
            }
        } else {
            instance->header_count = 0;
        }
        break;
    case StarLineDecoderStepCheckPreambula:
        if((!level) && (DURATION_DIFF(duration, subghz_protocol_star_line_const.te_long * 2) <
                        subghz_protocol_star_line_const.te_delta * 2)) {
            //Found Preambula
            instance->decoder.parser_step = StarLineDecoderStepReset;
        } else {
            instance->header_count = 0;
            instance->decoder.parser_step = StarLineDecoderStepReset;
        }
        break;
    case StarLineDecoderStepSaveDuration:
        if(level) {
            if(duration >= (subghz_protocol_star_line_const.te_long +
                            subghz_protocol_star_line_const.te_delta)) {
                instance->decoder.parser_step = StarLineDecoderStepReset;
                if((instance->decoder.decode_count_bit >=
                    subghz_protocol_star_line_const.min_count_bit_for_found) &&
                   (instance->decoder.decode_count_bit <=
                    subghz_protocol_star_line_const.min_count_bit_for_found + 2)) {
                    if(instance->generic.data != instance->decoder.decode_data) {
                        instance->generic.data = instance->decoder.decode_data;
                        instance->generic.data_count_bit =
                            subghz_protocol_star_line_const.min_count_bit_for_found;
                        if(instance->base.callback)
                            instance->base.callback(&instance->base, instance->base.context);
                    }
                }
                instance->decoder.decode_data = 0;
                instance->decoder.decode_count_bit = 0;
                instance->header_count = 0;
                break;
            } else {
                instance->decoder.te_last = duration;
                instance->decoder.parser_step = StarLineDecoderStepCheckDuration;
            }

        } else {
            instance->decoder.parser_step = StarLineDecoderStepReset;
        }
        break;
    case StarLineDecoderStepCheckDuration:
        if(!level) {
            if((DURATION_DIFF(instance->decoder.te_last, subghz_protocol_star_line_const.te_short) <
                subghz_protocol_star_line_const.te_delta) &&
               (DURATION_DIFF(duration, subghz_protocol_star_line_const.te_short) <
                subghz_protocol_star_line_const.te_delta)) {
                if(instance->decoder.decode_count_bit <
                   subghz_protocol_star_line_const.min_count_bit_for_found) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 0);
                } else {
                    instance->decoder.decode_count_bit++;
                }
                instance->decoder.parser_step = StarLineDecoderStepSaveDuration;
            } else if(
                (DURATION_DIFF(instance->decoder.te_last, subghz_protocol_star_line_const.te_long) <
                 subghz_protocol_star_line_const.te_delta) &&
                (DURATION_DIFF(duration, subghz_protocol_star_line_const.te_long) <
                 subghz_protocol_star_line_const.te_delta)) {
                if(instance->decoder.decode_count_bit <
                   subghz_protocol_star_line_const.min_count_bit_for_found) {
                    subghz_protocol_blocks_add_bit(&instance->decoder, 1);
                } else {
                    instance->decoder.decode_count_bit++;
                }
                instance->decoder.parser_step = StarLineDecoderStepSaveDuration;
            } else {
                instance->decoder.parser_step = StarLineDecoderStepReset;
            }
        } else {
            instance->decoder.parser_step = StarLineDecoderStepReset;
        }
        break;
    }
}

/**
 * Validation of decrypt data.
 * @param instance Pointer to a SubGhzBlockGeneric instance
 * @param decrypt Decrypd data
 * @param btn Button number, 4 bit
 * @param end_serial decrement the last 10 bits of the serial number
 * @return true On success
 */
static inline bool subghz_protocol_star_line_check_decrypt(
    SubGhzBlockGeneric* instance,
    uint32_t decrypt,
    uint8_t btn,
    uint32_t end_serial) {
    if((decrypt >> 24 == btn) && ((((uint16_t)(decrypt >> 16)) & 0x00FF) == end_serial)) {
        instance->cnt = decrypt & 0x0000FFFF;
        return true;
    }
    return false;
}

/** 
 * Checking the accepted code against the database manafacture key
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 * @param fix Fix part of the parcel
 * @param hop Hop encrypted part of the parcel
 * @param keystore Pointer to a SubGhzKeystore* instance
 * @param manufacture_name 
 * @return true on successful search
 */
static uint8_t subghz_protocol_star_line_check_remote_controller_selector(
    SubGhzBlockGeneric* instance,
    uint32_t fix,
    uint32_t hop,
    SubGhzKeystore* keystore,
    const char** manufacture_name) {
    uint16_t end_serial = (uint16_t)(fix & 0xFF);
    uint8_t btn = (uint8_t)(fix >> 24);
    uint32_t decrypt = 0;
    uint64_t man_normal_learning;
    bool mf_not_set = false;
    // TODO:
    // if(mfname == 0x0) {
    //     mfname = "";
    // }

    const char* mfname = keystore->mfname;

    if(strcmp(mfname, "Unknown") == 0) {
        return 1;
    } else if(strcmp(mfname, "") == 0) {
        mf_not_set = true;
    }
    for
        M_EACH(manufacture_code, *subghz_keystore_get_data(keystore), SubGhzKeyArray_t) {
            if(mf_not_set || (strcmp(furi_string_get_cstr(manufacture_code->name), mfname) == 0)) {
                switch(manufacture_code->type) {
                case KEELOQ_LEARNING_SIMPLE:
                    // Simple Learning
                    decrypt = subghz_protocol_keeloq_common_decrypt(hop, manufacture_code->key);
                    if(subghz_protocol_star_line_check_decrypt(
                           instance, decrypt, btn, end_serial)) {
                        *manufacture_name = furi_string_get_cstr(manufacture_code->name);
                        keystore->mfname = *manufacture_name;
                        return 1;
                    }
                    break;
                case KEELOQ_LEARNING_NORMAL:
                    // Normal Learning
                    // https://phreakerclub.com/forum/showpost.php?p=43557&postcount=37
                    man_normal_learning =
                        subghz_protocol_keeloq_common_normal_learning(fix, manufacture_code->key);
                    decrypt = subghz_protocol_keeloq_common_decrypt(hop, man_normal_learning);
                    if(subghz_protocol_star_line_check_decrypt(
                           instance, decrypt, btn, end_serial)) {
                        *manufacture_name = furi_string_get_cstr(manufacture_code->name);
                        keystore->mfname = *manufacture_name;
                        return 1;
                    }
                    break;
                case KEELOQ_LEARNING_UNKNOWN:
                    // Simple Learning
                    decrypt = subghz_protocol_keeloq_common_decrypt(hop, manufacture_code->key);
                    if(subghz_protocol_star_line_check_decrypt(
                           instance, decrypt, btn, end_serial)) {
                        *manufacture_name = furi_string_get_cstr(manufacture_code->name);
                        keystore->mfname = *manufacture_name;
                        keystore->kl_type = 1;
                        return 1;
                    }
                    // Check for mirrored man
                    uint64_t man_rev = 0;
                    uint64_t man_rev_byte = 0;
                    for(uint8_t i = 0; i < 64; i += 8) {
                        man_rev_byte = (uint8_t)(manufacture_code->key >> i);
                        man_rev = man_rev | man_rev_byte << (56 - i);
                    }
                    decrypt = subghz_protocol_keeloq_common_decrypt(hop, man_rev);
                    if(subghz_protocol_star_line_check_decrypt(
                           instance, decrypt, btn, end_serial)) {
                        *manufacture_name = furi_string_get_cstr(manufacture_code->name);
                        keystore->mfname = *manufacture_name;
                        keystore->kl_type = 1;
                        return 1;
                    }
                    //###########################
                    // Normal Learning
                    // https://phreakerclub.com/forum/showpost.php?p=43557&postcount=37
                    man_normal_learning =
                        subghz_protocol_keeloq_common_normal_learning(fix, manufacture_code->key);
                    decrypt = subghz_protocol_keeloq_common_decrypt(hop, man_normal_learning);
                    if(subghz_protocol_star_line_check_decrypt(
                           instance, decrypt, btn, end_serial)) {
                        *manufacture_name = furi_string_get_cstr(manufacture_code->name);
                        keystore->mfname = *manufacture_name;
                        keystore->kl_type = 2;
                        return 1;
                    }
                    // Check for mirrored man
                    man_normal_learning =
                        subghz_protocol_keeloq_common_normal_learning(fix, man_rev);
                    decrypt = subghz_protocol_keeloq_common_decrypt(hop, man_normal_learning);
                    if(subghz_protocol_star_line_check_decrypt(
                           instance, decrypt, btn, end_serial)) {
                        *manufacture_name = furi_string_get_cstr(manufacture_code->name);
                        keystore->mfname = *manufacture_name;
                        keystore->kl_type = 2;
                        return 1;
                    }
                    break;
                }
            }
        }

    *manufacture_name = "Unknown";
    keystore->mfname = "Unknown";
    instance->cnt = 0;

    return 0;
}

/** 
 * Analysis of received data
 * @param instance Pointer to a SubGhzBlockGeneric* instance
 * @param keystore Pointer to a SubGhzKeystore* instance
 * @param manufacture_name
 */
static void subghz_protocol_star_line_check_remote_controller(
    SubGhzBlockGeneric* instance,
    SubGhzKeystore* keystore,
    const char** manufacture_name) {
    uint64_t key = subghz_protocol_blocks_reverse_key(instance->data, instance->data_count_bit);
    uint32_t key_fix = key >> 32;
    uint32_t key_hop = key & 0x00000000ffffffff;

    subghz_protocol_star_line_check_remote_controller_selector(
        instance, key_fix, key_hop, keystore, manufacture_name);

    instance->serial = key_fix & 0x00FFFFFF;
    instance->btn = key_fix >> 24;
}

SubGhzProtocolStatus subghz_protocol_decoder_star_line_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    SubGhzProtocolDecoderStarLine* instance = context;
    subghz_protocol_star_line_check_remote_controller(
        &instance->generic, instance->keystore, &instance->manufacture_name);

    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;

    do {
        if(preset != NULL) {
            if(!flipper_format_insert_or_update_uint32(
                   flipper_format, FF_FREQUENCY, &preset->frequency, 1)) {
                break;
            }

            const char* preset_name = furi_string_get_cstr(preset->name);
            const char* short_preset = pp_get_short_preset_name(preset_name);
            if(!flipper_format_insert_or_update_string_cstr(
                   flipper_format, FF_PRESET, short_preset)) {
                break;
            }
        }

        if(!flipper_format_insert_or_update_string_cstr(
               flipper_format, FF_PROTOCOL, instance->generic.protocol_name)) {
            break;
        }

        uint32_t bits = instance->generic.data_count_bit;
        if(!flipper_format_insert_or_update_uint32(flipper_format, FF_BIT, &bits, 1)) {
            break;
        }

        char key_str[20];
        snprintf(key_str, sizeof(key_str), "%016llX", instance->generic.data);
        if(!flipper_format_insert_or_update_string_cstr(flipper_format, FF_KEY, key_str)) {
            break;
        }

        if(!flipper_format_insert_or_update_uint32(
               flipper_format, FF_SERIAL, &instance->generic.serial, 1)) {
            break;
        }

        uint32_t temp = instance->generic.btn;
        if(!flipper_format_insert_or_update_uint32(flipper_format, FF_BTN, &temp, 1)) {
            break;
        }

        if(!flipper_format_insert_or_update_uint32(
               flipper_format, FF_CNT, &instance->generic.cnt, 1)) {
            break;
        }

        if(!flipper_format_insert_or_update_string_cstr(
               flipper_format, FF_MANUFACTURE, instance->manufacture_name)) {
            break;
        }

        ret = SubGhzProtocolStatusOk;
    } while(false);

    return ret;
    //SubGhzProtocolStatus ret = subghz_block_generic_serialize(&instance->generic, flipper_format, preset);
}

SubGhzProtocolStatus
    subghz_protocol_decoder_star_line_deserialize(void* context, FlipperFormat* flipper_format) {
    SubGhzProtocolDecoderStarLine* instance = context;
    SubGhzProtocolStatus ret = SubGhzProtocolStatusError;

    flipper_format_rewind(flipper_format);

    do {
        if(pp_verify_protocol_name(flipper_format, instance->base.protocol->name) !=
           SubGhzProtocolStatusOk) {
            FURI_LOG_E(TAG, "Missing or wrong Protocol");
            break;
        }

        uint32_t bit_count_temp;
        if(!flipper_format_read_uint32(flipper_format, FF_BIT, &bit_count_temp, 1)) {
            FURI_LOG_E(TAG, "Missing Bit");
            break;
        }

        instance->generic.data_count_bit = subghz_protocol_star_line_const.min_count_bit_for_found;

        uint64_t key = 0;
        if(!pp_flipper_read_hex_u64(flipper_format, FF_KEY, &key)) {
            FURI_LOG_E(TAG, "Missing Key");
            break;
        }

        instance->generic.data = key;
        FURI_LOG_I(TAG, "Parsed key: 0x%016llX", instance->generic.data);

        if(instance->generic.data == 0) {
            FURI_LOG_E(TAG, "Key is zero after parsing!");
            break;
        }

        if(!flipper_format_read_uint32(flipper_format, FF_SERIAL, &instance->generic.serial, 1)) {
            FURI_LOG_E(TAG, "Missing Serial");
            break;
        }

        uint32_t btn_temp;
        if(!flipper_format_read_uint32(flipper_format, FF_BTN, &btn_temp, 1)) {
            FURI_LOG_E(TAG, "Missing Btn");
            break;
        }
        instance->generic.btn = (uint8_t)btn_temp;

        uint32_t cnt_temp;
        if(!flipper_format_read_uint32(flipper_format, FF_CNT, &cnt_temp, 1)) {
            FURI_LOG_E(TAG, "Missing Cnt");
            break;
        }
        instance->generic.cnt = (uint16_t)cnt_temp;

        if(!flipper_format_rewind(flipper_format)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        // Read manufacturer from file
        if(flipper_format_read_string(
               flipper_format, FF_MANUFACTURE, instance->manufacture_from_file)) {
            instance->manufacture_name = furi_string_get_cstr(instance->manufacture_from_file);
            instance->keystore->mfname = instance->manufacture_name;
        } else {
            FURI_LOG_D(TAG, "DECODER: Missing Manufacture");
        }

        FURI_LOG_I(TAG, "Decoder deserialized");

        ret = SubGhzProtocolStatusOk;
    } while(false);

    return ret;
}

void subghz_protocol_decoder_star_line_get_string(void* context, FuriString* output) {
    SubGhzProtocolDecoderStarLine* instance = context;

    subghz_protocol_star_line_check_remote_controller(
        &instance->generic, instance->keystore, &instance->manufacture_name);

    uint32_t code_found_hi = instance->generic.data >> 32;
    uint32_t code_found_lo = instance->generic.data & 0x00000000ffffffff;

    uint64_t code_found_reverse = subghz_protocol_blocks_reverse_key(
        instance->generic.data, instance->generic.data_count_bit);
    uint32_t code_found_reverse_hi = code_found_reverse >> 32;
    uint32_t code_found_reverse_lo = code_found_reverse & 0x00000000ffffffff;

    furi_string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:%08lX%08lX\r\n"
        "Fix:0x%08lX    Cnt:%04lX\r\n"
        "Hop:0x%08lX    Btn:%02X\r\n"
        "MF:%s\r\n",
        instance->generic.protocol_name,
        instance->generic.data_count_bit,
        code_found_hi,
        code_found_lo,
        code_found_reverse_hi,
        instance->generic.cnt,
        code_found_reverse_lo,
        instance->generic.btn,
        instance->manufacture_name);
}
