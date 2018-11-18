#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mux.h"

mux_t mux;

void* fn(void *arg)
{
    (void)arg;

    if (mux_lock(&mux) == 0) {
        printf("thread lock success\n");
    }

    if (mux_unlock(&mux) != 0) {
        printf("error\n");
        exit(1);
    }

    printf("thread unlock success\n");

    return 0;
}

int main()
{
    mux_init(&mux);

    // lock 2 times
    if (mux_lock(&mux) != 0) {
        printf("error\n");
        exit(1);
    }
    printf("lock 1 times\n");

    if (mux_lock(&mux) != 0) {
        printf("error\n");
        exit(1);
    }
    printf("lock 2 times\n");

    // unlock 1 times
    if (mux_unlock(&mux) != 0) {
        printf("error\n");
        exit(1);
    }
    printf("unlock 1 times\n");

    pthread_t pth;

    printf("create thread\n");
    pthread_create(&pth, 0, fn, 0);
    
    printf("wait 1 second ...\n");
    sleep(1);

    // unlock 2 times
    if (mux_unlock(&mux) != 0) {
        printf("error\n");
        exit(1);
    }
    printf("unlock 2 times\n");

    pthread_join(pth, 0);

    exit(0);
}


