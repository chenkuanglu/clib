/**
 * @file    cstr.c
 * @author  ln
 * @brief   the c string process
 **/

#include "cstr.h"

static int __print_date(void)
{
    struct tm ltm; 
    time_t now = time(NULL); 
    localtime_r(&now, &ltm);
    int num = printf("[%04d-%02d-%02d %02d:%02d:%02d]",
                     ltm.tm_year + 1900, ltm.tm_mon + 1, ltm.tm_mday,
                     ltm.tm_hour, ltm.tm_min, ltm.tm_sec);                                  
    return num;
}

int vprintfd(const char *format, va_list param)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
    pthread_mutex_lock(&mutex);             
    int num = __print_date();
    num += printf(" ");     
    num += vprintf(format, param);         
    pthread_mutex_unlock(&mutex);
    
    return num;
}

int printfd(const char *format, ...)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);               
    int num = __print_date();
    num += printf(" "); 
    
    va_list arg;
    va_start(arg, format);    
    num += vprintf(format, arg);  
    va_end(arg); 
          
    pthread_mutex_unlock(&mutex);
    
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

int bin2hex(char *hex, unsigned hex_size, const void *bin, unsigned len)
{
    unsigned i = 0;
    for (i = 0; i < len; i++) {
        if (hex_size < (i+1)*2 + 1) {
            i++;
            break;
        }
        sprintf(&hex[i*2], "%02x", ((unsigned char *)bin)[i]);
    }
    hex[i*2] = '\0';
    return i*2;
}

char* abin2hex(const void *bin, unsigned len)
{
    char *s = (char*)malloc((len * 2) + 1);
    if (s != NULL) {
        bin2hex(s, (len * 2) + 1, bin, len);
    }
    return s;
}

void hex2bin(void *bin, unsigned bin_size, const char *hex, unsigned len)
{
    if (len == 0)
        len = strlen(hex);

    char buf[4] = {0};
    unsigned num = (bin_size*2 > len) ? len/2: bin_size;
    while (*hex && num) {
        buf[0] = *hex++;
        buf[1] = *hex++;
        *((unsigned char *)bin) = (unsigned char)strtol(buf, NULL, 16);
        num--;
        bin++;
    }
}

unsigned char* ahex2bin(const char *hex, unsigned len)
{
    if (len == 0)
        len = strlen(hex);

    unsigned char *b = (unsigned char*)malloc(len/2);
    if (b != NULL) {
        hex2bin(b, len/2, hex, len);
    }
    return b;
}

