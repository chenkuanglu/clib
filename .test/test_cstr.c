#include "cstr.h"

int main(void)
{
    const char str[] = "112233445566778899aabbCCddEEff";
    char buf[256] = {0};

    strlwr(buf, str, sizeof(buf));
    printfd(CCL_YELLOW "strlwr: %s > %s\n" CCL_END, str, buf);
    strupr(buf, str, sizeof(buf));
    printfd(CCL_YELLOW "strupr: %s > %s\n" CCL_END, str, buf);

    unsigned char *abin = (unsigned char *)ahex2bin(str);
    char *ahex = abin2hex(abin, 15);
    printfd(CCL_YELLOW "ahex2bin & abin2hex: '%s'\n" CCL_END, ahex);

    memswap(ahex, ahex, 30, 0);
    printfd(CCL_YELLOW "memswap all: '%s'\n" CCL_END, ahex);

    memswap(ahex, ahex, 30, 0);
    memswap(ahex, ahex, 30, 4);
    printfd(CCL_YELLOW "memswap 4: '%s'\n" CCL_END, ahex);

    free(abin);
    free(ahex);
    return 0;
}

