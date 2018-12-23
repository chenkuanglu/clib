#include "argparser.h"
#include "cstr.h"

static int cmdline_proc(long id, char **param)
{
    switch (id) {
    case 'o':
        printfd(CCL_YELLOW "arg = -%c, param = %s\n" CCL_END, (char)id, param[0]);
        break;
    case 'C':
        printfd(CCL_YELLOW "arg = -%c, param = %s\n" CCL_END, (char)id, param[0]);
        break;
    case 'A':
        printfd(CCL_YELLOW "arg = -%c\n" CCL_END, (char)id);
        break;
    case 1001:
        printfd(CCL_YELLOW "arg = %d, param = %s\n" CCL_END, id, param[0]);
        break;
    case 1002:
        printfd(CCL_YELLOW "arg = %d: \n" CCL_END, id);
        printfd(CCL_YELLOW "\tparam[0] = %s\n" CCL_END, param[0]);
        printfd(CCL_YELLOW "\tparam[1] = %s\n" CCL_END, param[1]);
        printfd(CCL_YELLOW "\tparam[2] = %s\n" CCL_END, param[2]);
        break;
    default:
        printfd(CCL_YELLOW "unknown arg id %d\n" CCL_END, id);
        break;
    }

    return 0;
}

int main(int argc, char **argv)
{
    argparser_t *argparser = argparser_new(argc, argv);
    if (argparser == NULL) {
        printfd(CCL_RED "Fail to new argparser\n" CCL_END);
    }

    argparser_add(argparser, "-o", 'o', 1);
    argparser_add(argparser, "-C", 'C', 1);
    argparser_add(argparser, "--chip-num", 'C', 1);
    argparser_add(argparser, "-A", 'A', 0);

    argparser_add(argparser, "--max-temp", 1001, 1);
    argparser_add(argparser, "--write-reg", 1002, 3);

    if (argparser_parse(argparser, cmdline_proc)) {
        printfd(CCL_RED "Fail to process command line\n" CCL_END);
    }

    argparser_delete(argparser);
    return 0;
}

