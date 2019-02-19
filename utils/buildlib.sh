gcc -c -Wall -DDEBUG thrq.c que.c mux.c cstr.c log.c mpool.c
ar crv libutils.a *.o
rm -f *.o
