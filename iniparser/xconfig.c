/**
 * @file    xconfig.c
 * @author  ln
 * @brief   config template
 **/

#include "xconfig.h"
#include "log.h"

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

        xconfig->dev_name       = strdup(DEV_NAME_DEFAULT);
        xconfig->baudrate       = BAUDRATE_DEFAULT;
        xconfig->data_bits      = DATA_BITS_DEFAULT;
        xconfig->parity         = PARITY_DEFAULT;
        xconfig->stop_bits      = STOP_BITS_DEFAULT;
        xconfig->benchmark_en   = BENCHMARK_EN_DEFAULT;
    }
    iniparser_set_error_callback(loge);
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
#ifdef DEBUG
        printfd("xconfig: Loading serial config\n");
#endif
        if (xconfig->dev_name) {
            free(xconfig->dev_name);
        }
        xconfig->dev_name = strdup(iniparser_getstring(xconfig->DICT_ENTRY, CFG_DEV_NAME, DEV_NAME_DEFAULT));
        xconfig->baudrate = iniparser_getlongint(xconfig->DICT_ENTRY, CFG_BAUDRATE, BAUDRATE_DEFAULT);
        xconfig->data_bits = iniparser_getlongint(xconfig->DICT_ENTRY, CFG_DATA_BITS, DATA_BITS_DEFAULT);
        const char *pty = iniparser_getstring(xconfig->DICT_ENTRY, CFG_PARITY, MAKE_CSTR(PARITY_DEFAULT));
        if (strcmp(pty, "None") == 0) {
            xconfig->parity = None;
        } else if (strcmp(pty, "Odd") == 0) {
            xconfig->parity = Odd;
        } else if (strcmp(pty, "Even") == 0) {
            xconfig->parity = Even;
        } else {
            xconfig->parity = PARITY_DEFAULT;
            printfd(CCL_YELLOW "xconfig: Unknown serial parity config '%s'\n" CCL_END, pty);
        }
        xconfig->stop_bits = iniparser_getlongint(xconfig->DICT_ENTRY, CFG_STOP_BITS, STOP_BITS_DEFAULT);
#ifdef DEBUG
        printfd("dev_name   = %s\n", xconfig->dev_name);
        printfd("baudrate   = %d\n", xconfig->baudrate);
        printfd("data_bits  = %d\n", xconfig->data_bits);
        printfd("parity     = %s\n", (xconfig->parity == None) ? "None" : ((xconfig->parity == Odd) ? "Odd" : "Even"));
        printfd("stop_bits  = %d\n", xconfig->stop_bits);
#endif
        /* [benchmark] */
#ifdef DEBUG
        printfd("\n");
        printfd("xconfig: Loading benchmark config\n");
#endif
        xconfig->benchmark_en = iniparser_getboolean(xconfig->DICT_ENTRY, CFG_BENCHMARK_EN, (int)BENCHMARK_EN_DEFAULT);
#ifdef DEBUG
        printfd("benchmark_en = %s\n", xconfig->benchmark_en ? "true" : "false");
#endif

        ret = 0;
    }
    
    return ret;
}

void xconfig_delete(xconfig_t *xconfig)
{
    if (xconfig != NULL) {
        if (xconfig->DICT_ENTRY != NULL) {
            iniparser_freedict(xconfig->DICT_ENTRY);
            free(xconfig->dev_name);
        }
        free(xconfig);
    }
}

