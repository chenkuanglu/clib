#ifndef __THR_QUEUE__
#define __THR_QUEUE__

/**
 * @file    thr_queue.h
 * @author  ln
 * @brief   thread safe msg queue
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <sys/queue.h>
#include <pthread.h>
#include "../mux/mux.h"

/**
 * declare user data type with list head struct: 
 *
 *  struct __thrq_elm {
 *      // user data1 (empty as it is lib definition)
 *      struct {
 *          struct __thrq_elm *next;
 *          struct __thrq_elm **prev;   // a pointer to pointer!
 *      } entry;
 *      // user data2 (empty as it is lib definition)
 *  } thrq_elm_t;
 *
 *    elm->entry.next  == pointer to next elm
 *    elm->entry.prev  == pointer to pre-elm's entry.next
 *  *(elm->entry.prev) == pointer to itself
 *
 * 'thrq_elm_t' is used as the base struct of user custom type.
 **/
typedef struct __thrq_elm {
    TAILQ_ENTRY(__thrq_elm) entry;
    int                     len;
    unsigned char           data[];     /* flexible array */
} thrq_elm_t;

/**
 * declare queue head struct with first & last pointer to the type of 'struct __thrq_elm': 
 *
 *  struct __thrq_head {
 *      struct __thrq_elm *first;   // pointer to first elm
 *      struct __thrq_elm **last;   // pointer to last-elm's entry.next
 *  };
 **/
typedef TAILQ_HEAD(__thrq_head, __thrq_elm) thrq_head_t;

typedef int (*thrq_cmp_t)(void*, void*, int len);
typedef void (*thrq_clean_data_t)(void *data);

/* thread safe queue control block */
typedef struct {
    thrq_head_t         head;
    mux_t               lock;

    pthread_mutex_t     cond_lock;
    pthread_cond_t      cond;
    int                 cond_ok;

    int                 count;
    int                 max_size;

    thrq_clean_data_t   clean_data;
} thrq_cb_t;

/* lock/unlock thread queue */
#define THRQ_LOCK(thrq)         mux_lock(&thrq->lock)
#define THRQ_UNLOck(thrq)       mux_unlock(&thrq->lock)

/* is empty, not thread safe */  
#define THRQ_EMPTY(thrq)        TAILQ_EMPTY(&thrq->head)

/* first/begin element, not thread safe */
#define THRQ_FIRST(thrq)        TAILQ_FIRST(&thrq->head)
#define THRQ_BEGIN(thrq)        THRQ_FIRST(thrq)
/* last/end element, not thread safe */
#define THRQ_LAST(thrq)         TAILQ_LAST(&thrq->head, __thrq_head)
#define THRQ_END(Thrq)          THRQ_LAST(thrq)

/* element's next one, not thread safe */
#define THRQ_NEXT(elm)          TAILQ_NEXT(elm, entry)
/* element's previous one, not thread safe */
#define THRQ_PREV(elm)          TAILQ_PREV(elm, __thrq_head, entry)

/* for each element, not thread safe */
#define THRQ_FOREACH(var, thrq) \
    TAILQ_FOREACH(var, &thrq->head, entry)
/* for each element reversely, not thread safe */
#define THRQ_FOREACH_REVERSE(var, thrq) \
    TAILQ_FOREACH_REVERSE(var, &thrq->head, __thrq_head, entry)

#define THRQ_MAX_SIZE_DEF               1000

#define THRQ_ELM_DATA(elm, type)        ( *((type *)((elm)->data)) )

extern int thrq_init            (thrq_cb_t *thrq, int max_size, thrq_clean_data_t clean_data);
extern void thrq_clean          (thrq_cb_t *thrq);

extern int thrq_empty           (thrq_cb_t *thrq);

extern int thrq_count           (thrq_cb_t *thrq);

extern thrq_elm_t* thrq_first   (thrq_cb_t *thrq);
extern thrq_elm_t* thrq_last    (thrq_cb_t *thrq);

extern thrq_cb_t* thrq_create   (thrq_cb_t **thrq, int max_size, thrq_clean_data_t clean_data);
extern void thrq_destroy        (thrq_cb_t *thrq);

extern int thrq_insert_head     (thrq_cb_t *thrq, void *data, int len);
extern int thrq_insert_tail     (thrq_cb_t *thrq, void *data, int len);

extern int thrq_insert_after    (thrq_cb_t *thrq, thrq_elm_t *list_elm, void *data, int len);
extern int thrq_insert_before   (thrq_cb_t *thrq, thrq_elm_t *list_elm, void *data, int len);

extern int thrq_remove          (thrq_cb_t *thrq, thrq_elm_t *elm);
extern int thrq_concat          (thrq_cb_t *thrq1, thrq_cb_t *thrq2);

extern int thrq_send            (thrq_cb_t *thrq, void *data, int len);
extern int thrq_receive         (thrq_cb_t *thrq, void *buf, int max_size, double timeout);

extern thrq_elm_t* thrq_find    (thrq_cb_t *thrq, void *data, int len, thrq_cmp_t elm_cmp);

/* thread safe */
#define thrq_begin(thrq, data, len)     thrq_first(thrq)
#define thrq_end(thrq, data, len)       thrq_last(thrq)
#define thrq_push(thrq, data, len)      thrq_insert_head(thrq, data, len)
#define thrq_pop(thrq)                  thrq_remove(thrq, thrq_first(thrq))
#define thrq_append(thrq, data, len)    thrq_insert_tail(thrq, data, len)

#ifdef __cplusplus
}
#endif

#endif /* __THR_QUEUE__ */

