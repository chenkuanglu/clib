#gcc -o test.out test.c ../mux/mux.c thr_queue.c -lm -lpthread -I ../mux
gcc -o thrq.out test_thrq.c -I../utils -I../iniparser -L../utils -lutils -lpthread -lm
