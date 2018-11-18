/**
 * @file    thr_mux.h
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
} thr_mux_t;

int mux_init    (thr_mux_t *pmux);
int mux_lock    (thr_mux_t *pmux);
int mux_unlock  (thr_mux_t *pmux);

#ifdef __cplusplus
}
#endif

