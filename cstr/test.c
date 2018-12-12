#include <stdio.h>
#include "cstr.h"

#define COLON                       :
#define MAKE_DICT_KEY(sec, key)     ( MAKE_STRING(sec) MAKE_STRING(COLON) MAKE_STRING(key) )

/* user config */
#define INI_SEC_PROGRAM             program
#define INI_KEY_PROGRAM_ADDR        address

#define DICT_KEY_PROGRAM_ADDR       MAKE_DICT_KEY(INI_SEC_PROGRAM, INI_KEY_PROGRAM_ADDR)

int main(void)
{
    printf("ini dict key: %s\n", DICT_KEY_PROGRAM_ADDR);

    return 0;
}

