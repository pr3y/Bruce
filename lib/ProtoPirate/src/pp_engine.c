#define PP_PLATFORM_IMPL
#include "pp_engine.h"
#include "pp_platform.h"
#include "protocols/keeloq_common.h"
#include "protocols/protocol_items.h"
#include <ctype.h>
#include <errno.h>
#include "pp_registry.h"
#include "defines.h"

#if !PROTOPIRATE_WITH_ENCODER
#error "The Bruce ProtoPirate application requires emulation enabled by default"
#endif

#define PP_MAX_INPUT 262144U
#define PP_MAX_PULSES 16384U
#define PP_MAX_RESULTS 16U
static SubGhzKeystore empty_keystore;
static SubGhzEnvironment environment={&empty_keystore};
static char *take(FuriString *s){char *p=s->data;s->data=NULL;furi_string_free(s);return p;}
static void quote(FuriString *out,const char *s){
    furi_string_cat_printf(out,"\"");
    for(const unsigned char *p=(const unsigned char *)s;*p;p++){
        if(*p=='"'||*p=='\\')furi_string_cat_printf(out,"\\%c",*p);
        else if(*p<32)furi_string_cat_printf(out,"\\u%04x",*p);
        else furi_string_cat_printf(out,"%c",*p);
    }furi_string_cat_printf(out,"\"");
}
static char *error(const char *msg){FuriString *s=furi_string_alloc();furi_string_printf(s,"{\"ok\":false,\"error\":");quote(s,msg);furi_string_cat_printf(s,"}");return take(s);}
static FuriString *dump(const FlipperFormat *ff){FuriString *s=furi_string_alloc();for(size_t i=0;i<ff->count;i++)furi_string_cat_printf(s,"%s: %s\n",ff->fields[i].key,ff->fields[i].value);return s;}
static const SubGhzProtocol *find_protocol(const char *name){
    if(!name)return NULL;const char *canonical=protopirate_protocol_catalog_canonical_name(name);
    if(!canonical)canonical=name;
    for(size_t i=0;i<COUNT_OF(protocols);i++)if(!strcmp(protocols[i]->name,canonical))return protocols[i];return NULL;
}
static bool encoder_available(const SubGhzProtocol *p){return p->encoder&&p->encoder->alloc&&p->encoder->deserialize&&p->encoder->yield&&p->encoder->free;}
static char *catalog_impl(void){
    FuriString *out=furi_string_alloc();furi_string_printf(out,"{\"ok\":true,\"api\":1,\"version\":\"4.0-bruce.1-dev\",\"development\":true,\"emulationEnabled\":true,\"upstream\":\"4c9afe58f881a28ddb51015f63b84578e101068d\",\"keyStoreLoaded\":false,\"protocols\":[");
    for(size_t i=0;i<COUNT_OF(protocols);i++){if(i)furi_string_cat_printf(out,",");furi_string_cat_printf(out,"{\"name\":");quote(out,protocols[i]->name);furi_string_cat_printf(out,",\"encoder\":%s}",encoder_available(protocols[i])?"true":"false");}
    furi_string_cat_printf(out,"]}");return take(out);
}
typedef struct {FuriString *json;SubGhzRadioPreset preset;const SubGhzProtocol *protocol;const char *source;unsigned count;bool failed;uint64_t signatures[PP_MAX_RESULTS];} DecodeContext;
// The original decoder interface exposes checksum status in its display text.
// Reject only explicit failures; absence of a failure is not proof of validity.
static bool checksum_failed(const char *description){
    // Match status tokens, never arbitrary substrings: valid hexadecimal keys
    // can themselves contain "BAD" or "FAIL"-like text.
    return strstr(description,"CRC:") && (strstr(description," WRONG") ||
        strstr(description,"CRC:BAD") || strstr(description," - BAD") ||
        strstr(description,"[BAD]") || strstr(description,"CRC:FAIL") ||
        strstr(description,"[FAIL]") || strstr(description,"[ERR]"));
}
static void decoded(SubGhzProtocolDecoderBase *base,void *opaque){
    DecodeContext *ctx=opaque;if(ctx->count>=PP_MAX_RESULTS)return;
    FlipperFormat ff={0};FuriString *description=furi_string_alloc();
    SubGhzProtocolStatus r=ctx->protocol->decoder->serialize(base,&ff,&ctx->preset);
    if(r==SubGhzProtocolStatusOk){
        ctx->protocol->decoder->get_string(base,description);FuriString *data=dump(&ff);
        // Preserve every field in an imported decoded file. Some upstream
        // deserializers restore only Key/Bit and cannot reserialize other fields.
        if(ctx->source)furi_string_set(data,ctx->source);
        bool invalid=checksum_failed(furi_string_get_cstr(description));
        uint64_t signature=1469598103934665603ULL;
        for(const unsigned char *c=(const unsigned char *)furi_string_get_cstr(data);*c;c++)signature=(signature^*c)*1099511628211ULL;
        for(unsigned n=0;n<ctx->count;n++)if(ctx->signatures[n]==signature){furi_string_free(data);furi_string_free(description);pp_ff_clear(&ff);return;}
        ctx->signatures[ctx->count]=signature;
        if(ctx->count)furi_string_cat_printf(ctx->json,",");
        furi_string_cat_printf(ctx->json,"{\"protocol\":");quote(ctx->json,ctx->protocol->name);
        furi_string_cat_printf(ctx->json,",\"encoder\":%s,\"validation\":\"%s\",\"data\":",encoder_available(ctx->protocol)&&!invalid?"true":"false",invalid?"failed":"not-verified");quote(ctx->json,furi_string_get_cstr(data));
        furi_string_cat_printf(ctx->json,",\"text\":");quote(ctx->json,furi_string_get_cstr(description));furi_string_cat_printf(ctx->json,"}");
        ctx->count++;furi_string_free(data);
    }else ctx->failed=true;
    furi_string_free(description);pp_ff_clear(&ff);
}
static bool read_pulses(const char *text,int32_t *pulses,size_t *count){
    *count=0;const char *p=text;
    while(*p){const char *e=strchr(p,'\n');if(!e)e=p+strlen(p);size_t prefix=0;
        if(e-p>=9&&!strncmp(p,"RAW_Data:",9))prefix=9;
        if(e-p>=9&&!strncmp(p,"Data_RAW:",9))prefix=9;
        if(prefix){const char *s=p+prefix;while(s<e){while(s<e&&isspace((unsigned char)*s))s++;if(s==e)break;char *end;errno=0;long n=strtol(s,&end,10);
            if(end==s||end>e||errno||!n||n < -1000000L||n>1000000L||(end<e&&!isspace((unsigned char)*end))||*count>=PP_MAX_PULSES)return false;
            if(*count && ((pulses[*count-1]>0)==(n>0))){
                int64_t combined=(int64_t)pulses[*count-1]+n;
                if(combined>1000000||combined < -1000000)return false;
                pulses[*count-1]=(int32_t)combined;
            }else pulses[(*count)++]=(int32_t)n;
            s=end;}}
        p=*e?e+1:e;
    }return *count>0;
}
static bool read_preset(FlipperFormat *ff,SubGhzRadioPreset *preset){
    const char *name=pp_ff_get(ff,"Preset");if(!name)return false;
    if(!flipper_format_read_uint32(ff,"Frequency",&preset->frequency,1))return false;
    uint32_t f=preset->frequency;if(!((f>=300000000&&f<=348000000)||(f>=387000000&&f<=464000000)||(f>=779000000&&f<=928000000)))return false;
    const char *custom=pp_ff_get(ff,"Custom_preset_data");
    if(custom){
        size_t digits=0;for(const char *p=custom;*p;p++){if(isxdigit((unsigned char)*p))digits++;else if(!isspace((unsigned char)*p))return false;}
        if(!digits||digits%2||digits>512)return false;
        preset->data_size=digits/2;preset->data=calloc(preset->data_size,1);
        if(!flipper_format_read_hex(ff,"Custom_preset_data",preset->data,preset->data_size))return false;
        const char *module=pp_ff_get(ff,"Custom_preset_module");if(module&&strcmp(module,"CC1101"))return false;
    }else if(!strcmp(name,"FuriHalSubGhzPresetCustom"))return false;
    preset->name=furi_string_alloc();furi_string_set(preset->name,name);return true;
}
static char *decode_impl(const char *input){
    if(!input||strlen(input)>PP_MAX_INPUT)return error("File too large");
    FlipperFormat ff={0};if(!pp_ff_parse(&ff,input)){pp_ff_clear(&ff);return error("Malformed or duplicate fields");}
    DecodeContext ctx={0};if(!read_preset(&ff,&ctx.preset)){pp_ff_clear(&ff);return error("Missing or invalid frequency/preset");}
    const char *name=pp_ff_get(&ff,"Protocol");ctx.json=furi_string_alloc();furi_string_printf(ctx.json,"{\"ok\":true,\"frames\":[");
    if(name&&strcmp(name,"RAW")&&strcmp(name,"BinRAW")){
        const SubGhzProtocol *proto=find_protocol(name);
        if(proto&&proto->decoder&&proto->decoder->deserialize){void *decoder=proto->decoder->alloc(&environment);proto->decoder->reset(decoder);
            if(proto->decoder->deserialize(decoder,&ff)==SubGhzProtocolStatusOk){ctx.protocol=proto;ctx.source=input;decoded(decoder,&ctx);}(proto->decoder->free)(decoder);}
    }else{
        int32_t *pulses=calloc(PP_MAX_PULSES,sizeof(int32_t));size_t count=0;
        if(!pulses||!read_pulses(input,pulses,&count)){free(pulses);furi_string_free(ctx.json);furi_string_free(ctx.preset.name);pp_ff_clear(&ff);return error("Malformed, empty or oversized RAW data");}
        for(size_t i=0;i<COUNT_OF(protocols)&&ctx.count<PP_MAX_RESULTS;i++){
            ctx.protocol=protocols[i];const SubGhzProtocolDecoder *ops=ctx.protocol->decoder;
            if(!ops||!ops->alloc||!ops->feed||!ops->reset||!ops->serialize||!ops->get_string)continue;
            void *decoder=ops->alloc(&environment);ops->reset(decoder);
            SubGhzProtocolDecoderBase *base=decoder;base->callback=decoded;base->context=&ctx;
            for(size_t n=0;n<count&&ctx.count<PP_MAX_RESULTS;n++)ops->feed(decoder,pulses[n]>0,(uint32_t)(pulses[n]>0?pulses[n]:-pulses[n]));
            (ops->free)(decoder);
        }free(pulses);
    }
    furi_string_cat_printf(ctx.json,"],\"limitReached\":%s,\"serializationError\":%s}",ctx.count>=PP_MAX_RESULTS?"true":"false",ctx.failed?"true":"false");
    furi_string_free(ctx.preset.name);pp_ff_clear(&ff);return take(ctx.json);
}
static char *encode_impl(const char *input){
    if(!input||strlen(input)>16384)return error("Invalid decoded file size");
    FlipperFormat ff={0};if(!pp_ff_parse(&ff,input)){pp_ff_clear(&ff);return error("Malformed or duplicate fields");}
    const SubGhzProtocol *p=find_protocol(pp_ff_get(&ff,"Protocol"));
    if(!p||!encoder_available(p)){pp_ff_clear(&ff);return error("Upstream has no encoder for this protocol");}
    // Match the JS app's bounded default. Upstream defaults of ten complete
    // multi-burst uploads can exceed this API's ten-second transmission limit.
    if(!pp_ff_get(&ff,"Repeat")){uint32_t repeat=3;flipper_format_write_uint32(&ff,"Repeat",&repeat,1);}
    SubGhzRadioPreset preset={0};if(!read_preset(&ff,&preset)){pp_ff_clear(&ff);return error("Missing or invalid frequency/preset");}
    const SubGhzProtocolEncoder *ops=p->encoder;void *encoder=ops->alloc(&environment);
    SubGhzProtocolStatus status=ops->deserialize(encoder,&ff);
    if(status!=SubGhzProtocolStatusOk){(ops->free)(encoder);pp_shared_upload_release();pp_ff_clear(&ff);furi_string_free(preset.name);return error("Encoder rejected fields, framing or unavailable key material");}
    FuriString *raw=furi_string_alloc();furi_string_printf(raw,"Filetype: Flipper SubGhz RAW File\nVersion: 1\nFrequency: %lu\nPreset: %s\nProtocol: RAW\n",(unsigned long)preset.frequency,furi_string_get_cstr(preset.name));
    const char *custom=pp_ff_get(&ff,"Custom_preset_data");if(custom)furi_string_cat_printf(raw,"Custom_preset_module: CC1101\nCustom_preset_data: %s\n",custom);
    size_t count=0;uint64_t duration=0;bool complete=false;
    while(count<PP_MAX_PULSES){LevelDuration ld=ops->yield(encoder);if(level_duration_is_reset(ld)){complete=true;break;}uint32_t us=level_duration_get_duration(ld);
        if(!us||us>1000000)break;duration+=us;if(duration>10000000)break;
        if(count%512==0)furi_string_cat_printf(raw,"%sRAW_Data:",count?"\n":"");
        furi_string_cat_printf(raw," %s%lu",level_duration_get_level(ld)?"":"-",(unsigned long)us);count++;
    }furi_string_cat_printf(raw,"\n");ops->stop(encoder);(ops->free)(encoder);pp_shared_upload_release();furi_string_free(preset.name);
    if(!complete||!count){furi_string_free(raw);pp_ff_clear(&ff);return error("Encoder output exceeds duration/pulse limits or is empty");}
    FuriString *updated=dump(&ff),*json=furi_string_alloc();furi_string_printf(json,"{\"ok\":true,\"pulses\":%lu,\"raw\":",(unsigned long)count);quote(json,furi_string_get_cstr(raw));furi_string_cat_printf(json,",\"updated\":");quote(json,furi_string_get_cstr(updated));furi_string_cat_printf(json,"}");
    pp_ff_clear(&ff);furi_string_free(raw);furi_string_free(updated);return take(json);
}
void pp_response_free(char *response){free(response);}

// Every call owns an allocation scope. Malformed input and OOM unwind C-only
// protocol code, release the shared slab, and return an error to JS.
#define PP_RUN(expression) \
    pp_memory_begin(); \
    if(setjmp(*pp_memory_jump())) { \
        pp_shared_upload_release();pp_memory_finish(NULL);return pp_memory_error(); \
    } \
    char *result=(expression);pp_shared_upload_release();pp_memory_finish(result);return result
char *pp_catalog_json(void){PP_RUN(catalog_impl());}
char *pp_decode_json(const char *input){PP_RUN(decode_impl(input));}
char *pp_encode_json(const char *input){PP_RUN(encode_impl(input));}
