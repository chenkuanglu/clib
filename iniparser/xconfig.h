/**
 * @file    xconfig.h
 * @author  ln
 * @brief   config template
 **/

#ifndef __XCONFIG_H__
#define __XCONFIG_H__

#include "cstr.h"
#include "log.h"
#include "dictionary.h"
#include "iniparser.h"
#include <stdbool.h>

#define DICT_ENTRY                  dict_entry

#define COLON                       :
#define MAKE_DICT_KEY(sec, key)     ( MAKE_CSTR(sec) MAKE_CSTR(COLON) MAKE_CSTR(key) )


/* [serial] */
#define INI_SECTION_SERIAL          serial

#define INI_KEY_DEV_NAME            dev_name
#define INI_KEY_BAUDRATE         	baudrate
#define INI_KEY_DATA_BITS           data_bits
#define INI_KEY_PARITY              parity
#define INI_KEY_STOP_BITS           stop_bits

/* [benchmark] */
#define INI_SECTION_BENCHMARK       benchmark

#define INI_KEY_BENCHMARK_EN      	benchmark_en

/* make/declare xconfig MACRO */
#define CFG_DEV_NAME                MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_DEV_NAME)
#define CFG_BAUDRATE                MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_BAUDRATE)
#define CFG_DATA_BITS               MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_DATA_BITS)
#define CFG_PARITY                  MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_PARITY)
#define CFG_STOP_BITS               MAKE_DICT_KEY(INI_SECTION_SERIAL, INI_KEY_STOP_BITS)

#define CFG_BENCHMARK_EN            MAKE_DICT_KEY(INI_SECTION_BENCHMARK, INI_KEY_BENCHMARK_EN)

/* default config value if ini not config */
#define DEV_NAME_DEFAULT            "/dev/ttyUSB0"
#define BAUDRATE_DEFAULT            115200
#define DATA_BITS_DEFAULT           8
#define PARITY_DEFAULT              None
#define STOP_BITS_DEFAULT           1

#define BENCHMARK_EN_DEFAULT        false


/* serial parity */
enum { None, Odd, Even };

typedef struct {
    /* internal dictionary */
    dictionary *DICT_ENTRY;

    /* section [serial] */
    char*   dev_name;
	long    baudrate;
	long    data_bits;
	long    parity;
	long    stop_bits;

    /* section [benchmark] */
	bool    benchmark_en;
} xconfig_t;

extern xconfig_t*   xconfig_new     (void);
extern int          xconfig_load    (xconfig_t *xconfig, const char *inifile);
extern void         xconfig_delete  (xconfig_t *xconfig);

#endif

