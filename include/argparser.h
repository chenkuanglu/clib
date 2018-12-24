/**
 * @file    argparser.h
 * @author  ln
 * @brief   program arguments parser
 **/

#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include "thr_queue.h"

typedef struct {
    int         argc;
    char**      argv;
    thrq_cb_t*  arg_names;
} argparser_t;

typedef int (*parse_callback_t)(long id, char **param);

extern argparser_t* argparser_new       (int argc, char **argv);
extern void         argparser_delete    (argparser_t* parser);

extern void         argparser_add       (argparser_t *parser, const char* arg_name, long arg_id, int param_num);
extern int          argparser_parse     (argparser_t *parser, parse_callback_t parse_proc);

#endif

