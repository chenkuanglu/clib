/**
 * @file    cstr.h
 * @author  ln
 * @brief   the c string process
 **/

#ifndef __C_STRING_H__
#define __C_STRING_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char*        strlwr      (char *out, const char *in, unsigned size);
extern char*        strupr      (char *out, const char *in, unsigned size);

extern int          bin2hex     (char *hex, const void *bin, unsigned len);
extern char*        abin2hex    (const void *bin, unsigned len);

extern void         hex2bin     (void *bin, const char *hex, unsigned len);
extern void*        ahex2bin    (const char *hex);

extern void         memswap     (void *out, const void *in, unsigned len, unsigned section_size);

//extern dictionary*  strsplit    (char *str);
//extern dictionary*  strunsplit  (char *buf, unsigned size, dictionary* dict, char sep);

#ifdef __cplusplus
}
#endif

#endif  /* __C_STRING_H__ */

