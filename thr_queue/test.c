#include "mux.h"
#include "thr_queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <tgmath.h>
#include <unistd.h>

thrq_cb_t *myq = NULL;

void* fn(void *arg)
{
    (void)arg;

    int buf;

    for (;;) {
        int ret = thrq_receive(myq, &buf, 4, 1.5);
        if (ret == ETIMEDOUT)
            printf("sub thread rcv timeout");
        else
            printf("sub thread get data: %d\n", buf);
    }

    return 0;
}


int elmcmp(void *e1, void *e2, int size)
{
    return ((*((int *)e1)) - (*((int *)e2)));
}

int main()
{
    int fdata, num = 1;

    if (thrq_create(&myq, 0) < 0) 
        printf("create & init queue: error\n");
    else
        printf("create & init queue: ok\n");

    if (thrq_empty(myq))
        printf("check empty: ok\n");
    else 
        printf("check empty: error\n");

    for (int i=0; i<4; i++) {
        if (thrq_insert_tail(myq, &num, 4) < 0)
            printf("insert tail: error\n");
        else
            printf("insert tail(%d): ok\n", num);
        num++;
    }

    num = -1;
    for (int i=0; i<4; i++) {
        if (thrq_insert_head(myq, &num, 4) < 0)
            printf("insert head: error\n");
        else
            printf("insert head(%d): ok\n", num);
        num--;
    }

    printf("get count(%d): ok\n", thrq_count(myq));
    printf("get first(%d): ok\n", THRQ_ELM_DATA(thrq_first(myq), int));
    printf("get last(%d): ok\n", THRQ_ELM_DATA(thrq_last(myq), int));

    num = 0;
    fdata = -1;
    if (thrq_insert_before(myq, thrq_find(myq, &fdata, 4, elmcmp), &num, 4) == 0) 
        printf("insert before: ok\n");
    else 
        printf("insert before: error\n");
    num = 0;
    fdata = 1;
    if (thrq_insert_after(myq, thrq_find(myq, &fdata, 4, elmcmp), &num, 4) == 0) 
        printf("insert after: ok\n");
    else 
        printf("insert after: error\n");

    printf("get count(%d): ok\n", thrq_count(myq));

    printf("remove -3 & 3\n");
    fdata = -3;
    thrq_remove(myq, thrq_find(myq, &fdata, 4, elmcmp), 0);
    fdata = 3;
    thrq_remove(myq, thrq_find(myq, &fdata, 4, elmcmp), 0);
    printf("get count(%d): ok\n", thrq_count(myq));

    thrq_elm_t *var;
    mux_lock(&myq->lock);
    THRQ_FOREACH(var, myq) {
        printf("elm = %d\n", *((int *)(var->data)));
    }    
    mux_unlock(&myq->lock);

    //thrq_clean(myq, 0);  // error: segment error
    //printf("get count(%d): ok\n", thrq_count(myq));
    pthread_t pth;
    printf("create thread for receive...\n");
    pthread_create(&pth, 0, fn, 0);
    thrq_elm_t *elm;
    for (;;) {
        THRQ_FOREACH(var, myq) {
            thrq_send(myq, &THRQ_ELM_DATA(elm, int), 4);
            sleep(3);
        }
    }

    exit(0);
}


