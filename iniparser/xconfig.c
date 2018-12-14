/**
 * @file    xconfig.c
 * @author  ln
 * @brief   config template
 **/

#include "xconfig.h"
#include <string.h>

xconfig_t* xconfig_init(xconfig_t* xconfig)
{
    if (xconfig != NULL) {
        DICT_ENTRY              = NULL:

        xconfig->baudrate       = 115200;
        xconfig->data_bits      = 8;
        xconfig->check_flag     = None;
        xconfig->stop_bits      = 1;
        xconfig->benchmark_en   = false;
    }
    return xconfig;
}

xconfig_t* xconfig_create(void)
{
    return xconfig_init((xconfig_t *)malloc(sizeof xconfig_t));
}

int xconfig_load(xconfig_t *xconfig, const char *filename)
{
    int ret = 0;
    xconfig->DICT_ENTRY = iniparser_load(filename);
    if (xconfig->DICT_ENTRY == NULL) {
        ret = -1;
    }
    // ini get long
    // ini get long
    // ini get long
    // ini get long
    // ...
    return ret;
}

void xconfig_delete(xconfig_t *xconfig)
{
    if (xconfig != NULL) {
        if (xconfig->DICT_ENTRY != NULL) {
            iniparser_freedict(xconfig->DICT_ENTRY);
        }
        free(xconfig);
    }
}

