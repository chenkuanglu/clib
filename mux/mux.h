#ifndef __THR_MUX__
#define __THR_MUX__

/**
 * @file    mux.h
 * @author  ln
 * @brief   mutex, inner process & recursive
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

typedef struct {
    pthread_mutex_t     mux;
    pthread_mutexattr_t attr;
} mux_t;

int mux_init    (mux_t *mux);
int mux_lock    (mux_t *mux);
int mux_unlock  (mux_t *mux);

#ifdef __cplusplus
}
#endif

#endif /* __THR_MUX__ */

