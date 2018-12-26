# libmux.a
gcc -c -Wall ../mux/mux.c -I../mux
ar crv libmux.a mux.o

# libcstr.a
gcc -c -Wall ../cstr/cstr.c -I../mux -I../cstr 
ar crv libcstr.a cstr.o

# libthrq.a
gcc -c -Wall ../thr_queue/thr_queue.c -I../mux -I../cstr -I../thr_queue -lpthread -lm
ar crv libthrq.a thr_queue.o

# libdict.a
gcc -c -Wall ../iniparser/dictionary.c -I../cstr -I../thr_queue -I../iniparser -I../mux
ar crv libdict.a dictionary.o

# libiniparser.a
gcc -c -Wall ../iniparser/iniparser.c -I../cstr -I../thr_queue -I../iniparser -I../mux
ar crv libiniparser.a iniparser.o

# libargparser.a
gcc -c -Wall ../argparser/argparser.c -I../cstr -I../thr_queue -I../argparser -I../mux
ar crv libargparser.a argparser.o

# libutl.a
ar crv libutl.a *.o

rm -f *.o

