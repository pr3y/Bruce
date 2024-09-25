#ifndef __LGFX_QOI_H__
#define __LGFX_QOI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Main Qoi object
typedef struct _qoi_t qoi_t;
// QOI image description header
typedef struct _qoi_desc_t qoi_desc_t;

// Callback signatures
typedef uint32_t (*lgfx_qoi_read_callback_t)(void *user_data, uint8_t *buf, uint32_t len);
typedef void (*lgfx_qoi_draw_callback_t)(void *user_data, uint32_t x, uint32_t y, uint_fast8_t div_x, size_t len, const uint8_t* argb);


typedef uint8_t *(*lgfx_qoi_encoder_get_row_func)(uint8_t *lineBuffer, int flip, int w, int h, int y, void *qoienc);
// basic buffer/stream writer signature
typedef int (*lfgx_qoi_writer_func)(uint8_t* buf, size_t buf_len);

// ---------------------
// Basic read interfaces
// ---------------------
qoi_t *lgfx_qoi_new();

int lgfx_qoi_prepare(qoi_t *qoi, lgfx_qoi_read_callback_t read_cb, void* user_data);
int lgfx_qoi_decomp(qoi_t *qoi, lgfx_qoi_draw_callback_t draw_cb);

void lgfx_qoi_destroy(qoi_t *qoi);
void lgfx_qoi_reset(qoi_t *qoi);

uint32_t lgfx_qoi_get_width(qoi_t *qoi);
uint32_t lgfx_qoi_get_height(qoi_t *qoi);

qoi_desc_t *lgfx_qoi_get_desc(qoi_t *qoi);

// ----------------------
// Basic write interfaces
// ----------------------

// write to buffer (will malloc)
void  *lgfx_qoi_encoder_write_fb(const void *lineBuffer, int w, int h, int num_chans, size_t *out_len, int flip, lgfx_qoi_encoder_get_row_func cb, void *qoienc);
// write to callback (falls back to malloc if none provided)
size_t lgfx_qoi_encoder_write_cb(const void *lineBuffer, uint32_t buflen, int w, int h, int num_chans, int flip, lgfx_qoi_encoder_get_row_func get_row, lfgx_qoi_writer_func write_bytes, void *qoienc);
// encode
size_t lgfx_qoi_encode(const void *lineBuffer, const qoi_desc_t *desc, int flip, lgfx_qoi_encoder_get_row_func get_row, lfgx_qoi_writer_func write_bytes, void *qoienc);


#ifdef __cplusplus
}
#endif

#endif /* __LGFX_QOI_H__ */
