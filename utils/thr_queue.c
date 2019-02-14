/**
 * @file    thr_queue.c
 * @author  ln
 * @brief   thread safe msg queue
 **/

#include "thr_queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <tgmath.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   init thrq control block
 * @param   thrq        queue to be init
 *
 * @return  0 is ok
 **/
int thrq_init(thrq_cb_t *thrq)
{

    TAILQ_INIT(&thrq->head);

    mux_init(&thrq->lock);
    pthread_mutex_init(&thrq->cond_lock, 0);

    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);
    pthread_cond_init(&thrq->cond, &cond_attr);

    thrq->count      = 0;
    thrq->free_data  = 0;
    thrq->copy_data  = memcpy;
    thrq->cmp_elm    = memcmp;
    thrq->max_size   = THRQ_MAX_SIZE_DEF;

    mpool_init(&thrq->mpool, 0, 0);

    return 0;
}

/**
 * @brief   set a function to free user data if neccessary
 * @param   thrq        queue
 *          free_data   func to free user data
 *
 * @return  none
 *
 * func 'free_data' will be called while removing element
 **/
void thrq_set_free(thrq_cb_t *thrq, thrq_free_data_t free_data)
{
    mux_lock(&thrq->lock);
    thrq->free_data = free_data;
    mux_unlock(&thrq->lock);
}

/**
 * @brief   clean & init memory pool for thrq alloc/free
 * @param   thrq        queue
 *          n           number of data element
 *          data_size   max size of user data
 *
 * @return  0 is ok
 **/
int thrq_set_mpool(thrq_cb_t *thrq, size_t n, size_t data_size)
{
    mux_lock(&thrq->lock);
    mpool_clean(&thrq->mpool);
    int res = mpool_init(&thrq->mpool, n, data_size);
    mux_unlock(&thrq->lock);

    return res;
}

/**
 * @brief   set a function to copy user data if neccessary
 * @param   thrq        queue
 *          copy_data   func to copy user data
 *
 * @return  none
 *
 * func 'copy_data' will be called while insert/receiving data
 **/
void thrq_set_copy(thrq_cb_t *thrq, thrq_copy_data_t copy_data)
{
    mux_lock(&thrq->lock);
    thrq->copy_data = copy_data;
    mux_unlock(&thrq->lock);
}

/**
 * @brief   set a function to compare user data if neccessary
 * @param   thrq            queue
 *          compare_data    func to compare user data
 *
 * @return  none
 **/
void thrq_set_compare(thrq_cb_t *thrq, thrq_cmp_data_t compare_data)
{
    mux_lock(&thrq->lock);
    thrq->cmp_elm = compare_data;
    mux_unlock(&thrq->lock);
}

/**
 * @brief   set max size of thrq
 * @param   thrq        queue
 *          max_size    >= count of elements 
 *
 * @return  none
 **/
void thrq_set_maxsize(thrq_cb_t *thrq, int max_size)
{
    mux_lock(&thrq->lock);
    thrq->max_size = max_size;
    mux_unlock(&thrq->lock);
}

/**
 * @brief   is queue empty
 * @param   thrq    pointer to the queue
 * @return  true(!0) or false(0)
 **/
int thrq_empty(thrq_cb_t *thrq)
{
    mux_lock(&thrq->lock);
    int empty = THRQ_EMPTY(thrq);
    mux_unlock(&thrq->lock);

    return empty;
}

/**
 * @brief   get queue count
 * @param   thrq    pointer to the queue
 * @return  elments count
 **/
int thrq_count(thrq_cb_t *thrq)
{
    mux_lock(&thrq->lock);
    int count = thrq->count;
    mux_unlock(&thrq->lock);

    return count;
}

/**
 * @brief   get first queue element
 * @param   thrq    pointer to the queue
 * @return  pointer to the first elments
 **/
thrq_elm_t* thrq_first(thrq_cb_t *thrq)
{
    mux_lock(&thrq->lock);
    thrq_elm_t* elm = THRQ_FIRST(thrq);
    mux_unlock(&thrq->lock);

    return elm;
}

/**
 * @brief   get last queue element
 * @param   thrq    pointer to the queue
 * @return  pointer to the last elments
 **/
thrq_elm_t* thrq_last(thrq_cb_t *thrq)
{
    mux_lock(&thrq->lock);
    thrq_elm_t* elm = THRQ_LAST(thrq);
    mux_unlock(&thrq->lock);

    return elm;
}

/**
 * @brief   insert element to the head
 * @param   thrq    queue to be insert
 *          data    the data to insert
 *          len     data length
 *
 * @return  0 is ok
 **/
