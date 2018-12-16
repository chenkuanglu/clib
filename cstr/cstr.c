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

