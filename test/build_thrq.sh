#gcc -o test.out test.c ../mux/mux.c thr_queue.c -lm -lpthread -I ../mux
gcc -o thrq.out test_thrq.c -I../include -L../lib_build -lthrq -lmux -lpthread -lm
