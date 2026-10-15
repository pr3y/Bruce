#pragma once
#ifdef __cplusplus
extern "C" {
#endif
/* Responses are heap-owned JSON; release with pp_response_free(). No RF I/O. */
char *pp_catalog_json(void);
char *pp_decode_json(const char *sub_file);
char *pp_encode_json(const char *sub_file);
void pp_response_free(char *response);
#ifdef __cplusplus
}
#endif
