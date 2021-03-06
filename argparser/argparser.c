/**
 * @file    argparser.c
 * @author  ln
 * @brief   program arguments parser
 **/

#include "argparser.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_ARG_NAME    64

typedef struct {
    char    name[MAX_ARG_NAME];
    long    id;
    long    n;
} argparser_args_t;

static int argcmp(const void* left, const void* right, size_t len)
{
    if (left != NULL && right != NULL && len > 0) {
        const argparser_args_t *l = (const argparser_args_t *)left;
        const argparser_args_t *r = (const argparser_args_t *)right;
        if (strcmp(l->name, r->name) == 0)  {
            loge("argparser: multiple arg name '%s'\n", l->name);
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
        p->arg_names = que_new(NULL);
        if (p->arg_names != NULL) {
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
            que_destroy(parser->arg_names);
        }
        free(parser);
    }
}

void argparser_add(argparser_t *parser, const char* arg_name, long arg_id, int param_num)
{
    if (parser == NULL || arg_name == NULL || arg_id <= 0 || param_num < 0) {
        loge("argparser: Invalid parameter\n");
        return;
    }
    if (strlen(arg_name) > MAX_ARG_NAME - 1) {
        loge("argparser: Arg name too long\n");
        return;
    }

    argparser_args_t arg;
    strcpy(arg.name, arg_name);
    arg.id = arg_id;
    arg.n = param_num;
    que_lock(parser->arg_names);
    if (QUE_FIND(parser->arg_names, &arg, sizeof(argparser_args_t),argcmp ) == NULL) {
        if (que_insert_tail(parser->arg_names, &arg, sizeof(argparser_args_t)) < 0)
            loge("argparser: Fail to add, cannot insert queue\n");
    }
    que_unlock(parser->arg_names);
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
        que_elm_t *var;
        argparser_args_t *arg;
        QUE_FOREACH(var, parser->arg_names) {
            arg = (argparser_args_t *)var->data;
            if ((found = strcmp(*v, arg->name)) == 0) { 
                if (c - 1 < arg->n) {
                    /* process the previous arg */
                    if (prev_v != NULL) {
                        if (parse_proc(prev_i, prev_v+1, prev_c) < 0) {     /* number of arg may be more than 'arg->n' */
                            prev_v = NULL;
                        }
                    }
                    loge("argparser: Fail to parse '%s', Short of parameter\n", *v);
                    return -1;
                }

                /* process the previous arg */
                if (prev_v != NULL) {
                    if (parse_proc(prev_i, prev_v+1, prev_c) < 0) {         /* number of arg may be more than 'arg->n' */
                        prev_v = NULL;
                        return 0;                                           /* user stop parse */
                    }
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
        if (prev_c == 0)
            parse_proc(prev_i, NULL, prev_c);
        else
            parse_proc(prev_i, prev_v+1, prev_c);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif

