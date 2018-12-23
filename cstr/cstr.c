/**
 * @file    cstr.c
 * @author  ln
 * @brief   the c string process
 **/

#include "cstr.h"

/**
 * @brief   print format date like [2018-01-01 23:59:59]
 * @param   void
 * @return  number of char printed
 *
 * @caller  vprintfd
 *
 * internal function
 **/
static int printd(void)
{
    struct tm ltm; 
    time_t now = time(NULL); 
    localtime_r(&now, &ltm);
    int num = printf("[%04d-%02d-%02d %02d:%02d:%02d]",
                     ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday,
                     ltm.tm_hour, ltm.tm_min, ltm.tm_sec);                                  
    return num;
}

/**
 * @brief   print format string with date([2018-01-01 23:59:59]) prefix
 * @param   format      hex to convert
 *          param       parameter list
 *
 * @return  number of char printed
 **/
int vprintfd(const char *format, va_list param)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

    pthread_mutex_lock(&mutex);             
    int num = printd();
    num += printf(" ");     
    num += vprintf(format, param);         
    pthread_mutex_unlock(&mutex);
    
    return num;
}

/**
 * @brief   print format string with date([2018-01-01 23:59:59]) prefix
 * @param   format      hex to convert
 *          ...         variable parameters
 *
 * @return  number of char printed
 **/
int printfd(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);    
    int num = vprintfd(format, arg);  
    va_end(arg); 

    return num;
}

/**
 * @brief   Convert a string to lowercase
 * @param   in      String to convert
 *          out     Output buffer
 *          size    Size of the out buffer
 * @return  pointer to the out buffer or NULL if an error occured
 *
 * This function convert a string into lowercase.
 * At most size - 1 elements of the input string will be converted.
 **/
char* strlwr(char *out, const char *in, unsigned size)
{
    if (in == NULL || out == NULL || size == 0) {
        return NULL;
    }
    unsigned i = 0;
    while (in[i] != '\0' && i < size-1) {
        out[i] = (char)tolower((int)in[i]);
        i++ ;
    }
    out[i] = '\0';
    return out ;
}

/**
 * @brief   Convert a string to uppercase
 * @param   in      String to convert
 *          out     Output buffer
 *          size    Size of the out buffer
 * @return  pointer to the out buffer or NULL if an error occured
 *
 * This function convert a string into uppercase.
 * At most size - 1 elements of the input string will be converted.
 **/
char* strupr(char *out, const char *in, unsigned size)
{
    if (in == NULL || out == NULL || size == 0) {
        return NULL;
    }
    unsigned i = 0;
    while (in[i] != '\0' && i < size-1) {
        out[i] = (char)toupper((int)in[i]);
        i++ ;
    }
    out[i] = '\0';
    return out ;
}

/**
 * @brief   Convert bin to hex
 * @param   hex         Output string buffer
 *          bin         bin to convert
 *          len         lenght of the bin to convert
 *
 * @return  the number of char in hex buffer
 **/
int bin2hex(char *hex, const void *bin, unsigned len)
{
    if (bin == NULL || hex == NULL || len == 0) 
        return 0;
    unsigned i = 0;
    for (i = 0; i < len; i++) {
        sprintf(&hex[i*2], "%02x", ((unsigned char *)bin)[i]);
    }
    hex[i*2] = '\0';
    return i*2;
}

/**
 * @brief   Convert bin to hex & allocate output buffer
 * @param   bin         bin to convert
 *          len         lenght of the bin to convert
 *
 * @return  pointer to the ouput char buffer
 *
 * Need to free the pointer returned from this func
 **/
char* abin2hex(const void *bin, unsigned len)
{
    if (bin == NULL || len == 0) 
        return NULL;

    char *s = (char*)malloc((len * 2) + 1);
    if (s != NULL) {
        bin2hex(s, bin, len);
    }
    return s;
}

/**
 * @brief   Convert hex to bin
 * @param   bin         Output bin buffer
 *          hex         hex to convert
 *          len         lenght of output bin buffer
 *
 * @return  void
 **/
void hex2bin(void *bin, const char *hex, unsigned len)
{
    if (bin == NULL || hex == NULL || len == 0) 
        return;

    char buf[4] = {0};
    if (strlen(hex) % 2) {
        buf[0] = *hex++;
        *((unsigned char *)bin) = (unsigned char)strtol(buf, NULL, 16);
        bin = (unsigned char *)bin + 1;
    }
    while (*hex && len) {
        buf[0] = *hex++;
        buf[1] = *hex++;
        *((unsigned char *)bin) = (unsigned char)strtol(buf, NULL, 16);
        len--;
        bin = (unsigned char *)bin + 1;
    }
}

/**
 * @brief   Convert hex to bin
 * @param   hex         hex to convert
 *          len         lenght of the hex to convert
 *
 * @return  pointer to the ouput bin buffer
 *
 * the string 'fde' is same as '0fde'
 * Need to free the pointer returned from this func
 **/
void * ahex2bin(const char *hex)
{
    if (hex == NULL) 
        return NULL;

    unsigned len = strlen(hex);
    unsigned char *b = (unsigned char*)malloc(len/2 + 1);
    if (b != NULL) {
        hex2bin(b, hex, len/2 + 1);
    }
    return (void *)b;
}

/**
 * @brief   reverse/swap specified length memory byte-to-byte
 * @param   out             output buffer
 *          in              input buffer
 *          len             size of input buffer
 *          section_size    size of memory section to swap
 *
 * len = 4:
 * section_size=2:[0][1][2][3] ===> [1][0][3][2]
 * section_size=4:[0][1][2][3] ===> [3][2][1][0], the same as big/small endian convert on integer
 *
 * len 0 means section size is the len
 *
 * @return  void
 **/
void memswap(void *out, const void *in, unsigned len, unsigned section_size)
{
    if (out == NULL || in == NULL || len == 0)
        return;

    if (section_size == 0) {
        section_size = len;
    }
    unsigned num = len/section_size;
    for (unsigned i=0; i<num; i++) {
        char *pin = (char *)in + i*section_size;
        char *pout = (char *)out + i*section_size;
        unsigned ss = 0;
        while (ss < section_size/2) {
            char c = pin[ss];
            pout[ss] = pin[section_size - ss - 1];
            pout[section_size - ss - 1] = c;
            ss++;
        }
    }
}

/**
 * @brief   split string by ',' or blank(' ') and store the result in a dictionary
 * @param   str     string to split
 *
 * Need to free the pointer returned from this func by calling dictionary_del()
 *
 * @return  pointer to dictionary
 *
 * Split number = dict->n
 * Split elment = dictionary_get(dict, "N", NULL)     // N = 0 ~ (dict->n - 1)
 **/
//dictionary* strsplit(char *str, char sep)
//{
//    dictionary* dict = dictionary_new(0);
//    if (dict == NULL) 
//        return NULL;
//
//    char *begin = NULL;
//    char *s = str;
//    int len = strlen(str);
//    while (len) {
//        if (*s == ',' || *s == ' ') {
//            if (begin != NULL) {
//                dictionary_set();
//                begin = NULL;
//            }
//        } else { 
//            if (begin == NULL) {
//                begin = s;
//            }
//        }
//        s++;
//    }
//
//    return dict;
//}

