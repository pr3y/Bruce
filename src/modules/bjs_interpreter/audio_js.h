#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#ifndef __AUDIO_JS_H__
#define __AUDIO_JS_H__

#include <duktape.h>

duk_ret_t putPropAudioFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic);
duk_ret_t registerAudio(duk_context *ctx);

duk_ret_t native_playAudioFile(duk_context *ctx);
duk_ret_t native_tone(duk_context *ctx);

#endif
#endif
