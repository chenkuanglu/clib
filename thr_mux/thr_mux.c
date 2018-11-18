/**
 * @file    thr_mux.c
 * @author  ln
 * @brief   mutex, inner process & recursive
 **/

#include "thr_mux.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   init mutex, inner process & recursive
 * @param   mutex to be init
 * @return  0 is sucess
 **/
int mux_init(thr_mux_t *pmux)
{
    pthread_mutexattr_init(&pmux->attr);

    pthread_mutexattr_setpshared(&pmux->attr, PTHREAD_PROCESS_PRIVATE);
    pthread_mutexattr_setprotocol(&pmux->attr, PTHREAD_PRIO_INHERIT);
    pthread_mutexattr_settype(&pmux->attr, PTHREAD_MUTEX_RECURSIVE);

    return pthread_mutex_init(&pmux->mux, &pmux->attr);
}

/**
 * @brief   lock
 * @param   mutex to be lock
 * @return  0 is sucess
 **/
int mux_lock(thr_mux_t *pmux)
{
    return pthread_mutex_lock(&pmux->mux);
}

/**
 * @brief   unlock
 * @param   mutex to be unlock
 * @return  0 is sucess
 **/
int mux_unlock(thr_mux_t *pmux)
{
    return pthread_mutex_unlock(&pmux->mux);
}

#ifdef __cplusplus
}
#endif

