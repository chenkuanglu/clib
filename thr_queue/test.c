#include "mux.h"
#include "thr_queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <tgmath.h>

thrq_cb_t *myq = NULL;

void* fn(void *arg)
{
    (void)arg;

    printf("thread unlock success\n");

    return 0;
}


int main()
{
    int num = 101;

    thrq_create(&myq, 0);

    thrq_insert_tail(myq, &num, 4);
    num++;
    thrq_insert_tail(myq, &num, 4);
    num++;
    thrq_insert_tail(myq, &num, 4);
    num++;
    thrq_insert_tail(myq, &num, 4);
    num++;

    printf("count = %d\n", thrq_count(myq));
    printf("head = %d\n", *((int *)((thrq_last(myq))->data)));


    //pthread_t pth;

    //printf("create thread\n");
    //pthread_create(&pth, 0, fn, 0);
    //
    //printf("wait 1 second ...\n");
    //sleep(1);

    // unlock 2 times

    //pthread_join(pth, 0);

    exit(0);
}


