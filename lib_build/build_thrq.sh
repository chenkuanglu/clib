gcc -c -Wall thr_queue.c -lm -lpthread -I ../mux
ar crv libthrq.a thr_queue.o
