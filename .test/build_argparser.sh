gcc -Wall -o argparser.out test_argparser.c -I../cstr -I../thr_queue -I../argparser -I../mux -L../lib_build -largparser -lthrq -lcstr -lmux -lpthread -lm

# ./test.out -o www.baidu.com -A -C 32 --chip-num 32 --max-temp 123 --write-reg 1 2 3
