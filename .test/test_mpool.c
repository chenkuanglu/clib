#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mpool.h"

int main(void)
{
    mpool_t *mp = mpool_new(0, 0);
    int count = 0;
    int num = 0;

    while (1) {
        char *s = (char *)mpool_malloc(mp, 1024*1024);
        if (s) {
            printf("printf block %02d, old data = %02d\n", count, (int)s[0]);
            count++;
            num = (num + 1) % 20;
            memset(s, num, 1024*1024);
        } else {
            printf("malloc error !!!!!!!!!!!!!!!!!!!!!!!!!\n");
        }

        printf("sleep 1\n");
        usleep(50*1000);
        mpool_free(mp, s);
        count--;
    }

    return 0;
}

