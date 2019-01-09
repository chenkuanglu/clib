/**
 * @file    log.c
 * @author  ln
 * @brief   print & save log info
 **/

/* for using PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP */
#define _GNU_SOURCE

#include "log.h"
 
#ifdef __cplusplus
extern "C" {
#endif

pthread_mutex_t mutex_log = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP; 

/**
 * @brief   print format date like [2018-01-01 23:59:59]
 * @param   void
 * @return  number of char printed
 *
 * @caller  vprintfd
 *
 * internal function
 **/
static int printdate(void)
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
 * @brief   print format string with date([2018-01-01 23:59:59]) prefix into file stream
 * @param   format      format string
 *          param       parameter list
 *
 * @return  number of char printed
 **/
int vfprintfd(FILE *stream, const char *format, va_list param)
{
    pthread_mutex_lock(&mutex_log);
    int num = printdate();
    num += printf(" ");     
    num += vfprintf(stream, format, param);         
    pthread_mutex_unlock(&mutex_log);
    
    return num;
}

/**
 * @brief   print format string with date([2018-01-01 23:59:59]) prefix into file stream
 * @param   format      format string
 *          ...         variable parameters
 *
 * @return  number of char printed
 **/
int fprintfd(FILE *stream, const char *format, ...)
{
    va_list arg;
    va_start(arg, format);    
    int num = vfprintfd(stream, format, arg);  
    va_end(arg); 

    return num;
}

/**
 * @brief   print format string with date([2018-01-01 23:59:59]) prefix
 * @param   format      format string
 *          param       parameter list
 *
 * @return  number of char printed
 **/
int vprintfd(const char *format, va_list param)
{
    return vfprintfd(stdout, format, param);
}

/**
 * @brief   print format string with date([2018-01-01 23:59:59]) prefix
 * @param   format      format string
 *          ...         variable parameters
 *
 * @return  number of char printed
 **/
int printfd(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);    
    int num = vfprintfd(stdout, format, arg);  
    va_end(arg); 

    return num;
}

int logd(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);    

    pthread_mutex_lock(&mutex_log);
    int num = fprintf(stdout, CCL_GRAY_DARK);
    num += vfprintfd(stdout, format, arg);  
    num += fprintf(stdout, CCL_END);
    pthread_mutex_unlock(&mutex_log);

    va_end(arg); 

    return num;
}

int logi(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);    
    int num = vfprintfd(stdout, format, arg);  
    va_end(arg); 

    return num;
}

int logw(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);    

    pthread_mutex_lock(&mutex_log);
    int num = fprintf(stdout, CCL_YELLOW);
    num += vfprintfd(stdout, format, arg);  
    num += fprintf(stdout, CCL_END);
    pthread_mutex_unlock(&mutex_log);

    va_end(arg); 

    return num;
}

int loge(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);    

    pthread_mutex_lock(&mutex_log);
    int num = fprintf(stderr, CCL_RED);
    num += vfprintfd(stderr, format, arg);  
    num += fprintf(stderr, CCL_END);
    pthread_mutex_unlock(&mutex_log);

    va_end(arg); 

    return num;
}

#ifdef __cplusplus
}
#endif

