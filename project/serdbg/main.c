// serial debugger 

#include "xconfig.h"
#include <curses.h>

int main(void)
{
    const char *filename = "config.ini";
    xconfig_t *config = xconfig_new();
    if (config == NULL) {
        printfd(CCL_RED "xconfig: fail to create\n" CCL_END);
        return -1;
    }
    
    int res = xconfig_load(config, filename);
    if (res < 0) {
        printfd(CCL_RED "xconfig: fail to load file '%s'\n" CCL_END, filename);
        return -1;
    }
    printfd(CCL_YELLOW "xconfig: load file '%s'\n" CCL_END, filename);
    
    printfd(CCL_CYAN "baudrate = %d\n" CCL_END, config->baudrate);
    printfd(CCL_CYAN "data_bits = %d\n" CCL_END, config->data_bits);
    char *s = NULL;
    if (config->check_flag == None) 
        s = "None";
    else if (config->check_flag == Odd) 
        s = "Odd";
    else if (config->check_flag == Even)
        s = "Even";
    else 
        s = "Unkown";
    printfd(CCL_CYAN "check_flag = %s\n" CCL_END, s);
    printfd(CCL_CYAN "stop_bits = %d\n" CCL_END, config->stop_bits);
    
    xconfig_delete(config);

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

    return 0;
}

