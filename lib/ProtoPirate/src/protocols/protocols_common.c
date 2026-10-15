#include "protocols_common.h"

#include <string.h>

uint8_t pp_reverse_bits8(uint8_t value) {
    value = (uint8_t)(((value >> 4U) | (value << 4U)) & 0xFFU);
    value = (uint8_t)(((value & 0x33U) << 2U) | ((value >> 2U) & 0x33U));
    value = (uint8_t)(((value & 0x55U) << 1U) | ((value >> 1U) & 0x55U));
    return value;
}

void pp_u64_to_bytes_be(uint64_t data, uint8_t bytes[8]) {
    for(size_t i = 0; i < 8; i++) {
        bytes[i] = (uint8_t)((data >> ((7U - i) * 8U)) & 0xFFU);
    }
}

uint64_t pp_bytes_to_u64_be(const uint8_t bytes[8]) {
    uint64_t data = 0;
    for(size_t i = 0; i < 8; i++) {
        data = (data << 8U) | bytes[i];
    }
    return data;
}

static bool pp_hex_nibble(char c, uint8_t* nibble) {
    if(c >= '0' && c <= '9') {
        *nibble = (uint8_t)(c - '0');
    } else if(c >= 'A' && c <= 'F') {
        *nibble = (uint8_t)(c - 'A' + 10);
    } else if(c >= 'a' && c <= 'f') {
        *nibble = (uint8_t)(c - 'a' + 10);
    } else {
        return false;
    }
    return true;
}

bool pp_parse_hex_u64_strict(const char* str, uint64_t* out_key) {
    if(!str || !out_key) {
        return false;
    }

    uint64_t key = 0;
    uint8_t hex_pos = 0;
    for(size_t i = 0; str[i] != '\0' && hex_pos < 16; i++) {
        if(str[i] == ' ') {
            continue;
        }

        uint8_t nibble = 0;
        if(!pp_hex_nibble(str[i], &nibble)) {
            return false;
        }
        key = (key << 4) | nibble;
        hex_pos++;
    }

    if(hex_pos != 16) {
        return false;
    }
    *out_key = key;
    return true;
}

bool pp_flipper_read_hex_u64(FlipperFormat* flipper_format, const char* key, uint64_t* out_key) {
    FuriString* value = furi_string_alloc();
    if(!value) {
        return false;
    }

    bool ok = false;
    if(flipper_format_rewind(flipper_format) &&
       flipper_format_read_string(flipper_format, key, value)) {
        ok = pp_parse_hex_u64_strict(furi_string_get_cstr(value), out_key);
    }
    furi_string_free(value);
    return ok;
}

void pp_flipper_update_or_insert_u32(
    FlipperFormat* flipper_format,
    const char* key,
    uint32_t value) {
    flipper_format_rewind(flipper_format);
    if(!flipper_format_update_uint32(flipper_format, key, &value, 1)) {
        flipper_format_rewind(flipper_format);
        flipper_format_insert_or_update_uint32(flipper_format, key, &value, 1);
    }
}

SubGhzProtocolStatus pp_verify_protocol_name(FlipperFormat* ff, const char* expected_name) {
    if(!ff || !expected_name) {
        return SubGhzProtocolStatusError;
    }
    FuriString* tmp = furi_string_alloc();
    if(!tmp) {
        return SubGhzProtocolStatusError;
    }
    SubGhzProtocolStatus result = SubGhzProtocolStatusError;
    if(!flipper_format_read_string(ff, FF_PROTOCOL, tmp)) {
        result = SubGhzProtocolStatusErrorParserOthers;
    } else if(furi_string_equal(tmp, expected_name)) {
        result = SubGhzProtocolStatusOk;
    }
    furi_string_free(tmp);
    return result;
}

SubGhzProtocolStatus pp_encoder_read_bit(
    FlipperFormat* ff,
    const uint16_t* allowed_bits,
    size_t allowed_bits_count,
    uint32_t* out_bit) {
    if(!ff || !out_bit) return SubGhzProtocolStatusError;
    flipper_format_rewind(ff);
    uint32_t bit = 0;
    if(!flipper_format_read_uint32(ff, FF_BIT, &bit, 1)) {
        return SubGhzProtocolStatusErrorValueBitCount;
    }
    if(allowed_bits && allowed_bits_count) {
        bool ok = false;
        for(size_t i = 0; i < allowed_bits_count; i++) {
            if((uint32_t)allowed_bits[i] == bit) {
                ok = true;
                break;
            }
        }
        if(!ok) return SubGhzProtocolStatusError;
    }
    *out_bit = bit;
    return SubGhzProtocolStatusOk;
}

