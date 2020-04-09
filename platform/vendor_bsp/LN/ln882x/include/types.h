#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#ifdef _BIT
#undef _BIT
#endif
/* Set bit macro */
#define _BIT(n) (1 << (n))
#define SET_BIT(x,y)                x|=(1<<y)
#define CLR_BIT(x,y)                x&=~(1<<y)
#define REVERSE_BIT(x,y)            x^=(1<<y)
#define GET_BIT(x,y)                (((x) >> (y))&1)

/* NULL pointer */
#ifndef NULL
#define NULL ((void *) 0)
#endif

/* Number of elements in an array */
#define NELEMENTS(array)  (sizeof(array) / sizeof(array[0]))


#ifndef MAC2STR
#define MAC2STR(a)          (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR            "%02X:%02X:%02X:%02X:%02X:%02X"
#endif

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif

#endif /* __TYPES_H__ */
