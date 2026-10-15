#pragma once
#include <stddef.h>
#include <setjmp.h>
void pp_memory_begin(void);
jmp_buf *pp_memory_jump(void);
void pp_memory_finish(void *keep);
void pp_memory_fail(void);
void *pp_malloc(size_t n);
void *pp_calloc(size_t n,size_t size);
void *pp_realloc(void *p,size_t n);
void pp_free(void *p);
char *pp_memory_error(void);
