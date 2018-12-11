#include <stdio.h>
#include "iniparser.h"

int main(void)
{
    dictionary *dict = NULL;

    dict = iniparser_load("cfg.ini");
    if (dict == NULL) {
        return -1;
    }

    const char* str = dictionary_get(dict, "nonce:nonce_hit", "none");
    printf("get nonce_hit str = '%s'\n", str);

    iniparser_freedict(dict);
    return 0;
}
