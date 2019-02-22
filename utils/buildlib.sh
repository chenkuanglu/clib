gcc -c -Wall -DDEBUG thrq.c mux.c cstr.c log.c mpool.c popen_p.c
ar crv libutils.a *.o
rm -f *.o
