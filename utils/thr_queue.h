/**
 * @file    thr_queue.h
 * @author  ln
 * @brief   thread safe msg queue
 **/

#ifndef __THR_QUEUE__
#define __THR_QUEUE__

#include <errno.h>
#include <sys/queue.h>
#include <pthread.h>
#include "mpool.h"
#include "mux.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * declare user data type with list head struct: 
 *
 *  struct __thrq_elm {
 *      struct {
 *          struct __thrq_elm *next;
 *          struct __thrq_elm **prev;   // a pointer to pointer!
 *      } entry;
 *      // user data (empty as it is lib definition)
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

typedef int     (*thrq_cmp_data_t)      (const void*, const void*, size_t len);
typedef void*   (*thrq_copy_data_t)     (void*, const void*, size_t len);
typedef void    (*thrq_destroy_data_t)  (void *data);

/* thread safe queue control block */
typedef struct {
    mpool_t             mpool;

    thrq_head_t         head;           /* list header */
    mux_t               lock;           /* data lock */

    pthread_condattr_t  cond_attr;
    pthread_mutex_t     cond_lock;      /* condition lock */
    pthread_cond_t      cond;

    int                 count;
    int                 max_size;

    thrq_destroy_data_t destroy_data;
    thrq_copy_data_t    copy_data;
    thrq_cmp_data_t     cmp_elm;
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

#define THRQ_CONTAINER_OF(ptr)          ((thrq_elm_t *)((char *)(ptr) - ((size_t)&((thrq_elm_t *)0)->data)))

/* parse element/data pointer to data/element pointer */
#define THRQ_DATA_ELM(data)             ( THRQ_CONTAINER_OF(data) )
#define THRQ_ELM_DATA(elm, data_type)   ( *((data_type *)((elm)->data)) )

extern int          thrq_init           (thrq_cb_t *thrq);
extern void         thrq_clean          (thrq_cb_t *thrq);

extern void         thrq_set_free       (thrq_cb_t *thrq, thrq_destroy_data_t destroy_data);
extern void         thrq_set_copy       (thrq_cb_t *thrq, thrq_copy_data_t copy_data);
extern void         thrq_set_compare    (thrq_cb_t *thrq, thrq_cmp_data_t compare_data);
extern void         thrq_set_maxsize    (thrq_cb_t *thrq, int max_size);
extern int          thrq_set_mpool      (thrq_cb_t *thrq, size_t n, size_t data_size);

extern int          thrq_empty          (thrq_cb_t *thrq);
extern int          thrq_count          (thrq_cb_t *thrq);

extern thrq_cb_t*   thrq_create         (thrq_cb_t **thrq);
extern void         thrq_destroy        (thrq_cb_t *thrq);

extern int          thrq_insert_head    (thrq_cb_t *thrq, void *data, int len);
extern int          thrq_insert_tail    (thrq_cb_t *thrq, void *data, int len);

extern thrq_elm_t*  thrq_first          (thrq_cb_t *thrq);
extern thrq_elm_t*  thrq_last           (thrq_cb_t *thrq);

extern thrq_elm_t*  thrq_find           (thrq_cb_t *thrq, void *data, int len);

extern int          thrq_insert_after   (thrq_cb_t *thrq, thrq_elm_t *list_elm, void *data, int len);
extern int          thrq_insert_before  (thrq_cb_t *thrq, thrq_elm_t *list_elm, void *data, int len);

extern int          thrq_remove         (thrq_cb_t *thrq, thrq_elm_t *elm);

extern int          thrq_concat         (thrq_cb_t *thrq1, thrq_cb_t *thrq2);

extern int          thrq_send           (thrq_cb_t *thrq, void *data, int len);
extern int          thrq_receive        (thrq_cb_t *thrq, void *buf, int max_size, double timeout);

#define thrq_new(pp)                    thrq_create((pp))

/* thread safe */
#define thrq_begin(thrq)                thrq_first(thrq)
#define thrq_end(thrq)                  thrq_last(thrq)
#define thrq_push(thrq, data, len)      thrq_insert_head(thrq, data, len)
#define thrq_pop(thrq)                  thrq_remove(thrq, thrq_first(thrq))
#define thrq_append(thrq, data, len)    thrq_insert_tail(thrq, data, len)

#ifdef __cplusplus
}
#endif

#endif /* __THR_QUEUE__ */

