/**
 * @file    cstr.h
 * @author  ln
 * @brief   concat string with '##' 
 **/

#ifndef __C_STRING_H__
#define __C_STRING_H__

/* make string */
#define _MAKE_STRING(s)                 #s
#define MAKE_STRING(s)                  _MAKE_STRING(s)

/* concat string */
#define _CONCAT_STRING(l, r)            l##r
#define CONCAT_STRING(l, r)             _CONCAT_STRING(l, r)

#endif

