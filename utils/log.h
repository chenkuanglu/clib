/**
 * @file    log.h
 * @author  ln
 * @brief   print & save log info
 **/

#ifndef __LOG_INFO_H__
#define __LOG_INFO_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/* make string */
#define _MAKE_CSTR(s)                   #s
#define MAKE_CSTR(s)                    _MAKE_CSTR(s)

/* concat string */
#define _CONCAT_STRING(l, r)            l##r
#define CONCAT_STRING(l, r)             _CONCAT_STRING(l, r)

/* Control Sequence Introducer */
#define CONSOLE_CSI_BEGIN               \x1b[
#define CONSOLE_CSI_END                 m

#define SGR_FOREGROUND                  3
#define SGR_BACKGROUND                  4
#define SGR_NOT                         2
#define SGR_GRAY_DARK                   90

#define SGR_BLACK                       0
#define SGR_RED                         1
#define SGR_GREEN                       2
#define SGR_YELLOW                      3
#define SGR_BLUE                        4
#define SGR_PURPLE                      5
#define SGR_CYAN                        6
#define SGR_WHITE                       7

#define SGR_INIT                        0
#define SGR_HIGHLIGHT                   1
#define SGR_VAGUE                       2
#define SGR_ITALIC                      3
#define SGR_UNDERLINE                   4
#define SGR_BLINK_QUIK                  5
#define SGR_BLINK_LOW                   6
#define SGR_REVERSE                     7
#define SGR_HIDE                        8
#define SGR_DELETE                      9

/* color in console: \x1b[n(;...)m */
#define CSIB                            MAKE_CSTR(CONSOLE_CSI_BEGIN)
#define CSIE                            MAKE_CSTR(CONSOLE_CSI_END)

#define CCL_BLACK                       CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_BLACK)) CSIE
#define CCL_RED                         CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_RED)) CSIE
#define CCL_GREEN                       CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_GREEN)) CSIE
#define CCL_YELLOW                      CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_YELLOW)) CSIE
#define CCL_BLUE                        CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_BLUE)) CSIE
#define CCL_PURPLE                      CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_PURPLE)) CSIE
#define CCL_CYAN                        CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_CYAN)) CSIE
#define CCL_WHITE                       CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_WHITE)) CSIE
#define CCL_WHITE_HL                    CSIB MAKE_CSTR(CONCAT_STRING(SGR_FOREGROUND,SGR_WHITE)) ";" \
                                        MAKE_CSTR(SGR_HIGHLIGHT) CSIE
#define CCL_GRAY_DARK                   CSIB MAKE_CSTR(SGR_GRAY_DARK) CSIE
#define CCL_END                         CSIB MAKE_CSTR(SGR_INIT) CSIE

extern pthread_mutex_t mutex_log; 

extern int          vfprintfd   (FILE *stream, const char *format, va_list param);
extern int          fprintfd    (FILE *stream, const char *format, ...);

extern int          vprintfd    (const char *format, va_list param);
extern int          printfd     (const char *format, ...);

extern int logd(const char *format, ...);
extern int logi(const char *format, ...);
extern int logw(const char *format, ...);
extern int loge(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif  /* __LOG_INFO_H__ */

