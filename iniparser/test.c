#include <stdio.h>
#include "iniparser.h"

#define CFG_MAKE_KEY(sec, key)      CFG_CONCAT_STR(sec, key)
#define CFG_CONCAT_STR(sec, key)    sec##_##key

#define __CFG_STRING(x)             (#x)
#define CFG_STRING(x)               __CFG_STRING(x)
#define CFG_PROGRAM_ADDR            CFG_STRING(CFG_MAKE_KEY(program, address))

int main(void)
{
    dictionary *dict = NULL;

    dict = iniparser_load("cfg.ini");
    if (dict == NULL) {
        return -1;
    }

    const char *s= iniparser_getstring(dict, "program:address", "1");
    printf("get program addr = 0x%s\n", s);

    const char* str = iniparser_getstring(dict, "nonce:nonce_hit", "none");
    printf("get nonce_hit str = '%s'\n", str);

    printf("concat = '%s'\n", CFG_PROGRAM_ADDR);

    iniparser_freedict(dict);
    return 0;
}
