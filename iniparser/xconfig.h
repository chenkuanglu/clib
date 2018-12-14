/**
 * @file    xconfig.h
 * @author  ln
 * @brief   config template
 **/

#ifndef __C_XCONFIG_H__
#define __C_XCONFIG_H__

#include "cstr.h"
#include <stdbool.h>

#define DICT_ENTRY                  dict_entry

#define COLON                       :
#define MAKE_DICT_KEY(sec, key)     ( MAKE_STRING(sec) MAKE_STRING(COLON) MAKE_STRING(key) )


/* [serial] */
#define INI_SECTION_SERIAL          serial

#define INI_KEY_BAUDRATE         	baudrate
#define INI_KEY_DATA_BITS           data_bits
#define INI_KEY_CHECK_FLAG          check_flag
#define INI_KEY_STOP_BITS           stop_bits

/* [benchmark] */
#define INI_SECTION_BENCHMARK       benchmark

#define INI_KEY_BENCHMARK_EN      	benchmark_en

/* make/declare xconfig key */
#define CFG_BAUDRATE                MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_BAUDRATE)
#define CFG_DATA_BITS               MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_DATA_BITS)
#define CFG_CHECK_FLAG              MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_CHECK_FLAG)
#define CFG_STOP_BITS               MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_STOP_BITS)

#define CFG_BENCHMARK_EN            MAKE_DICT_KEY(INI_SECTION_BENCHMARK, INI_KEY_BENCHMARK_EN)

/* check flag */
enum { None, Odd, Even };

typedef struct {
    /* internal dictionary */
    dictionary *DICT_ENTRY;

    /* section [serial] */
	long        baudrate;
	long        data_bits;
	long        check_flag;
	long        stop_bits;

    /* section [benchmark] */
	bool        benchmark_en;
} xconfig_t;

extern xconfig_t*   xconfig_init    (xconfig_t* xconfig);
extern xconfig_t*   xconfig_create  (void);
extern int          xconfig_load    (xconfig_t *xconfig, const char *filename);
extern void         xconfig_delete  (xconfig_t *xconfig);

#endif

