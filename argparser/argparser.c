/**
 * @file    argparser.c
 * @author  ln
 * @brief   program arguments parser
 **/

#include "argparser.h"
#include "cstr.h"

#define MAX_ARG_NAME    64

typedef struct {
    char    name[MAX_ARG_NAME];
    long    id;
    long    n;
} argparser_args_t;

static int argcmp(const void* left, const void* right, size_t len)
{
    if (left != NULL && right != NULL && len > 0) {
        const argparser_args_t *l = left;
        const argparser_args_t *r = right;
        if (strcmp(l->name, r->name) == 0)  {
            printfd(CCL_RED "argparser: Cannot add arg '%s', multiple arg name\n" CCL_END);
            return 0;
        }
    }

    return -1;
}

argparser_t* argparser_new(int argc, char **argv)
{
    if (argc <= 0 || argv == NULL)
        return NULL;

    argparser_t* p = (argparser_t *)malloc(sizeof(argparser_t));
    if (p != NULL) {
        p->arg_names = thrq_create(NULL);
        if (p->arg_names != NULL) {
            thrq_set_compare(p->arg_names, argcmp);
            p->argc = argc;
            p->argv = argv;
            return p;
        }
        else 
            free(p);
    }
    return NULL;
}

void argparser_delete(argparser_t* parser)
{
    if (parser != NULL) {
        if (parser->arg_names != NULL) {
            thrq_destroy(parser->arg_names);
        }
        free(parser);
    }
}

void argparser_add(argparser_t *parser, const char* arg_name, long arg_id, int param_num)
{
    if (parser == NULL || arg_name == NULL || arg_id <= 0 || param_num < 0) {
        printfd(CCL_RED "argparser: Invalid parameter\n" CCL_END);
        return;
    }
    if (strlen(arg_name) > MAX_ARG_NAME - 1) {
        printfd(CCL_RED "argparser: Arg name too long\n" CCL_END);
        return;
    }

    argparser_args_t arg;
    strcpy(arg.name, arg_name);
    arg.id = arg_id;
    arg.n = param_num;
    if (thrq_find(parser->arg_names, &arg, sizeof(argparser_args_t)) == NULL) {
        if (thrq_append(parser->arg_names, &arg, sizeof(argparser_args_t)) < 0)
            printfd(CCL_RED "argparser: Fail to add, cannot insert queue\n" CCL_END);
    }
}

int argparser_parse(argparser_t *parser, parse_callback_t parse_proc)
{
    if (parser == NULL || parse_proc == NULL)
        return -1;

    char **prev_v = NULL;
    int prev_c;
    int prev_i;

    int found;
    int c = parser->argc - 1;
    char **v = parser->argv + 1;
    while (c > 0) {
        thrq_elm_t *var;
        argparser_args_t *arg;
        THRQ_FOREACH(var, parser->arg_names) {
            arg = (argparser_args_t *)var->data;
            if ((found = strcmp(*v, arg->name)) == 0) { 
                if (c - 1 < arg->n) {
                    printfd(CCL_RED "argparser: Fail to parse '%s', Short of parameter\n" CCL_END, *v);
                    return -1;
                }

                /* process the previous arg */
                if (prev_v != NULL) {
                    parse_proc(prev_i, prev_v+1, prev_c);   /* number of arg may be more than 'arg->n' */
                }

                prev_v = v;
                prev_c = arg->n;
                prev_i = arg->id;

                c -= arg->n + 1;
                v += arg->n + 1;
                break;
            }
        }
        if (found != 0) {
            c -= 1;
            v += 1;
            prev_c++;
        }
    }

    if (prev_v != NULL) {
        parse_proc(prev_i, prev_v+1, prev_c);
    }

    return 0;
}

