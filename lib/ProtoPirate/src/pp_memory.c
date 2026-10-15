#include "pp_memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
typedef struct {void *p;size_t n;} Allocation;
static Allocation allocated[512];
static size_t used,total;
static int active;
static jmp_buf escape;
void pp_memory_begin(void){used=0;total=0;active=1;}
jmp_buf *pp_memory_jump(void){return &escape;}
void pp_memory_fail(void){if(active)longjmp(escape,1);abort();}
static void track(void *p,size_t n){if(!active)return;if(!p||used==512||n>1048576-total){free(p);pp_memory_fail();}allocated[used++]=(Allocation){p,n};total+=n;}
void *pp_malloc(size_t n){if(!n)n=1;if(active&&(used==512||n>1048576-total))pp_memory_fail();void *p=calloc(1,n);if(!p)pp_memory_fail();track(p,n);return p;}
void *pp_calloc(size_t n,size_t size){if(size&&n>SIZE_MAX/size)pp_memory_fail();return pp_malloc(n*size);}
void pp_free(void *p){if(!p)return;if(active){for(size_t i=0;i<used;i++)if(allocated[i].p==p){total-=allocated[i].n;allocated[i]=allocated[--used];break;}}free(p);}
void *pp_realloc(void *p,size_t n){
    if(!p)return pp_malloc(n);size_t i=0;while(i<used&&allocated[i].p!=p)i++;
    size_t old=active&&i<used?allocated[i].n:0;
    if(active&&n>1048576-total+old)pp_memory_fail();
    void *q=realloc(p,n?n:1);if(!q)pp_memory_fail();
    if(active&&i<used){allocated[i].p=q;allocated[i].n=n;total=total-old+n;}else track(q,n);
    return q;
}
void pp_memory_finish(void *keep){for(size_t i=0;i<used;i++)if(allocated[i].p!=keep)free(allocated[i].p);used=total=0;active=0;}
char *pp_memory_error(void){const char *s="{\"ok\":false,\"error\":\"Protocol input rejected or memory limit reached\"}";size_t n=strlen(s)+1;char *p=malloc(n);if(p)memcpy(p,s,n);return p;}
