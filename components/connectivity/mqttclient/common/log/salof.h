#ifndef _SALOF_H_
#define _SALOF_H_

#include "salof_defconfig.h"
#include "format.h"
#include "fifo.h"
#include <stdio.h>

int salof_init(void);
void salof(const char *fmt, ...);

/** font color */
#define     SALOF_FC_BLACK    30       
#define     SALOF_FC_RED      31
#define     SALOF_FC_GREEN    32
#define     SALOF_FC_YELLOW   33
#define     SALOF_FC_BLUE     34
#define     SALOF_FC_PURPLE   35
#define     SALOF_FC_DARK     36
#define     SALOF_FC_WHITE    37

#ifdef SALOF_USING_LOG

#if SALOF_USING_SALOF
    #define     SALOF_PRINT_LOG           salof
#else

#if ((!SALOF_USING_SALOF)&&(!SALOF_PRINT_LOG))
    #define         SALOF_PRINT_LOG       printf
#endif

#ifndef SALOF_PRINT_LOG
    #error "If the SALOF_USING_LOG macro definition is turned on, you must define SALOF_PRINT_LOG as the LOG output, such as #definePRINT_LOG printf"
#endif

#endif

#if SALOF_LOG_COLOR
    #define SALOF_LOG_START(l, c)     SALOF_PRINT_LOG("\033\n["#c"m["#l"] >> ")
    #define SALOF_LOG_END             SALOF_PRINT_LOG("\033[0m")  
#else
    #define SALOF_LOG_START(l, c)     SALOF_PRINT_LOG("\n["#l"] >> ")
    #define SALOF_LOG_END       
#endif

#if SALOF_LOG_TS && SALOF_LOG_TAR
    #define SALOF_LOG_T           SALOF_PRINT_LOG("[TS: %d] [TAR: %s] ",salof_get_tick(), salof_get_task_name())
#elif SALOF_LOG_TS
    #define SALOF_LOG_T           SALOF_PRINT_LOG("[TS: %d] ", salof_get_tick())
#elif SALOF_LOG_TAR
    #define SALOF_LOG_T           SALOF_PRINT_LOG("[TAR: %s] ", salof_get_task_name())
#else
    #define SALOF_LOG_T
#endif


#define SALOF_LOG_LINE(l, c, fmt, ...)        \
    do {                                \
        SALOF_LOG_START(l, c);                \
        SALOF_LOG_T;                          \
        SALOF_PRINT_LOG(fmt, ##__VA_ARGS__);  \
        SALOF_LOG_END;                        \
    } while (0)

#define SALOF_BASE_LEVEL      (0)
#define SALOF_ERR_LEVEL       (SALOF_BASE_LEVEL + 1)
#define SALOF_WARN_LEVEL      (SALOF_ERR_LEVEL + 1)
#define SALOF_INFO_LEVEL      (SALOF_WARN_LEVEL + 1)
#define SALOF_DEBUG_LEVEL     (SALOF_INFO_LEVEL + 1)

#ifndef SALOF_LOG_LEVEL
    #define SALOF_LOG_LEVEL   SALOF_DEBUG_LEVEL
#endif 

#if SALOF_LOG_LEVEL < SALOF_DEBUG_LEVEL
    #define SALOF_LOG_DEBUG(fmt, ...)
#else
    #define SALOF_LOG_DEBUG(fmt, ...)     SALOF_LOG_LINE(D, 0, fmt, ##__VA_ARGS__)
#endif

#if SALOF_LOG_LEVEL < SALOF_INFO_LEVEL
    #define SALOF_LOG_INFO(fmt, ...)
#else
    #define SALOF_LOG_INFO(fmt, ...)      SALOF_LOG_LINE(I, SALOF_FC_GREEN, fmt, ##__VA_ARGS__)
#endif

#if SALOF_LOG_LEVEL < SALOF_WARN_LEVEL
    #define SALOF_LOG_WARN(fmt, ...)
#else
    #define SALOF_LOG_WARN(fmt, ...)      SALOF_LOG_LINE(W, SALOF_FC_YELLOW, fmt, ##__VA_ARGS__)
#endif

#if SALOF_LOG_LEVEL < SALOF_ERR_LEVEL
    #define SALOF_LOG_ERR(fmt, ...)
#else
    #define SALOF_LOG_ERR(fmt, ...)       SALOF_LOG_LINE(E, SALOF_FC_RED, fmt, ##__VA_ARGS__)
#endif

#if SALOF_LOG_LEVEL < SALOF_BASE_LEVEL
    #define SALOF_LOG(fmt, ...)
#else
    #define SALOF_LOG(fmt, ...)           SALOF_PRINT_LOG(fmt, ##__VA_ARGS__)
#endif
#else
#define SALOF_LOG_DEBUG(fmt, ...)
#define SALOF_LOG_INFO(fmt, ...)
#define SALOF_LOG_WARN(fmt, ...)
#define SALOF_LOG_ERR(fmt, ...)
#define SALOF_LOG_LOG(fmt, ...)
#endif

#endif // !_SALOF_H_

