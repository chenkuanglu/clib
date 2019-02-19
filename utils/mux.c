/**
 * @file    mux.c
 * @author  ln
 * @brief   mutex, inner process & recursive
 **/

#include "mux.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   init mutex, inner process & recursive
 * @param   mutex to be init
 * @return  0 is sucess
 **/
int mux_init(mux_t *mux)
{
    if (mux == NULL) 
        return -1;
    
    int res = 0;
    res += pthread_mutexattr_init(&mux->attr);
    res += pthread_mutexattr_setpshared(&mux->attr, PTHREAD_PROCESS_PRIVATE);
    res += pthread_mutexattr_setprotocol(&mux->attr, PTHREAD_PRIO_INHERIT);
    res += pthread_mutexattr_settype(&mux->attr, PTHREAD_MUTEX_RECURSIVE);

    res += pthread_mutex_init(&mux->mux, &mux->attr);
    return res;
}

/**
 * @brief   malloc & init mutex, inner process & recursive
 * @param   mux     pointer to your mutex pointer
 * @return  pointer to the mutex created
 **/
mux_t* mux_new(mux_t **mux)
{
    mux_t *p = (mux_t *)malloc(sizeof(mux_t));
    if (mux_init(p) < 0) {
        free(p);
        p = NULL;
    }

    if (mux != NULL)
        *mux = p;
    return p;
}

/**
 * @brief   destroy mutex 
 * @param   mux     mutex to be clean 
 * @return  void
 *
 * do not call this function if a mutex was initialized by INITIALIZER macro! 
 **/
void mux_destroy(mux_t *mux)
{
    if (mux) {
        pthread_mutexattr_destroy(&mux->attr);
        pthread_mutex_destroy(&mux->mux);
    }
}

/**
 * @brief   lock
 * @param   mutex to be lock
 * @return  0 is sucess
 **/
int mux_lock(mux_t *mux)
{
    return pthread_mutex_lock(&mux->mux);
}

/**
 * @brief   unlock
 * @param   mutex to be unlock
 * @return  0 is sucess
 **/
int mux_unlock(mux_t *mux)
{
    return pthread_mutex_unlock(&mux->mux);
}

#ifdef __cplusplus
}
#endif