int thrq_insert_head(thrq_cb_t *thrq, void *data, int len)
{
    /* one byte data at least */
    if (data == 0 || len == 0)
        return -1;

    /* queue is full */
    mux_lock(&thrq->lock);

    if (thrq->count >= thrq->max_size) {        
        mux_unlock(&thrq->lock);
        return -1;
    }

    /* memoy allocate */
    thrq_elm_t *elm = (thrq_elm_t*)mpool_malloc(&thrq->mpool, (sizeof(thrq_elm_t) + len));
    if (elm == 0) {
        mux_unlock(&thrq->lock);
        return -1;
    }

    /* insert queue */
    thrq->copy_data(elm->data, data, len);
    elm->len = len;
    TAILQ_INSERT_HEAD(&thrq->head, elm, entry);
    thrq->count++;

    mux_unlock(&thrq->lock);

    return 0;
}

/**
 * @brief   insert element to the tail
 * @param   thrq    queue to be insert
 *          data    the data to insert
 *          len     data length
 *
 * @return  0 is ok
 **/
int thrq_insert_tail(thrq_cb_t *thrq, void *data, int len)
{
    if (data == 0 || len == 0)
        return -1;

    /* queue is full */
    mux_lock(&thrq->lock);

    if (thrq->count >= thrq->max_size) {        
        mux_unlock(&thrq->lock);
        return -1;
    }

    thrq_elm_t *elm = (thrq_elm_t*)mpool_malloc(&thrq->mpool, (sizeof(thrq_elm_t) + len));
    if (elm == 0) {
        mux_unlock(&thrq->lock);
        return -1;
    }

    thrq->copy_data(elm->data, data, len);
    elm->len = len;
    TAILQ_INSERT_TAIL(&thrq->head, elm, entry);
    thrq->count++;

    mux_unlock(&thrq->lock);

    return 0;
}

/**
 * @brief   insert element after any queue elment
 * @param   thrq        queue to be insert
 *          list_elm    the queue elment which to insert after
 *          data        the data to insert
 *          len         data length
 *
 * @return  0 is ok
 **/
int thrq_insert_after(thrq_cb_t *thrq, thrq_elm_t *list_elm, void *data, int len)
{
    if (list_elm == 0 || data == 0 || len == 0)
        return -1;

    /* queue is full */
    mux_lock(&thrq->lock);

    if (thrq->count >= thrq->max_size) {        
        mux_unlock(&thrq->lock);
        return -1;
    }

    thrq_elm_t *elm = (thrq_elm_t*)mpool_malloc(&thrq->mpool, (sizeof(thrq_elm_t) + len));
    if (elm == 0) {
        mux_unlock(&thrq->lock);
        return -1;
    }

    thrq->copy_data(elm->data, data, len);
    elm->len = len;
    TAILQ_INSERT_AFTER(&thrq->head, list_elm, elm, entry);
    thrq->count++;

    mux_unlock(&thrq->lock);

    return 0;
}

/**
 * @brief   insert element before any queue elment
 * @param   thrq        queue to be insert
 *          list_elm    the queue elment which to insert before
 *          data        the data to insert
 *          len         data length
 *
 * @return  0 is ok
 **/
int thrq_insert_before(thrq_cb_t *thrq, thrq_elm_t *list_elm, void *data, int len)
{
    if (list_elm == 0 || data == 0 || len == 0)
        return -1;

    /* queue is full */
    mux_lock(&thrq->lock);

    if (thrq->count >= thrq->max_size) {        
        mux_unlock(&thrq->lock);
        return -1;
    }

    thrq_elm_t *elm = (thrq_elm_t*)mpool_malloc(&thrq->mpool, (sizeof(thrq_elm_t) + len));
    if (elm == 0) {
        mux_unlock(&thrq->lock);
        return -1;
    }

    thrq->copy_data(elm->data, data, len);
    elm->len = len;
    TAILQ_INSERT_BEFORE(list_elm, elm, entry);
    thrq->count++;

    mux_unlock(&thrq->lock);

    return 0;
}

/**
 * @brief   create thrq
 * @param   thrq    ponter to the queue-pointer
 * @return  return a pointer to the queue created
 *
 * example: thrq_cb_t *myq = thrq_create(0);
 *          or 
 *          thrq_cb_t *myq;
 *          thrq_create(&myq);
 **/
thrq_cb_t* thrq_create(thrq_cb_t **thrq)
{
    thrq_cb_t *que = (thrq_cb_t*)malloc(sizeof(thrq_cb_t));
    if (que) {
        thrq_init(que);
    }
    if (thrq) {
        *thrq = que;
    }
    return que;
}

/**
 * @brief   free all the elements of thrq (except thrq itself)
 * @param   thrq    queue to clean
 * @return  void
 **/
void thrq_clean(thrq_cb_t *thrq)
{
    while (thrq && !thrq_empty(thrq)) {
        thrq_remove(thrq, thrq_first(thrq));
    }    
}

