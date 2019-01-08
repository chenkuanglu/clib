# debug
gcc -o dbg.out dbg.c iniparser/xconfig.c argparser/argparser.c -DDEBUG -I./utils -I./iniparser -I./argparser -L./utils -lcurses -lutils -lpthread -lm

# release
#gcc -o dbg.out dbg.c iniparser/xconfig.c argparser/argparser.c -I./utils -I./iniparser -I./argparser -L./utils -lcurses -lutils -lpthread -lm
