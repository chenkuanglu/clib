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

int xconfig_load(const char *filename)
{
}

void xconfig_delete(xconfig_t *xconfig)
{
    if (xconfig != NULL) {
        //free(xconfig->ptr);
        //...
        
        free(xconfig);
    }
}

