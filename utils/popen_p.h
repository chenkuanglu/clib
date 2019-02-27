/**
 * @file    popen_p.h
 * @author  ln
 * @brief   popen likely, return the pid of child process & pipe stream
 **/

#ifndef __POPEN_P_H__
#define __POPEN_P_H__

#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FILE *fp;
    pid_t pid;
} file_t;

extern int popen_p  (const char *cmd, const char *type, file_t *fpid);
extern int pclose_p (file_t *fpid);

#ifdef __cplusplus
}
#endif

#endif /* __THR_MUX__ */

