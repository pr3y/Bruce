#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pp_engine.h"
static char *execute(const char *op, const char *input) {
    return !strcmp(op,"catalog") ? pp_catalog_json() :
        !strcmp(op,"encode") ? pp_encode_json(input) : pp_decode_json(input);
}
int main(int argc,char **argv){
    if(argc>1 && !strcmp(argv[1],"batch")) {
        char header[64], op[16], input[262146];
        size_t n;
        while(fgets(header,sizeof(header),stdin)) {
            if(sscanf(header,"%15s %zu",op,&n)!=2 || n>=sizeof(input)) return 3;
            if(fread(input,1,n,stdin)!=n) return 4;
            input[n]=0;
            char *out=execute(op,input);
            if(!out) return 2;
            puts(out);
            pp_response_free(out);
            fflush(stdout);
        }
        return 0;
    }
    char input[262146];size_t n=fread(input,1,sizeof(input)-1,stdin);input[n]=0;
    char *out=execute(argc>1?argv[1]:"decode",input);
    if(!out) return 2;
    puts(out);
    pp_response_free(out);
    return 0;
}
