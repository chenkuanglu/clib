/**
 * @file    thr_queue.h
 * @author  ln
 * @brief   thread safe msg queue
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/queue.h>
#include <math.h>
#include "mux.h"

/**
 * declare user data type with list-head-struct: 
 *
 *  struct __thrq_elm {
 *      // user data1 (empty as it is lib definition)
 *      struct {
 *          struct __thrq_elm *prev;
 *          struct __thrq_elm *next;
 *      } entry;
 *      // user data2 (empty as it is lib definition)
 *  } thrq_elm_t;
 *
 * 'thrq_elm_t' is used as the base struct of user custom type.
 **/
typedef struct __thrq_elm {
    TAILQ_ENTRY(__thrq_elm) entry;
} thrq_elm_t;

/**
 * declare queue-head-struct with first & last pointer to the type of 'struct __thrq_elm': 
 *
 *  struct __thrq_head_base {
 *      struct __thrq_elm *first;
 *      struct __thrq_elm *last;
 *  };
 **/
TAILQ_HEAD(__thrq_head_base, __thrq_elm);

/* thread safe queue control block */
typedef struct {
    struct __thrq_head_base     head;
    mux_t                       lock;
    pthread_mutex_t             cond_lock;
    pthread_cond_t              cond;
} thrq_cb_t;

/* is empty */
#define thrq_empty(thrq)        TAILQ_EMPTY(&thrq->head)

/* first element */
#define thrq_first(thrq)        TAILQ_FIRST(&thrq->head)

/* end element */
#define thrq_last(thrq)         TAILQ_LAST(&thrq->head, __thrq_head_base)
#define thrq_end(thrq)          thrq_last(thrq)

// element's next one
#define thrq_next(elm)          TAILQ_NEXT(elm, entry)
// element's previous one
#define thrq_prev(elm)          TAILQ_PREV(elm, __thrq_head_bas, entry)

// insert before first element(head)
#define thrq_push(head, elm)    thrq_insert_tail(head, elm)
// remove first element
#define thrq_pop(head, elm)     thrq_remove(head, thrq_last(head))
// insert at the end
#define thrq_append(head, elm)  thrq_insert_tail(head, elm)

#define THRQ_FOREACH(var, head) \
    TAILQ_FOREACH(var, &head->head, entry)

#define THRQ_FOREACH_REVERSE(var, head) \
    TAILQ_FOREACH_REVERSE(var, &head->head, __thrq_head_base, entry)

typedef int (*pfn_thrq_cmp_t)(thrq_elm_t*, thrq_elm_t*);

extern int thrq_head_init(thrq_head_t *phead);

extern int thrq_insert_head(thrq_head_t *phead, thrq_elm_t *pelm);
extern int thrq_insert_tail(thrq_head_t *phead, thrq_elm_t *pelm);
extern int thrq_insert_after(thrq_head_t *phead, thrq_elm_t *listelm, thrq_elm_t *pelm);
extern int thrq_insert_before(thrq_head_t *phead, thrq_elm_t *listelm, thrq_elm_t *pelm);
extern int thrq_remove(thrq_head_t *phead, thrq_elm_t *pelm);
extern int thrq_concat(thrq_head_t *phead1, thrq_head_t *phead2);
extern thrq_elm_t* thrq_find(thrq_head_t *phead, thrq_elm_t *pelm, pfn_thrq_cmp_t elm_cmp);

extern int thrq_send(thrq_head_t *phead, thrq_elm_t *pelm);
extern int thrq_receive(thrq_head_t *phead, thrq_elm_t **ppelm, double time);

#ifdef __cplusplus
}
#endif

