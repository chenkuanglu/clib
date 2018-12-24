# libmux.a
gcc -c -Wall ../mux/mux.c -I../include
ar crv libmux.a mux.o

# libcstr.a
gcc -c -Wall ../cstr/cstr.c -I../include 
ar crv libcstr.a cstr.o

# libthrq.a
gcc -c -Wall ../thr_queue/thr_queue.c -I../include -lpthread -lm
ar crv libthrq.a thr_queue.o

# libdict.a
gcc -c -Wall ../iniparser/dictionary.c -I../include 
ar crv libdict.a dictionary.o

# libiniparser.a
gcc -c -Wall ../iniparser/iniparser.c -I../include 
ar crv libiniparser.a iniparser.o

# libargparser.a
gcc -c -Wall ../argparser/argparser.c -I../include 
ar crv libargparser.a argparser.o

rm -f *.o

