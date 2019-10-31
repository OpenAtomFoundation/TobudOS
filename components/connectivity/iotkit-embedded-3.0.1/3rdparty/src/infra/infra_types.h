#ifndef _INFRA_TYPES_H_
#define _INFRA_TYPES_H_

#include <stddef.h>
#include "infra_config.h"

#define IOT_TRUE            (1)     /* indicate boolean value true */
#define IOT_FALSE           (0)     /* indicate boolean value false */

#if !defined(PLATFORM_HAS_STDINT)

typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned int        uint32_t;
typedef signed int          int32_t;
typedef unsigned long int   uint64_t;
typedef signed long int     int64_t;
typedef unsigned int        uintptr_t;

#else

#include <stdint.h>

#endif  /* #if !defined(PLATFORM_HAS_STDINT) */

#endif

