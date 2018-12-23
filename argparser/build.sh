# ./test.out -o www.baidu.com -A -C 32 --chip-num 32 --max-temp 123 --write-reg 1 2 3
gcc -Wall -o test.out test.c argparser.c ../mux/mux.c ../cstr/cstr.c ../thr_queue/thr_queue.c -I. -I../cstr -I../thr_queue -I../mux -lpthread -lm
