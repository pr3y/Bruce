#pragma once
#include <furi.h>
typedef struct { char *key, *value; } PPField;
typedef struct { PPField fields[64]; size_t count; } FlipperFormat;
void pp_ff_clear(FlipperFormat *ff);
bool pp_ff_parse(FlipperFormat *ff, const char *text);
const char *pp_ff_get(const FlipperFormat *ff, const char *key);
bool flipper_format_rewind(FlipperFormat *ff);
bool flipper_format_read_string(FlipperFormat *ff, const char *key, FuriString *out);
bool flipper_format_read_uint32(FlipperFormat *ff, const char *key, uint32_t *out, size_t count);
bool flipper_format_read_hex(FlipperFormat *ff, const char *key, uint8_t *out, size_t count);
bool flipper_format_write_string_cstr(FlipperFormat *ff, const char *key, const char *value);
bool flipper_format_write_uint32(FlipperFormat *ff, const char *key, const uint32_t *value, size_t count);
bool flipper_format_write_hex(FlipperFormat *ff, const char *key, const uint8_t *value, size_t count);
bool flipper_format_delete_key(FlipperFormat *ff, const char *key);
#define flipper_format_insert_or_update_string_cstr flipper_format_write_string_cstr
#define flipper_format_insert_or_update_uint32 flipper_format_write_uint32
#define flipper_format_insert_or_update_hex flipper_format_write_hex
#define flipper_format_update_uint32 flipper_format_write_uint32
#define flipper_format_update_hex flipper_format_write_hex
