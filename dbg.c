// debugger 

#include <curses.h>
#include "thr_queue.h"
#include "xconfig.h"
#include "argparser.h"

// type of main control block
typedef struct {
    char*           inifile;    // *.ini file
    xconfig_t*      config;     // config info
    argparser_t*    cmdline;    // cmdline arguments
    int             run_mode;
} dbg_cb_t;

enum {
    RUN_MODE_NORMAL,    // normal 
    RUN_MODE_HELP,      // print help info, '-h x' or 'x --help'
    RUN_MODE_VERSION,   // print version
};

static int cmdline_proc(long id, char **param, int num)
{
    (void)num;

    switch (id) {
    case 'h':
        printfd("print help info...\n");
        break;
    case 'v':
        printfd("print version info...\n");
        break;
    case 'd':
        printfd("set serial device: %s\n", param[0]);
        break;
    case 1001:  // --baud
        printfd("set baud: %s\n", param[0]);
        break;
    default:
        printfd(CCL_YELLOW "unknown arg id %d\n" CCL_END, id);
        break;
    }

    return 0;
}

static int dbg_run(dbg_cb_t *dbg)
{
    (void)dbg;

    thrq_cb_t *myq = thrq_create(NULL);
    if (myq == NULL) {
        printfd(CCL_RED "Fail to create queue\n" CCL_END);
    }
    
    double tmout = 2;
    char *buf[32];
    for (;;) {
        int ret = thrq_receive(myq, &buf, sizeof(buf), tmout);
        if (ret == ETIMEDOUT) {
            printfd(CCL_YELLOW "%.2fs timeout...\n" CCL_END, tmout);
        } else {
            printfd(CCL_RED "error occured: %s\n" CCL_END, strerror(errno));
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    printfd(CCL_GREEN "Debugger For Serial\n" CCL_END);
    printfd(CCL_GREEN "Version 0.0.1\n" CCL_END);

    // Init main control block
    dbg_cb_t *dbg = (dbg_cb_t *)malloc(sizeof(dbg_cb_t));
    if (dbg == NULL) {
        printfd(CCL_RED "Fail to malloc 'dbg_cb_t'\n" CCL_END);
        return -1;
    }
    dbg->inifile = strdup("dbg.ini");
    dbg->run_mode = RUN_MODE_NORMAL;

    // Open & parse ini file
    dbg->config = xconfig_new();
    if (dbg->config == NULL) {
        printfd(CCL_RED "Fail to new 'xconfig_t'\n" CCL_END);
        return -1;
    }
    printfd("\n");
    printfd(CCL_CYAN "Load ini file '%s'\n" CCL_END, dbg->inifile);
    if (xconfig_load(dbg->config, dbg->inifile) < 0) {
        printfd(CCL_RED "Fail to load ini file '%s'\n" CCL_END, dbg->inifile);
        return -1;
    }
    
    // Parse command line 
    dbg->cmdline = argparser_new(argc, argv);
    if (dbg->cmdline == NULL) {
        printfd(CCL_RED "Fail to new 'argparser_t'\n" CCL_END);
    }
    argparser_add(dbg->cmdline, "-h", 'h', 0);
    argparser_add(dbg->cmdline, "--help", 'h', 0);
    argparser_add(dbg->cmdline, "-v", 'v', 0);
    argparser_add(dbg->cmdline, "-d", 'd', 1);
    argparser_add(dbg->cmdline, "--baud", 1001, 1);
    if (argparser_parse(dbg->cmdline, cmdline_proc)) {
        printfd(CCL_RED "Fail to process command line\n" CCL_END);
    }

    // Run the debugger 
    dbg_run(dbg);

    // Release resources
    free(dbg->inifile);
    xconfig_delete(dbg->config);
    argparser_delete(dbg->cmdline);

    return 0;
}


//initscr();
//WINDOW *mywin = newwin(10, 10, 2, 2);
////box(mywin, ACS_VLINE, ACS_HLINE);
//box(mywin, 0, 0);
////move(LINES/2 + 1, COLS/2 - sizeof("hello, world!")/2);
//move(4, 4);
//waddstr(mywin, "hello, world!");

//hline(0, 20);
//vline(0, 20);
//wrefresh(mywin);
//wrefresh(stdscr);
//getch();

//wclear(mywin);
//delwin(mywin);
//endwin();
