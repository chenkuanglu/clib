gcc -c -Wall dictionary.c thr_queue.c mux.c cstr.c log.c mpool.c
ar crv libutils.a *.o
rm -f *.o