void pp_encoder_read_fields(
    FlipperFormat* ff,
    uint32_t* serial_out,
    uint32_t* btn_out,
    uint32_t* cnt_out,
    uint32_t* type_out) {
    if(!ff) return;
    uint32_t tmp = 0;
    if(serial_out) {
        flipper_format_rewind(ff);
        if(flipper_format_read_uint32(ff, FF_SERIAL, &tmp, 1)) *serial_out = tmp;
    }
    if(btn_out) {
        flipper_format_rewind(ff);
        if(flipper_format_read_uint32(ff, FF_BTN, &tmp, 1)) *btn_out = tmp;
    }
    if(cnt_out) {
        flipper_format_rewind(ff);
        if(flipper_format_read_uint32(ff, FF_CNT, &tmp, 1)) *cnt_out = tmp;
    }
    if(type_out) {
        flipper_format_rewind(ff);
        if(flipper_format_read_uint32(ff, FF_TYPE, &tmp, 1)) *type_out = tmp;
    }
}

uint32_t pp_encoder_read_repeat(FlipperFormat* ff, uint32_t default_repeat) {
    if(!ff) return default_repeat;
    flipper_format_rewind(ff);
    uint32_t tmp = 0;
    if(!flipper_format_read_uint32(ff, FF_REPEAT, &tmp, 1)) {
        return default_repeat;
    }

    if(tmp == 0) {
        return default_repeat;
    }
    if(tmp > PP_ENCODER_REPEAT_MAX) {
        tmp = PP_ENCODER_REPEAT_MAX;
    }
    return tmp;
}

SubGhzProtocolStatus pp_serialize_fields(
    FlipperFormat* ff,
    uint32_t field_mask,
    uint32_t serial,
    uint32_t btn,
    uint32_t cnt,
    uint32_t type) {
    if(!ff) return SubGhzProtocolStatusError;

    if(field_mask & PP_FIELD_SERIAL) {
        pp_flipper_update_or_insert_u32(ff, FF_SERIAL, serial);
    }
    if(field_mask & PP_FIELD_BTN) {
        pp_flipper_update_or_insert_u32(ff, FF_BTN, btn);
    }
    if(field_mask & PP_FIELD_CNT) {
        pp_flipper_update_or_insert_u32(ff, FF_CNT, cnt);
    }
    if(field_mask & PP_FIELD_TYPE) {
        pp_flipper_update_or_insert_u32(ff, FF_TYPE, type);
    }
    return SubGhzProtocolStatusOk;
}

SubGhzProtocolStatus
    pp_write_display(FlipperFormat* ff, const char* protocol_name, const char* suffix) {
    if(!ff || !protocol_name || !suffix) {
        return SubGhzProtocolStatusError;
    }
    FuriString* display = furi_string_alloc();
    if(!display) {
        return SubGhzProtocolStatusError;
    }
    furi_string_printf(display, "%s - %s", protocol_name, suffix);
    flipper_format_rewind(ff);
    SubGhzProtocolStatus status =
        flipper_format_insert_or_update_string_cstr(ff, "Disp", furi_string_get_cstr(display)) ?
            SubGhzProtocolStatusOk :
            SubGhzProtocolStatusErrorParserOthers;
    furi_string_free(display);
    return status;
}

size_t pp_emit_merge(LevelDuration* up, size_t i, size_t cap, bool level, uint32_t us) {
    if(i > 0 && level_duration_get_level(up[i - 1]) == level) {
        uint32_t prev = level_duration_get_duration(up[i - 1]);
        up[i - 1] = level_duration_make(level, prev + us);
        return i;
    }
    if(i < cap) up[i++] = level_duration_make(level, us);
    return i;
}

