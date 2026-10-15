#define PP_PLATFORM_IMPL
#include "pp_platform.h"
#include "protocols/keeloq_common.h"
#include <ctype.h>
#include <errno.h>

static char *pp_copy(const char *s, size_t n) { char *p=calloc(n+1,1); if(p) memcpy(p,s,n); return p; }
FuriString *furi_string_alloc(void) { return calloc(1,sizeof(FuriString)); }
void furi_string_free(FuriString *s) { if(s) { free(s->data); free(s); } }
void furi_string_reset(FuriString *s) { s->len=0; if(s->data) s->data[0]=0; }
const char *furi_string_get_cstr(const FuriString *s) { return s&&s->data?s->data:""; }
static bool pp_reserve(FuriString *s,size_t n) {
    if(n>1048576) pp_memory_fail();
    if(n<=s->cap) return true;
    size_t capacity=s->cap?s->cap:64;
    while(capacity<n)capacity*=2;
    char *p=realloc(s->data,capacity); if(!p) return false; s->data=p;s->cap=capacity;return true;
}
void furi_string_set(FuriString *s,const char *v) {
    size_t n=strlen(v); if(pp_reserve(s,n+1)) { memcpy(s->data,v,n+1);s->len=n; }
}
static void pp_vprintf(FuriString *s,bool append,const char *fmt,va_list ap) {
    va_list aq;va_copy(aq,ap);int n=vsnprintf(NULL,0,fmt,aq);va_end(aq);
    if(n<0)return;size_t off=append?s->len:0;
    if(pp_reserve(s,off+(size_t)n+1)){vsnprintf(s->data+off,n+1,fmt,ap);s->len=off+n;}
}
void furi_string_printf(FuriString *s,const char *fmt,...) {va_list ap;va_start(ap,fmt);pp_vprintf(s,false,fmt,ap);va_end(ap);}
void furi_string_cat_printf(FuriString *s,const char *fmt,...) {va_list ap;va_start(ap,fmt);pp_vprintf(s,true,fmt,ap);va_end(ap);}
int furi_string_cmp_str(const FuriString *s,const char *v){return strcmp(furi_string_get_cstr(s),v);}
bool furi_string_equal(const FuriString *s,const char *v){return !strcmp(furi_string_get_cstr(s),v);}

