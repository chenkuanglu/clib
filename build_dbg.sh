# debug
gcc -o dbg-debug.out dbg.c iniparser/xconfig.c iniparser/dictionary.c iniparser/iniparser.c argparser/argparser.c -DDEBUG -I./utils -I./iniparser -I./argparser -L./utils -lcurses -lutils -lpthread -lm

# release
gcc -o dbg.out dbg.c iniparser/xconfig.c iniparser/dictionary.c iniparser/iniparser.c argparser/argparser.c -I./utils -I./iniparser -I./argparser -L./utils -lcurses -lutils -lpthread -lm
