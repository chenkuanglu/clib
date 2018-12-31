// debugger 

#include <curses.h>
#include <signal.h>
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

// help infomation
char help_tbl[] = CCL_WHITE_HL "\
Usage: sdbg [-h] [-v] [-d <file>] [<option> --help]\n\n\
    -h                  Display this information\n\
    -v                  Display version\n\
    -d <file>           Set serial device\n\
    --baud <integer>    Set baudrate\n" CCL_END;

char help_version[] ="\
Usage: sdbg -v\n\
    Display version\n";

char help_help[] ="\
Usage: sdbg -h\n\
    Display help\n";

char help_baud[] ="\
Usage: sdbg --baud <integer>\n\
    Set baudrate\n";

char help_device[] ="\
Usage: sdbg -d <file>\n\
    Set serial device\n";

// main control block
dbg_cb_t *dbg = NULL;

void signal_handler(int signal)
{
    switch (signal) {
        case SIGINT:
            printf("\n");
            printfd(CCL_YELLOW "Debugger exit.\n" CCL_END);
            exit(0);
            break;
        default:
            break;
    }
}

static int help(long id)
{
    switch (id) {
        case 0:
            printf("%s\n", help_tbl);
            break;
        case 'h':
            printf("%s\n", help_help);
            break;
        case 'v':
            printf("%s\n", help_version);
            break;
        case 'd':
            printf("%s\n", help_device);
            break;
        case 1001:
            printf("%s\n", help_baud);
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
            printfd(CCL_GREEN "Serial Debugger Version %s\n" CCL_END, DBG_VERSION);
            break;
        case 'd':
            free(dbg->config->dev_name);
            dbg->config->dev_name = NULL;
            dbg->config->dev_name = strdup(param[0]);
#ifdef DEBUG
            printfd(CCL_CYAN "Set serial device: '%s'\n" CCL_END, dbg->config->dev_name);
#endif
            break;
        case 1001:  // --baud
            dbg->config->baudrate = strtol(param[0], NULL, 0);
#ifdef DEBUG
            printfd(CCL_CYAN "Set baudrate: %d\n" CCL_END, dbg->config->baudrate);
#endif
            break;
        default:
            printfd(CCL_YELLOW "Unknown arg id %d\n" CCL_END, id);
            break;
    }

    return 0;
}

void draw(void)
{
    initscr();
    cbreak();
    noecho();
    nl();

    char msgbx[] = "message box";
    int y, x, height = 10, width = 30;
    y = (LINES - height) / 2;
    x = (COLS - width) / 2;

    WINDOW *mywin = newwin(height, width, y, x);

    wattron(mywin, A_REVERSE);
    //box(mywin, ACS_VLINE, ACS_HLINE);
    box(mywin, ' ', ' ');
    wmove(mywin, 0, (width-(int)strlen(msgbx)) / 2);
    wprintw(mywin, "%s",msgbx);
    wattroff(mywin, A_REVERSE);

    wmove(mywin, 1, 1);
    waddstr(mywin, "hello, world!");
    wmove(mywin, 2, 1);
    whline(mywin, 0, width-2);

    wmove(mywin, 3, 1);
    wrefresh(mywin);

    keypad(mywin, TRUE);
    int ch;
    while ((ch = wgetch(mywin)) != '\n') {
        switch (ch) {
            case KEY_LEFT:
                if (x) {
                    redrawwin(stdscr);
                    mvwin(mywin, y, --x);
                    refresh();
                }
                break;
            case KEY_RIGHT:
                if (x+width < COLS) {
                    redrawwin(stdscr);
                    mvwin(mywin, y, ++x);
                    refresh();
                }
                break;
            case KEY_UP:
                if (y) {
                    redrawwin(stdscr);
                    mvwin(mywin, --y, x);
                    refresh();
                }
                break;
            case KEY_DOWN:
                if (y+height < LINES) {
                    redrawwin(stdscr);
                    mvwin(mywin, ++y, x);
                    refresh();
                }
                break;
            default:
                break;
        }
    }

    delwin(mywin);
    endwin();
}

static int dbg_run(void)
{
    draw();

    printfd(CCL_GREEN "Serial Debugger Version %s\n" CCL_END, DBG_VERSION);

    thrq_cb_t *myq = thrq_create(NULL);
    if (myq == NULL) {
        printfd(CCL_RED "Fail to create queue\n" CCL_END);
    }
    
    double tmout = 2.0;
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
    signal(SIGINT, signal_handler);

    // Init main control block
    dbg = (dbg_cb_t *)malloc(sizeof(dbg_cb_t));
    if (dbg == NULL) {
        printfd(CCL_RED "Fail to malloc 'dbg_cb_t'\n" CCL_END);
        return -1;
    }
    dbg->inifile = strdup("./sdbg.ini");
    dbg->run_mode = RUN_MODE_NORMAL;

    // Open & parse ini file
    dbg->config = xconfig_new();
    if (dbg->config == NULL) {
        printfd(CCL_RED "Fail to new 'xconfig_t'\n" CCL_END);
        return -1;
    }
#ifdef DEBUG
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
    if (argparser_parse(cmdline, cmdline_proc) < 0) {
        printfd(CCL_RED "Fail to process command line\n" CCL_END);
        return -1;
    }

    switch (dbg->run_mode) {
        case RUN_MODE_NORMAL:
            dbg_run();          // Run the debugger
            break;

        case RUN_MODE_HELP:
        case RUN_MODE_VERSION:
        default:
#ifdef DEBUG
            printfd(CCL_YELLOW "Debugger exit.\n" CCL_END);
#endif
            break;
    }

    return 0;
}

