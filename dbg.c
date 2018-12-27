// debugger 

#include "xconfig.h"
#include <curses.h>

// type of main control block
typedef struct {
    char*           inifile;    // *.ini file
    xconfig_t*      config;     // config info
} dbg_cb_t;

int dbg_run(dbg_cb_t *dbg)
{
    (void)dbg;
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

    // Open & parser ini file
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
    
    dbg_run(dbg);

    initscr();
    WINDOW *mywin = newwin(10, 10, 2, 2);
    //box(mywin, ACS_VLINE, ACS_HLINE);
    box(mywin, 0, 0);
    //move(LINES/2 + 1, COLS/2 - sizeof("hello, world!")/2);
    move(4, 4);
    waddstr(mywin, "hello, world!");

    hline(0, 20);
    vline(0, 20);
    wrefresh(mywin);
    wrefresh(stdscr);
    getch();

    wclear(mywin);
    delwin(mywin);
    endwin();
    
    xconfig_delete(dbg->config);

    return 0;
}

