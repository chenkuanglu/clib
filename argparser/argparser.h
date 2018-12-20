/**
 * @file    argparser.h
 * @author  ln
 * @brief   program arguments parser
 **/

#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include "thr_queue.h"

typedef struct {
    long    name;
    long    id;
    long    num;
} argparser_args_t;

typedef struct {
    thrq_cb_t name_tbl;
} argparser_t;

typedef int (*parse_func_t)(long id, char **param);

extern argparser_t* argparser_new       (void);
extern void         argparser_delete    (argparser_t*);

extern void         argparser_add       (argparser_t *parser, const char* arg_name, long arg_id, int param_num);
extern void         argparser_parse     (argparser_t *parser, parse_func_t parse);

#endif

