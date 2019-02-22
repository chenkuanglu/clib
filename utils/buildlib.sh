gcc -c -Wall -DDEBUG thrq.c que.c mux.c cstr.c log.c mpool.c popen_p.c
ar crv libutils.a *.o
rm -f *.o