size_t
    pp_emit_byte_manchester(LevelDuration* up, size_t i, size_t cap, uint8_t value, uint32_t te) {
    for(int8_t bit = 7; bit >= 0; bit--) {
        bool bit_value = ((value >> bit) & 1) != 0;
        i = pp_emit_manchester_bit(up, i, cap, bit_value, te);
    }
    return i;
}

size_t
    pp_emit_short_pairs(LevelDuration* up, size_t i, size_t cap, uint32_t te, size_t pair_count) {
    for(size_t p = 0; p < pair_count; p++) {
        i = pp_emit(up, i, cap, true, te);
        i = pp_emit(up, i, cap, false, te);
    }
    return i;
}

#if PROTOPIRATE_WITH_ENCODER

void pp_encoder_free(void* context) {
    ProtoPirateEncoderHeader* hdr = context;
    hdr->encoder.upload = NULL;
    hdr->encoder.size_upload = 0;
    free(hdr);
}

void pp_encoder_stop(void* context) {
    ProtoPirateEncoderHeader* hdr = context;
    hdr->encoder.is_running = false;
    hdr->encoder.front = 0;
}

LevelDuration pp_encoder_yield(void* context) {
    ProtoPirateEncoderHeader* hdr = context;
    if(hdr->encoder.repeat == 0 || !hdr->encoder.is_running || hdr->encoder.size_upload == 0) {
        hdr->encoder.is_running = false;
        return level_duration_reset();
    }
    LevelDuration ret = hdr->encoder.upload[hdr->encoder.front];
    if(++hdr->encoder.front == hdr->encoder.size_upload) {
        hdr->encoder.repeat--;
        hdr->encoder.front = 0;
    }
    return ret;
}

static LevelDuration* pp_shared_upload_buf = NULL;

LevelDuration* pp_shared_upload_buffer(void) {
    if(pp_shared_upload_buf == NULL) {
        pp_shared_upload_buf = malloc(PP_SHARED_UPLOAD_CAPACITY * sizeof(LevelDuration));
    }
    return pp_shared_upload_buf;
}

size_t pp_shared_upload_capacity(void) {
    return PP_SHARED_UPLOAD_CAPACITY;
}

void pp_shared_upload_release(void) {
    free(pp_shared_upload_buf);
    pp_shared_upload_buf = NULL;
}

void pp_encoder_buffer_ensure(void* context, size_t capacity) {
    ProtoPirateEncoderHeader* hdr = context;
    hdr->encoder.upload = pp_shared_upload_buffer();
    hdr->encoder.size_upload = capacity;
}

#endif

uint8_t pp_decoder_hash_blocks(void* context) {
    ProtoPirateDecoderHeader* hdr = context;
    return subghz_protocol_blocks_get_hash_data(
        &hdr->decoder, (hdr->decoder.decode_count_bit / 8U) + 1U);
}

void pp_decoder_free_default(void* context) {
    free(context);
}

bool pp_preset_name_is_custom_marker(const char* preset_name) {
    return preset_name && (!strcmp(preset_name, "Custom") || !strcmp(preset_name, "CUSTOM") ||
                           !strcmp(preset_name, "FuriHalSubGhzPresetCustom") ||
                           strstr(preset_name, "PresetCustom"));
}

const char* pp_get_short_preset_name(const char* preset_name) {
    if(!preset_name || preset_name[0] == '\0') return "AM650";

    if(strstr(preset_name, "Ook650") || strstr(preset_name, "OOK650")) return "AM650";
    if(strstr(preset_name, "Ook270") || strstr(preset_name, "OOK270")) return "AM270";
    if(strstr(preset_name, "2FSKDev238") || strstr(preset_name, "Dev238")) return "FM238";
    if(strstr(preset_name, "2FSKDev12K") || strstr(preset_name, "Dev12K")) return "FM12K";
    if(strstr(preset_name, "2FSKDev476") || strstr(preset_name, "Dev476")) return "FM476";
    if(pp_preset_name_is_custom_marker(preset_name)) return "Custom";

    if(!strcmp(preset_name, "AM650")) return "AM650";
    if(!strcmp(preset_name, "AM270")) return "AM270";
    if(!strcmp(preset_name, "FM238")) return "FM238";
    if(!strcmp(preset_name, "FM12K")) return "FM12K";
    if(!strcmp(preset_name, "FM476")) return "FM476";

    return preset_name;
}
