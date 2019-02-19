// Debugger 

#include <signal.h>
#include "thr_queue.h"
#include "xconfig.h"
#include "argparser.h"

#define DBG_VERSION     "0.0.1"

// type of main control block
typedef struct {
    char*           inifile;    // *.ini file
    xconfig_t*      config;     // config info come from ini & cmdline
} dbg_cb_t;

// help infomation
char help_tbl[] = "\
Usage: dbg [options] [<options> --help]\n\
Options:\
    -h                  Display this information\n\
    -v                  Display version\n\
    -d <file>           Set serial device\n\
    --baud <integer>    Set baudrate\n";

char help_version[] ="\
Usage: dbg -v\n\
    Display version\n";

char help_help[] ="\
Usage: dbg -h\n\
    Display help\n";

char help_baud[] ="\
Usage: dbg --baud <integer>\n\
    Set baudrate\n";

char help_device[] ="\
Usage: dbg -d <file>\n\
    Set serial device\n";

// main control block
dbg_cb_t *dbg = NULL;

void signal_handler(int signal)
{
    switch (signal) {
        case SIGINT:
            printf("\n");
            logd("Ctrl-C, Debugger exit.\n");
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
        help(id);
        exit(0);
    }

    switch (id) {
        case 'h':
            help(0);
            exit(0);
            break;
        case 'v':
            printf("Debugger Version %s\n", DBG_VERSION);
            exit(0);
            break;
        case 'd':
            //free(dbg->config->dev_name);
            //dbg->config->dev_name = NULL;
            //dbg->config->dev_name = strdup(param[0]);
            //dbg_setdev();
            //logi("Set serial device: '%s'\n", dbg->config->dev_name);
            break;
        case 1001:  // --baud
            //dbg->config->baudrate = strtol(param[0], NULL, 0);
            //dbg_setbaud();
            //logi("Set baudrate: %d\n", dbg->config->baudrate);
            break;
        default:
            loge("Unknown arg id %d\n", id);
            exit(0);
            break;
    }

    return 0;
}

static int dbg_run(void)
{
#ifdef USE_GUI
#endif

    logn("Serial Debugger Version %s\n", DBG_VERSION);

    thrq_cb_t *myq = thrq_create(NULL);
    if (myq == NULL) {
        loge("Fail to create queue\n");
    }
    
    double tmout = 1.9;
    char *buf[32];
    for (;;) {
        int ret = thrq_receive(myq, &buf, sizeof(buf), tmout);
        if (ret == ETIMEDOUT) {
            logw("%.2fs timeout...\n", tmout);
        } else {
            loge("error occured: %s\n", strerror(errno));
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
        loge("Fail to new 'dbg_cb_t'\n");
        return -1;
    }
    dbg->inifile = strdup("./dbg.ini");

    // Open & parse ini file
    dbg->config = xconfig_new();
    if (dbg->config == NULL) {
        loge("Fail to new 'xconfig_t'\n");
        return -1;
    }
    logd("Load ini file '%s'\n", dbg->inifile);
    if (xconfig_load(dbg->config, dbg->inifile) < 0) {
        logw("Fail to load ini file '%s'\n", dbg->inifile);
    }
    
    // Parse command line 
    argparser_t* cmdline = argparser_new(argc, argv);
    if (cmdline == NULL) {
        loge("Fail to new 'argparser_t'\n");
        return -1;
    }
    argparser_add(cmdline, "-h", 'h', 0);
    argparser_add(cmdline, "-v", 'v', 0);
    argparser_add(cmdline, "-d", 'd', 1);
    argparser_add(cmdline, "--baud", 1001, 1);
    if (argparser_parse(cmdline, cmdline_proc) < 0) {
        loge("Fail to process command line\n");
        return -1;
    }

    dbg_run();

    logd("Debugger exit.\n");
    return 0;
}

