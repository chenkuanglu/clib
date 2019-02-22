/**
 * @file    popen_p.h
 * @author  ln
 * @brief   popen likely, and return the pid of sub-process
 **/

#ifndef __POPEN_P_H__
#define __POPEN_P_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FILE *fp;
    pid_t pid;
} fpid_t;

extern int popen_p(const char *cmd, const char *type, fpid_t *fpid);
extern int pclose_p(fpid_t *fpid);

#ifdef __cplusplus
}
#endif

#endif /* __THR_MUX__ */

