/**
 * @file    mux.c
 * @author  ln
 * @brief   mutex, inner process & recursive
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include "mux.h"

/**
 * @brief   init mutex, inner process & recursive
 * @param   mutex to be init
 * @return  0 is sucess
 **/
int mux_init(mux_t *mux)
{
    pthread_mutexattr_init(&mux->attr);

    pthread_mutexattr_setpshared(&mux->attr, PTHREAD_PROCESS_PRIVATE);
    pthread_mutexattr_setprotocol(&mux->attr, PTHREAD_PRIO_INHERIT);
    pthread_mutexattr_settype(&mux->attr, PTHREAD_MUTEX_RECURSIVE);

    return pthread_mutex_init(&mux->mux, &mux->attr);
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

