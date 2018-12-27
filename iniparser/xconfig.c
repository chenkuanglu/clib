/**
 * @file    xconfig.c
 * @author  ln
 * @brief   config template
 **/

#include "xconfig.h"

int iniparser_error_callback(const char *format, ...)
{
    int num;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
              
    pthread_mutex_lock(&mutex);  

    num = printfd("");
    num += printf(CCL_RED);
    va_list arg;
    va_start(arg, format);
    num += vprintf(format, arg);
    va_end(arg);
    num += printf(CCL_END);
         
    pthread_mutex_unlock(&mutex);

    return num;
}

static xconfig_t* xconfig_init(xconfig_t* xconfig)
{
    if (xconfig != NULL) {
        xconfig->DICT_ENTRY     = NULL;

        xconfig->baudrate       = BAUDRATE_DEFAULT;
        xconfig->data_bits      = DATA_BITS_DEFAULT;
        xconfig->check_flag     = CHECK_FLAG_DEFAULT;
        xconfig->stop_bits      = STOP_BITS_DEFAULT;
        xconfig->benchmark_en   = BENCHMARK_EN_DEFAULT;
    }
    iniparser_set_error_callback(iniparser_error_callback);
    return xconfig;
}

xconfig_t* xconfig_new(void)
{
    return xconfig_init((xconfig_t *)malloc(sizeof(xconfig_t)));
}

int xconfig_load(xconfig_t *xconfig, const char *inifile)
{
    int ret = -1;
    dictionary *dict = iniparser_load(inifile);
    if (dict != NULL) {
        if (xconfig->DICT_ENTRY != NULL) {
            iniparser_freedict(xconfig->DICT_ENTRY);
            xconfig->DICT_ENTRY = NULL;
        }
        xconfig->DICT_ENTRY = dict;
        
        /* [serial] */
        xconfig->baudrate = iniparser_getlongint(xconfig->DICT_ENTRY, CFG_BAUDRATE, BAUDRATE_DEFAULT);
        xconfig->data_bits = iniparser_getlongint(xconfig->DICT_ENTRY, CFG_DATA_BITS, DATA_BITS_DEFAULT);
        const char *cf = iniparser_getstring(xconfig->DICT_ENTRY, CFG_CHECK_FLAG, "unkown");
        if (strcmp(cf, "None") == 0) {
            xconfig->check_flag = None;
        } else if (strcmp(cf, "Odd") == 0) {
            xconfig->check_flag = Odd;
        } else if (strcmp(cf, "Even") == 0) {
            xconfig->check_flag = Even;
        } else {
            xconfig->check_flag = CHECK_FLAG_DEFAULT;
            printfd(CCL_YELLOW "xconfig: unkown serial config '%s'\n" CCL_END, cf);
        }
        xconfig->stop_bits = iniparser_getlongint(xconfig->DICT_ENTRY, CFG_STOP_BITS, STOP_BITS_DEFAULT);

        /* [benchmark] */
        xconfig->benchmark_en = iniparser_getboolean(xconfig->DICT_ENTRY, CFG_BENCHMARK_EN, (int)BENCHMARK_EN_DEFAULT);

        ret = 0;
    }
    
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

