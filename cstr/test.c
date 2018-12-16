#include "cstr.h"

int main(void)
{
    
    
    const char *str = "ABC123def";
    char buf[32];
    strlwr(buf, str, 32);
    printfd(CCL_BLUE "strlwr: %s > %s\n" CCL_END, str, buf);
    strupr(buf, str, 32);
    printfd(CCL_BLUE "strupr: %s > %s\n" CCL_END, str, buf);
    
    return 0;
}

