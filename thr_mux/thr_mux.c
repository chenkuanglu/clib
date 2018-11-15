//
// pthread mutex 
// 

#ifdef __cplusplus
extern "C" {
#endif

#include "thr_mux.h"

// attributes: inner process, prio inherit and recursive
int mux_init(thr_mux_t *pmux)
{
    pthread_mutexattr_init(&pmux->attr);

    pthread_mutexattr_setpshared(&pmux->attr, PTHREAD_PROCESS_PRIVATE);
    pthread_mutexattr_setprotocol(&pmux->attr, PTHREAD_PRIO_INHERIT);
    pthread_mutexattr_settype(&pmux->attr, PTHREAD_MUTEX_RECURSIVE);

    return pthread_mutex_init(&pmux->mux, &pmux->attr);
}

int mux_lock(thr_mux_t *pmux)
{
    return pthread_mutex_lock(&pmux->mux);
}

int mux_unlock(thr_mux_t *pmux)
{
    return pthread_mutex_unlock(&pmux->mux);
}

#ifdef __cplusplus
}
#endif

