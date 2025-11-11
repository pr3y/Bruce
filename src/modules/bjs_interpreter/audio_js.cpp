#if !defined(LITE_VERSION) && !defined(DISABLE_INTERPRETER)
#include "audio_js.h"

#include "helpers_js.h"

duk_ret_t putPropAudioFunctions(duk_context *ctx, duk_idx_t obj_idx, uint8_t magic) {
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "playFile", native_playAudioFile, 1, magic);
    bduk_put_prop_c_lightfunc(ctx, obj_idx, "tone", native_tone, 3, magic);
    return 0;
}

duk_ret_t registerAudio(duk_context *ctx) {
    bduk_register_c_lightfunc(ctx, "playAudioFile", native_playAudioFile, 1);
    bduk_register_c_lightfunc(ctx, "tone", native_tone, 3);
    return 0;
}

duk_ret_t native_playAudioFile(duk_context *ctx) {
    // usage: playAudioFile(filename : string);
    // returns: bool==true on success, false on any error
    // MEMO: no need to check for board support (done in parseSerialCommand)
    bool r = parseSerialCommand("music_player " + String(duk_to_string(ctx, 0)));
    duk_push_boolean(ctx, r);
    return 1;
}

duk_ret_t native_tone(duk_context *ctx) {
    // usage: tone(frequency: number);
    // usage: tone(frequency: number, duration: number, nonBlocking: boolean);
    if (!bruceConfig.soundEnabled) return 0;

#if defined(BUZZ_PIN)
    tone(BUZZ_PIN, duk_get_uint_default(ctx, 0, 500), duk_get_uint_default(ctx, 1, 1000));
#elif defined(HAS_NS4168_SPKR)
    //  alt. implementation using the speaker
    if (!duk_get_int_default(ctx, 2, 0)) {
        parseSerialCommand("tone " + String(duk_to_int(ctx, 0)) + " " + String(duk_to_int(ctx, 1)));
    }
#endif
    return 0;
}

#endif
