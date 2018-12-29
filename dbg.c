// debugger 

#include <curses.h>
#include "thr_queue.h"
#include "xconfig.h"
#include "argparser.h"

#define DBG_VERSION     "0.0.1"

// type of main control block
typedef struct {
    char*           inifile;    // *.ini file
    xconfig_t*      config;     // config info come from ini & cmdline
    int             run_mode;
} dbg_cb_t;

enum {
    RUN_MODE_NORMAL,    // normal 
    RUN_MODE_HELP,      // print help info, '-h x' or 'x --help'
    RUN_MODE_VERSION,   // print version
};

// main control block
dbg_cb_t *dbg = NULL;

static int help(long id)
{
    switch (id) {
        case 0:
            printfd("help self\n");
            break;
        case 'v':
            printfd("help v\n");
            break;
        default:
            break;
    }
    return 0;
}

static int cmdline_proc(long id, char **param, int num)
{
    if ((param) && (num > 0) && (strcmp(param[0], "--help") == 0)) {
        dbg->run_mode = RUN_MODE_HELP;
        help(id);
        return -1;  // stop parse
    }

    switch (id) {
        case 'h':
            dbg->run_mode = RUN_MODE_HELP;
            help(0);
            break;
        case 'v':
            dbg->run_mode = RUN_MODE_VERSION;
            printfd(CCL_GREEN "Debugger For Serial\n" CCL_END);
            printfd(CCL_GREEN "Version %s\n" CCL_END, DBG_VERSION);
            break;
        case 'd':
            free(dbg->config->dev_name);
            dbg->config->dev_name = NULL;
            dbg->config->dev_name = strdup(param[0]);
            printfd(CCL_CYAN "Set serial device: '%s'\n" CCL_END, dbg->config->dev_name);
            break;
        case 1001:  // --baud
            dbg->config->baudrate = strtol(param[0], NULL, 0);
            printfd("set baud: %d\n", dbg->config->baudrate);
            break;
        default:
            printfd(CCL_YELLOW "Unknown arg id %d\n" CCL_END, id);
            break;
    }

    return 0;
}

static int dbg_run(void)
{
    printfd(CCL_GREEN "Debugger For Serial\n" CCL_END);
    printfd(CCL_GREEN "Version %s\n" CCL_END, DBG_VERSION);

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
    // Init main control block
    dbg = (dbg_cb_t *)malloc(sizeof(dbg_cb_t));
    if (dbg == NULL) {
        printfd(CCL_RED "Fail to malloc 'dbg_cb_t'\n" CCL_END);
        return -1;
    }
    dbg->inifile = strdup("./dbg.ini");
    dbg->run_mode = RUN_MODE_NORMAL;

    // Open & parse ini file
    dbg->config = xconfig_new();
    if (dbg->config == NULL) {
        printfd(CCL_RED "Fail to new 'xconfig_t'\n" CCL_END);
        return -1;
    }
#ifdef DEBUG
    printfd("\n");
    printfd(CCL_CYAN "Load ini file '%s'\n" CCL_END, dbg->inifile);
#endif
    if (xconfig_load(dbg->config, dbg->inifile) < 0) {
        printfd(CCL_RED "Fail to load ini file '%s'\n" CCL_END, dbg->inifile);
        return -1;
    }
    
    // Parse command line 
    argparser_t* cmdline = argparser_new(argc, argv);
    if (cmdline == NULL) {
        printfd(CCL_RED "Fail to new 'argparser_t'\n" CCL_END);
        return -1;
    }
    argparser_add(cmdline, "-h", 'h', 0);
    argparser_add(cmdline, "-v", 'v', 0);
    argparser_add(cmdline, "-d", 'd', 1);
    argparser_add(cmdline, "--baud", 1001, 1);
    if (argparser_parse(cmdline, cmdline_proc)) {
        printfd(CCL_RED "Fail to process command line\n" CCL_END);
        return -1;
    }

    switch (dbg->run_mode) {
        case RUN_MODE_NORMAL:
            dbg_run();          // Run the debugger, main thread
            break;

        case RUN_MODE_HELP:
        case RUN_MODE_VERSION:
        default:
#ifdef DEBUG
            printfd("\n");
            printfd(CCL_YELLOW "Debugger exit.\n" CCL_END);
#endif
            break;
    }

    // Release resources
    argparser_delete(cmdline);
    free(dbg->inifile);
    xconfig_delete(dbg->config);
    free(dbg);

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
