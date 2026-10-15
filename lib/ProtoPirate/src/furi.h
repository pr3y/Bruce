#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <assert.h>
#define EXT_PATH(x) x
#define APP_ASSETS_PATH(x) x
#define COUNT_OF(x) (sizeof(x)/sizeof((x)[0]))
#define UNUSED(x) (void)(x)
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#include "pp_memory.h"
#define furi_crash(...) pp_memory_fail()
#define FURI_LOG_E(...)
#define FURI_LOG_W(...)
#define FURI_LOG_I(...)
#define FURI_LOG_D(...)
#define FURI_LOG_T(...)
#define furi_assert(x) do { if(!(x)) pp_memory_fail(); } while(0)
#define furi_check(x) furi_assert(x)
typedef struct { char *data; size_t len, cap; } FuriString;
FuriString *furi_string_alloc(void);
void furi_string_free(FuriString *s);
void furi_string_reset(FuriString *s);
const char *furi_string_get_cstr(const FuriString *s);
void furi_string_set(FuriString *s, const char *v);
void furi_string_printf(FuriString *s, const char *fmt, ...);
void furi_string_cat_printf(FuriString *s, const char *fmt, ...);
int furi_string_cmp_str(const FuriString *s, const char *v);
bool furi_string_equal(const FuriString *s, const char *v);
/* Flipper allocations are zero initialized. Keep this behavior in the port. */
#define malloc(n) pp_malloc(n)
#define calloc(n,s) pp_calloc(n,s)
#define realloc(p,n) pp_realloc(p,n)
#define free(p) pp_free(p)