/**
 * @brief   free all the elements of thrq  & thrq itself
 * @param   thrq    queue to free
 * @return  void
 **/
void thrq_destroy(thrq_cb_t *thrq)
{
    if (thrq) { 
        thrq_clean(thrq);
        free(thrq);
    }
}

/**
 * @brief   remove element
 * @param   thrq    ponter to the queue
 *          elm     the elment to be removed
 *
 * @return  0 is ok
 **/
int thrq_remove(thrq_cb_t *thrq, thrq_elm_t *elm)
{
    if (elm != 0) {
        mux_lock(&thrq->lock);
        if (thrq->free_data) {
            thrq->free_data(elm->data);
        }
        TAILQ_REMOVE(&thrq->head, elm, entry);
        mpool_free(&thrq->mpool, elm);
        if (thrq->count > 0) {
            thrq->count--;
        }
        mux_unlock(&thrq->lock);
    }
    return 0;
}

/**
 * @brief   concat 2 thrq
 * @param   thrq1   ponter to the queue 1
 *          thrq2   ponter to the queue 2
 *
 * @return  0 is ok
 *
 * thrq1 = thrq1 + thrq2, and thrq2 = init
 **/
int thrq_concat(thrq_cb_t *thrq1, thrq_cb_t *thrq2)
{
    mux_lock(&thrq1->lock);
    mux_lock(&thrq2->lock);
    TAILQ_CONCAT(&thrq1->head, &thrq2->head, entry);
    mux_unlock(&thrq2->lock);
    mux_unlock(&thrq1->lock);

    return 0;
}

/**
 * @brief   find element from queue
 * @param   thrq        queue to be insert
 *          data        the data to find
 *          len         data length
 *
 * @return  return the pointer to the element found
 **/
thrq_elm_t* thrq_find(thrq_cb_t *thrq, void *data, int len)
{
    thrq_elm_t *var;

    mux_lock(&thrq->lock);
    THRQ_FOREACH(var, thrq) {
        if ((thrq->cmp_elm)(var->data, data, fmin(len, var->len)) == 0) {
            mux_unlock(&thrq->lock);
            return var;
        }
    }    
    mux_unlock(&thrq->lock);

    return 0;
}

/**
 * @brief   insert element and send signal
 * @param   thrq    queue to be send
 *          data    the data to send
 *          len     data length
 *
 * @return  0 is ok
 **/
int thrq_send(thrq_cb_t *thrq, void *data, int len)
{
    pthread_mutex_lock(&thrq->cond_lock);
    int res = thrq_insert_tail(thrq, data, len);
    pthread_cond_signal(&thrq->cond);
    pthread_mutex_unlock(&thrq->cond_lock);
    return res;
}

/**
 * @brief   receive and remove element
 * @param   thrq        queue to receive
 *          buf         the data buf
 *          max_size    buf size
 *          timeout     thread block time, 0 is block until signal received
 *
 * @return  0 is ok
 * 
 *  function returns when error occured or data received,
 *  ETIMEDOUT returned while timeout (ETIMEDOUT defined in <errno.h>)
 **/
int thrq_receive(thrq_cb_t *thrq, void *buf, int max_size, double timeout)
{
    int res = 0;
    struct timespec ts;

    if (timeout > 0) {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        ts.tv_nsec = (long)((timeout - (long)timeout) * 1000000000L) + ts.tv_nsec;  // ok, max_long_int = 2.1s > (1s + 1s)
        ts.tv_sec = (time_t)timeout + ts.tv_sec + (ts.tv_nsec / 1000000000L);
        ts.tv_nsec = ts.tv_nsec % 1000000000L;
    }
    printf("s=%ld, ns=%ld ##############################", ts.tv_sec, ts.tv_nsec);

    pthread_mutex_lock(&thrq->cond_lock);

    /* break when error occured or data receive */
    while (res == 0 && thrq_count(thrq) <= 0) {
        if (timeout > 0) {
            res = pthread_cond_timedwait(&thrq->cond, &thrq->cond_lock, &ts);
        } else {
            res = pthread_cond_wait(&thrq->cond, &thrq->cond_lock);
        }
    }
    if (res != 0) {
        pthread_mutex_unlock(&thrq->cond_lock);
        return res;
    }

    /* data received */
    mux_lock(&thrq->lock);
    thrq_elm_t *elm = thrq_first(thrq);
    thrq->copy_data(buf, elm->data, fmin(max_size, elm->len));
    thrq_remove(thrq, elm);
    mux_unlock(&thrq->lock);

    pthread_mutex_unlock(&thrq->cond_lock);

    return res;
}

#ifdef __cplusplus
}
#endif

