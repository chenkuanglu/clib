#gcc -o test.out test.c ../mux/mux.c thr_queue.c -lm -lpthread -I ../mux
gcc -o test.out test.c -L. -lthrq -lmux -lm -lpthread -I ../mux
