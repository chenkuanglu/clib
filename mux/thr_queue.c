//
// pthread mutex 
// 

#include "thr_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

// declare your head var & initialize them
int thrq_head_init(thrq_head_t *phead)
{
    TAILQ_INIT(&phead->head_base);
    thr_mux_init(&phead->mux);

    pthread_mutex_init(&phead->cond_lock, 0);
    pthread_cond_init(&phead->cond, 0);

    return 0;
}

int thrq_insert_head(thrq_head_t *phead, thrq_elm_t *pelm)
{
    thr_mux_lock(&phead->mux);
    TAILQ_INSERT_HEAD(&phead->head_base, pelm, entry);
    thr_mux_unlock(&phead->mux);
    return 0;
}

int thrq_insert_tail(thrq_head_t *phead, thrq_elm_t *pelm)
{
    thr_mux_lock(&phead->mux);
    TAILQ_INSERT_TAIL(&phead->head_base, pelm, entry);
    thr_mux_unlock(&phead->mux);
    return 0;
}

int thrq_insert_after(thrq_head_t *phead, thrq_elm_t *listelm, thrq_elm_t *pelm)
{
    if (listelm == NULL) {
        return -1;
    }
    thr_mux_lock(&phead->mux);
    TAILQ_INSERT_AFTER(&phead->head_base, listelm, pelm, entry);
    thr_mux_unlock(&phead->mux);
    return 0;
}

int thrq_insert_before(thrq_head_t *phead, thrq_elm_t *listelm, thrq_elm_t *pelm)
{
    if (listelm == NULL) {
        return -1;
    }
    thr_mux_lock(&phead->mux);
    TAILQ_INSERT_BEFORE(listelm, pelm, entry);
    thr_mux_unlock(&phead->mux);
    return 0;
}

int thrq_remove(thrq_head_t *phead, thrq_elm_t *pelm)
{
    if (pelm == NULL) {
        return -1;
    }
    thr_mux_lock(&phead->mux);
    TAILQ_REMOVE(&phead->head_base, pelm, entry);
    thr_mux_unlock(&phead->mux);
    return 0;
}

// head1 = head1 + head2, head2 = init(delete)
int thrq_concat(thrq_head_t *phead1, thrq_head_t *phead2)
{
    thr_mux_lock(&phead1->mux);
    thr_mux_lock(&phead2->mux);
    TAILQ_CONCAT(&phead2->head_base, &phead2->head_base, entry);
    thr_mux_unlock(&phead1->mux);
    thr_mux_unlock(&phead2->mux);
    return 0;
}

thrq_elm_t* thrq_find(thrq_head_t *phead, thrq_elm_t *pelm, pfn_thrq_cmp_t elm_cmp)
{
    thrq_elm_t *var;
    THRQ_FOREACH(var, phead) {
        if (elm_cmp(var, pelm) == 0) {
            return var;
        }
    }    
    return NULL;
}

//
//+++++++++++++++++++++++++++++ FIFO queue with cond +++++++++++++++++++++++++++++++++
//
int thrq_send(thrq_head_t *phead, thrq_elm_t *pelm)
{
    thrq_insert_tail(phead, pelm);

    pthread_mutex_lock(&phead->cond_lock);
    pthread_cond_signal(&phead->cond);
    pthread_mutex_lock(&phead->cond_lock);

    return 0;
}

// time = x.y sec, 0 is blocked until signal received
int thrq_receive(thrq_head_t *phead, thrq_elm_t **ppelm, double time)
{
    int res;
    int empty;
    struct timespec timeout;

    if (time > 0) {
        timeout.tv_sec = (int)(time);
        timeout.tv_nsec = (int)((time - timeout.tv_sec) * 1e6);
    }

    do {
        pthread_mutex_lock(&phead->cond_lock);

        if (time > 0) {
            res = pthread_cond_timedwait(&phead->cond, &phead->cond_lock, &timeout);
        } else {
            res = pthread_cond_wait(&phead->cond, &phead->cond_lock);
        }
        thr_mux_lock(&phead->mux);
        empty = thrq_empty(phead);
        thr_mux_lock(&phead->mux);

        pthread_mutex_lock(&phead->cond_lock);
    } while ((res == 0) && empty);

    if (res == 0) {
        *ppelm = thrq_first(phead);
        thrq_remove(phead, *ppelm);
    } else {
        *ppelm = 0;
    }

    return res;
}

#ifdef __cplusplus
}
#endif

