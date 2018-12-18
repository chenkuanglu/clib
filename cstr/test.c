#include "cstr.h"

int main(void)
{
    unsigned int word = 0x1234abcd;

    const char *str = "AB1234de";
    char buf[32];
    strlwr(buf, str, 32);
    printfd(CCL_BLUE "strlwr: %s > %s\n" CCL_END, str, buf);
    strupr(buf, str, 32);
    printfd(CCL_BLUE "strupr: %s > %s\n" CCL_END, str, buf);

    bin2hex(buf, 9, &word, 4);
    printfd(CCL_BLUE "bin2hex: 0x%08x > '%s'\n" CCL_END, word, buf);
    bin2hex(buf, 8, &word, 4);
    printfd(CCL_BLUE "bin2hex: 0x%008x > '%s'\n" CCL_END, word, buf);
    char *ahex = abin2hex(&word, 4);
    printfd(CCL_BLUE "abin2hex: 0x%008x > '%s'\n" CCL_END, word, ahex);
    free(ahex);
    word = 0;
    hex2bin(&word, 4, str, 8);
    printfd(CCL_BLUE "hex2bin: '%s' > 0x%08x\n" CCL_END, str, word);
    unsigned char *abin = ahex2bin(str, 0);
    printfd(CCL_BLUE "hex2bin: '%s' > 0x%08x\n" CCL_END, str, *(unsigned int *)abin);
    free(abin);
    
    return 0;
}

