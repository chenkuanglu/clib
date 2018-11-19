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

/* thread safe queue control block */
typedef struct {
    thrq_head_t         head;
    mux_t               lock;
    pthread_mutex_t     cond_lock;
    pthread_cond_t      cond;
} thrq_cb_t;

/* is empty */  not safe
#define thrq_empty(thrq)        TAILQ_EMPTY(&thrq->head)

/* first/begin element */  not safe
#define thrq_first(thrq)        TAILQ_FIRST(&thrq->head)
#define thrq_begin(thrq)        thrq_first(thrq)

/* last/end element */
#define thrq_last(thrq)         TAILQ_LAST(&thrq->head, __thrq_head)
#define thrq_end(thrq)          thrq_last(thrq)

/* element's next one */  not safe
#define thrq_next(elm)          TAILQ_NEXT(elm, entry)

/* element's previous one */  not safe
#define thrq_prev(elm)          TAILQ_PREV(elm, __thrq_head, entry)

/* insert before first element(head) */
#define thrq_push(thrq, elm)    thrq_insert_head(thrq, elm)

/* remove first element */
#define thrq_pop(thrq, elm)     thrq_remove(head, thrq_first(head))

/* insert at the end */
#define thrq_append(head, elm)  thrq_insert_tail(head, elm)

/* for each element */
#define THRQ_FOREACH(var, thrq) \
    TAILQ_FOREACH(var, &thrq->head, entry)

/* for each element reversely */
#define THRQ_FOREACH_REVERSE(var, thrq) \
    TAILQ_FOREACH_REVERSE(var, &thrq->head, __thrq_head, entry)

typedef int (*thrq_cmp_t)(thrq_elm_t*, thrq_elm_t*);

extern int thrq_head_init       (thrq_head_t *thrq);

extern int thrq_insert_head     (thrq_cb_t *thrq, thrq_elm_t *elm);
extern int thrq_insert_tail     (thrq_cb_t *thrq, thrq_elm_t *elm);

extern int thrq_insert_after    (thrq_cb_t *thrq, thrq_elm_t *list_elm, thrq_elm_t *elm);
extern int thrq_insert_before   (thrq_cb_t *thrq, thrq_elm_t *list_elm, thrq_elm_t *elm);

extern int thrq_remove          (thrq_cb_t *thrq, thrq_elm_t *elm);
extern int thrq_concat          (thrq_cb_t *thrq1, thrq_cb_t *thrq2);

extern int thrq_send            (thrq_cb_t *thrq, thrq_elm_t *elm, int size);
extern int thrq_receive         (thrq_cb_t *thrq, void *buf, int size, double time);

extern thrq_elm_t* thrq_find    (thrq_cb_t *thrq, thrq_elm_t *elm, thrq_cmp_t elm_cmp);

#ifdef __cplusplus
}
#endif

