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
            printf("sub thread rcv timeout\n");
        else if (ret == 0)
            printf("sub thread get data: %d\n", buf);
        else 
            printf("sub thread error\n");
    }

    return 0;
}


int elmcmp(const void *e1, const void *e2, size_t size)
{
    return ((*((int *)e1)) - (*((int *)e2)));
}

int main()
{
    int fdata, num = 1;

    // create & init
    if (thrq_create(&myq) < 0) 
        printf("create & init queue: error\n");
    else
        printf("create & init queue: ok\n");

    //thrq_set_compare(myq, elmcmp);

    // empty
    if (thrq_empty(myq))
        printf("check empty: ok\n");
    else 
        printf("check empty: error\n");

    // insert tail 
    for (int i=0; i<4; i++) {
        if (thrq_insert_tail(myq, &num, 4) < 0)
            printf("insert tail: error\n");
        else
            printf("insert tail(%d): ok\n", num);
        num++;
    }

    // insert head
    num = -1;
    for (int i=0; i<4; i++) {
        if (thrq_insert_head(myq, &num, 4) < 0)
            printf("insert head: error\n");
        else
            printf("insert head(%d): ok\n", num);
        num--;
    }

    // count & first & last
    printf("get count(%d): ok\n", thrq_count(myq));
    printf("get first(%d): ok\n", THRQ_ELM_DATA(thrq_first(myq), int));
    printf("get last(%d): ok\n", THRQ_ELM_DATA(thrq_last(myq), int));

    // insert before & after
    num = 0;
    fdata = -1;
    if (thrq_insert_before(myq, thrq_find(myq, &fdata, 4), &num, 4) == 0) 
        printf("insert before -1(0): ok\n");
    else 
        printf("insert before: error\n");
    num = 0;
    fdata = 1;
    if (thrq_insert_after(myq, thrq_find(myq, &fdata, 4), &num, 4) == 0) 
        printf("insert after 1(0): ok\n");
    else 
        printf("insert after: error\n");

    printf("get count(%d): ok\n", thrq_count(myq));

    // remove & find
    printf("remove -3 & 3\n");
    fdata = -3;
    thrq_remove(myq, thrq_find(myq, &fdata, 4));
    fdata = 3;
    thrq_remove(myq, thrq_find(myq, &fdata, 4));
    printf("get count(%d): ok\n", thrq_count(myq));

    thrq_elm_t *var;
    mux_lock(&myq->lock);
    THRQ_FOREACH(var, myq) {
        printf("elm = %d\n", *((int *)(var->data)));
    }    
    mux_unlock(&myq->lock);

    // clean
    //thrq_clean(myq);  
    //printf("get count(%d): ok\n", thrq_count(myq));

    // send & receive
    pthread_t pth;
    printf("create thread for receive...\n");
    pthread_create(&pth, 0, fn, 0);
    int snd_data = 1000;
    for (;;) {
        thrq_send(myq, &snd_data, 4);
        snd_data++;
        sleep(6);
    }

    exit(0);
}