void pp_ff_clear(FlipperFormat *ff){for(size_t i=0;i<ff->count;i++){free(ff->fields[i].key);free(ff->fields[i].value);}memset(ff,0,sizeof(*ff));}
const char *pp_ff_get(const FlipperFormat *ff,const char *key){for(size_t i=0;i<ff->count;i++)if(!strcmp(key,ff->fields[i].key))return ff->fields[i].value;return NULL;}
bool flipper_format_write_string_cstr(FlipperFormat *ff,const char *key,const char *v){
    if(!ff||!key||!v||strlen(key)>64||strlen(v)>4096)return false;
    size_t i=0;while(i<ff->count&&strcmp(key,ff->fields[i].key))i++;
    if(i==64)return false;char *copy=pp_copy(v,strlen(v));if(!copy)return false;
    if(i==ff->count){ff->fields[i].key=pp_copy(key,strlen(key));if(!ff->fields[i].key){free(copy);return false;}ff->count++;}
    free(ff->fields[i].value);ff->fields[i].value=copy;return true;
}
bool flipper_format_rewind(FlipperFormat *ff){return ff!=NULL;}
bool flipper_format_delete_key(FlipperFormat *ff,const char *key){for(size_t i=0;i<ff->count;i++)if(!strcmp(ff->fields[i].key,key)){free(ff->fields[i].key);free(ff->fields[i].value);memmove(ff->fields+i,ff->fields+i+1,(ff->count-i-1)*sizeof(PPField));ff->count--;return true;}return false;}
bool flipper_format_read_string(FlipperFormat *ff,const char *key,FuriString *out){const char *s=pp_ff_get(ff,key);if(!s)return false;furi_string_set(out,s);return true;}
bool flipper_format_read_uint32(FlipperFormat *ff,const char *key,uint32_t *out,size_t count){
    const char *s=pp_ff_get(ff,key);if(!s)return false;
    for(size_t i=0;i<count;i++){while(isspace((unsigned char)*s))s++;if(!isdigit((unsigned char)*s))return false;char *end;errno=0;unsigned long n=strtoul(s,&end,10);if(errno||n>UINT32_MAX||end==s)return false;out[i]=(uint32_t)n;s=end;}
    while(isspace((unsigned char)*s))s++;return *s==0;
}
bool flipper_format_write_uint32(FlipperFormat *ff,const char *key,const uint32_t *v,size_t n){
    if(n>32)return false;char buf[384];size_t used=0;for(size_t i=0;i<n;i++)used+=snprintf(buf+used,sizeof(buf)-used,"%s%lu",i?" ":"",(unsigned long)v[i]);return flipper_format_write_string_cstr(ff,key,buf);
}
bool flipper_format_write_hex(FlipperFormat *ff,const char *key,const uint8_t *v,size_t n){
    if(n>256)return false;char buf[769];size_t used=0;for(size_t i=0;i<n;i++)used+=snprintf(buf+used,sizeof(buf)-used,"%s%02X",i?" ":"",v[i]);return flipper_format_write_string_cstr(ff,key,buf);
}
static int pp_nibble(char c){if(c>='0'&&c<='9')return c-'0';c=(char)tolower((unsigned char)c);return c>='a'&&c<='f'?c-'a'+10:-1;}
bool flipper_format_read_hex(FlipperFormat *ff,const char *key,uint8_t *out,size_t n){
    const char *s=pp_ff_get(ff,key);if(!s)return false;
    for(size_t i=0;i<n;i++){while(isspace((unsigned char)*s))s++;if(!s[0]||!s[1])return false;int a=pp_nibble(s[0]),b=pp_nibble(s[1]);if(a<0||b<0)return false;out[i]=(a<<4)|b;s+=2;}
    while(isspace((unsigned char)*s))s++;return !*s;
}
bool pp_ff_parse(FlipperFormat *ff,const char *text){
    pp_ff_clear(ff);if(!text||strlen(text)>262144)return false;
    const char *p=text;while(*p){const char *e=strchr(p,'\n');if(!e)e=p+strlen(p);const char *colon=memchr(p,':',e-p);
        if(colon&&*p!='#'){size_t kn=colon-p;while(kn&&isspace((unsigned char)p[kn-1]))kn--;const char *v=colon+1;while(v<e&&isspace((unsigned char)*v))v++;const char *ve=e;while(ve>v&&isspace((unsigned char)ve[-1]))ve--;
            if(kn>64)return false;char key[65];memcpy(key,p,kn);key[kn]=0;
            if(strcmp(key,"RAW_Data")&&strcmp(key,"Data_RAW")){if(pp_ff_get(ff,key))return false;char *value=pp_copy(v,ve-v);if(!value)return false;bool ok=flipper_format_write_string_cstr(ff,key,value);free(value);if(!ok)return false;}}
        p=*e?e+1:e;
    }return true;
}
SubGhzProtocolStatus subghz_block_generic_serialize(SubGhzBlockGeneric *g,FlipperFormat *ff,SubGhzRadioPreset *p){
    pp_ff_clear(ff);uint32_t bits=g->data_count_bit,version=1;uint8_t key[8];for(int i=0;i<8;i++)key[7-i]=(uint8_t)(g->data>>(8*i));
    if(!flipper_format_write_string_cstr(ff,"Filetype","Flipper SubGhz Key File")||!flipper_format_write_uint32(ff,"Version",&version,1)||!flipper_format_write_uint32(ff,"Frequency",&p->frequency,1)||!flipper_format_write_string_cstr(ff,"Preset",furi_string_get_cstr(p->name))||!flipper_format_write_string_cstr(ff,"Protocol",g->protocol_name)||!flipper_format_write_uint32(ff,"Bit",&bits,1)||!flipper_format_write_hex(ff,"Key",key,8))return SubGhzProtocolStatusError;
    if(p->data&&p->data_size){flipper_format_write_string_cstr(ff,"Custom_preset_module","CC1101");flipper_format_write_hex(ff,"Custom_preset_data",p->data,p->data_size);}return SubGhzProtocolStatusOk;
}
SubGhzProtocolStatus subghz_block_generic_deserialize(SubGhzBlockGeneric *g,FlipperFormat *ff){
    uint32_t bits;uint8_t key[8];if(!flipper_format_read_uint32(ff,"Bit",&bits,1)||bits>UINT16_MAX)return SubGhzProtocolStatusErrorParserBitCount;
    if(!flipper_format_read_hex(ff,"Key",key,8))return SubGhzProtocolStatusErrorParserKey;
    g->data=0;for(int i=0;i<8;i++)g->data=(g->data<<8)|key[i];g->data_count_bit=bits;return SubGhzProtocolStatusOk;
}
SubGhzProtocolStatus subghz_block_generic_deserialize_check_count_bit(SubGhzBlockGeneric *g,FlipperFormat *ff,uint16_t bits){SubGhzProtocolStatus r=subghz_block_generic_deserialize(g,ff);return r?r:g->data_count_bit==bits?SubGhzProtocolStatusOk:SubGhzProtocolStatusErrorValueBitCount;}
SubGhzKeystore *subghz_environment_get_keystore(SubGhzEnvironment *e){return e?e->keystore:NULL;}
SubGhzKeyArray_t *subghz_keystore_get_data(SubGhzKeystore *k){return k?&k->data:NULL;}
/* Device-encrypted Flipper keystores cannot be treated as plaintext on ESP32. */
bool subghz_keystore_raw_get_data(const char *name,size_t off,uint8_t *out,size_t n){(void)name;(void)off;(void)out;(void)n;return false;}
