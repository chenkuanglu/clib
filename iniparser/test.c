#include <stdio.h>
#include "iniparser.h"

int main(void)
{
    dictionary *dict = NULL;

    dict = iniparser_load("cfg.ini");
    if (dict == NULL) {
        return -1;
    }

    unsigned addr = iniparser_getlongint(dict, "program:program_addr", -1);
    printf("get program addr = 0x%08x\n", addr);

    const char* str = iniparser_getstring(dict, "nonce:nonce_hit", "none");
    printf("get nonce_hit str = '%s'\n", str);

    iniparser_freedict(dict);
    return 0;
}
