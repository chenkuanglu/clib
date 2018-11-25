/**
 * @file    thr_queue.c
 * @author  ln
 * @brief   thread safe msg queue
 **/

#include "thr_queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <tgmath.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   init thrq control block
 * @param   thrq        queue to be init
 *          max_size    max queue size
 *
 * @return  0 is ok
 **/
int thrq_init(thrq_cb_t *thrq, int max_size)
{
    TAILQ_INIT(&thrq->head);

    mux_init(&thrq->lock);
    pthread_mutex_init(&thrq->cond_lock, 0);
    pthread_cond_init(&thrq->cond, 0);

    thrq->count = 0;
    thrq->cond_ok = 0;
    if (max_size <= 0) {
        thrq->max_size = THRQ_MAX_SIZE_DEF;
    } else {
        thrq->max_size = max_size;
    }

    return 0;
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
    if (data == 0 || len == 0)
        return -1;

    thrq_elm_t *elm = (thrq_elm_t*)malloc(sizeof(thrq_elm_t) + len);
    if (elm == 0) 
        return -1;

    memcpy(elm->data, data, len);
    elm->len = len;
    mux_lock(&thrq->lock);
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

    thrq_elm_t *elm = (thrq_elm_t*)malloc(sizeof(thrq_elm_t) + len);
    if (elm == 0) 
        return -1;

    memcpy(elm->data, data, len);
    elm->len = len;
    mux_lock(&thrq->lock);
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

    thrq_elm_t *elm = (thrq_elm_t*)malloc(sizeof(thrq_elm_t) + len);
    if (elm == 0) 
        return -1;

    memcpy(elm->data, data, len);
    elm->len = len;
    mux_lock(&thrq->lock);
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

    thrq_elm_t *elm = (thrq_elm_t*)malloc(sizeof(thrq_elm_t) + len);
    if (elm == 0) 
        return -1;

    memcpy(elm->data, data, len);
    elm->len = len;
    mux_lock(&thrq->lock);
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
thrq_cb_t* thrq_create(thrq_cb_t **thrq, int max_size)
{
    thrq_cb_t *que = (thrq_cb_t*)malloc(sizeof(thrq_cb_t));
    if (que) {
        thrq_init(que, max_size);
        *thrq = que;
    } else {
        *thrq = 0;
    }
    return que;
}

/**
 * @brief   free the thrq created before
 * @param   thrq    queue to free
 * @return  void
 **/
void thrq_free(thrq_cb_t *thrq)
{
    if (thrq) 
        free(thrq);
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
        TAILQ_REMOVE(&thrq->head, elm, entry);
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
 *          list_elm    the queue elment which to insert before
 *          data        the data to insert
 *          len         data length
 *
 * @return  return the pointer to the element found
 **/
thrq_elm_t* thrq_find(thrq_cb_t *thrq, void *data, int len, thrq_cmp_t elm_cmp)
{
    thrq_elm_t *var;
    THRQ_FOREACH(var, thrq) {
        if (elm_cmp(var, data, fmin(len, var->len)) == 0) {
            return var;
        }
    }    
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
    int res = 0;

    res = thrq_insert_tail(thrq, data, len);
    if (res == 0) {
        pthread_mutex_lock(&thrq->cond_lock);
        thrq->cond_ok++;
        pthread_cond_signal(&thrq->cond);
        pthread_mutex_unlock(&thrq->cond_lock);
    }

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
 *  function returns when error occured or data received
 **/
int thrq_receive(thrq_cb_t *thrq, void *buf, int max_size, double timeout)
{
    int res = 0;
    struct timespec ts = {0, 0};

    if (timeout > 0) {
        ts.tv_sec = (int)(timeout);
        ts.tv_nsec = (int)((timeout - ts.tv_sec) * 1e6);
    }

    pthread_mutex_lock(&thrq->cond_lock);

    /* break when error occured or data receive */
    while (res == 0 && thrq->cond_ok <= 0) {
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

    /* data receved */
    if (thrq->cond_ok > 0) {
        thrq->cond_ok--;
    }
    mux_lock(&thrq->lock);
    thrq_elm_t *elm = thrq_first(thrq);
    memcpy(buf, elm->data, fmin(max_size, elm->len));
    thrq_remove(thrq, elm);
    mux_lock(&thrq->lock);

    pthread_mutex_unlock(&thrq->cond_lock);

    return res;
}

#ifdef __cplusplus
}
#endif

