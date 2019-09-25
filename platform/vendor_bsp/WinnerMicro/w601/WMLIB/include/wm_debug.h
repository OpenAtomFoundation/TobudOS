/**
 * @file    wm_debug.h
 *
 * @brief   debug Module APIs
 *
 * @author  dave
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef WM_DEBUG_H
#define WM_DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include "wm_config.h"

/* 0x00000000 - 0x80000000 */
/** Define the debugging switch: on */
#define TLS_DBG_ON                  1
/** Define the debugging switch: off */
#define TLS_DBG_OFF                 0


/* 0x0000000F - 0x00000001 */
/** Define the debugging level: info */
#define TLS_DBG_LEVEL_INFO          TLS_DBG_OFF
/** Define the debugging level: warning */
#define TLS_DBG_LEVEL_WARNING       TLS_DBG_OFF
/** Define the debugging level: error */
#define TLS_DBG_LEVEL_ERR           TLS_DBG_OFF
/** Define the debugging level: dump */
#define TLS_DBG_LEVEL_DUMP          TLS_DBG_OFF

/** general debug info switch, default: off */
#define TLS_GENERAL_DBG             TLS_DBG_OFF


#define __TLS_DBGPRT_INFO(fmt, ...)				     \
do {									                     \
       u32 time = tls_os_get_time();	                 \
       printf("[WM_I] <%d.%02d> %s : "fmt, (time/100), (time%100), __func__ , ##__VA_ARGS__); \
} while (0)

#define __TLS_DBGPRT_WARNING(fmt, ...)				     \
do {									                     \
       u32 time = tls_os_get_time();	                 \
       printf("[WM_W] <%d.%02d> %s : "fmt, (time/100), (time%100), __func__ , ##__VA_ARGS__); \
} while (0)

#define __TLS_DBGPRT_ERR(fmt, ...)				     \
do {									                     \
       u32 time = tls_os_get_time();	                 \
       printf("[WM_E] <%d.%02d> %s : "fmt, (time/100), (time%100), __func__ , ##__VA_ARGS__); \
} while (0)


/**
 * @defgroup System_APIs System APIs
 * @brief System APIs
 */

/**
 * @addtogroup System_APIs
 * @{
 */

/**
 * @defgroup DEBUG_APIs DEBUG APIs
 * @brief DEBUG APIs
 */

/**
 * @addtogroup DEBUG_APIs
 * @{
 */

#if (TLS_GENERAL_DBG && TLS_DBG_LEVEL_INFO)
/** Print information of the info level */
#define TLS_DBGPRT_INFO(f, a...)     __TLS_DBGPRT_INFO(f, ##a)
#else
/** Print information of the info level */
#define TLS_DBGPRT_INFO(f, a...)
#endif

#if (TLS_GENERAL_DBG && TLS_DBG_LEVEL_WARNING)
/** Print information of the warning level */
#define TLS_DBGPRT_WARNING(f, a...)  __TLS_DBGPRT_WARNING(f, ##a)
#else
/** Print information of the warning level */
#define TLS_DBGPRT_WARNING(f, a...)
#endif

#if (TLS_GENERAL_DBG && TLS_DBG_LEVEL_ERR)
/** Print information of the error level */
#define TLS_DBGPRT_ERR(f, a...)      __TLS_DBGPRT_ERR(f, ##a)
#else
/** Print information of the error level */
#define TLS_DBGPRT_ERR(f, a...)
#endif

#if (TLS_GENERAL_DBG && TLS_DBG_LEVEL_DUMP)
/**
 * @brief          dump memory
 *
 * @param[in]      *p     pointer the memory
 * @param[in]      len    length of memory
 *
 * @return         None
 *
 * @note           None
 */
void    TLS_DBGPRT_DUMP(char *p, u32 len);
#else
/** Print information of the dump level */
#define TLS_DBGPRT_DUMP(p, len)
#endif

/**
 * @}
 */

/**
 * @}
 */

#endif /* end of WM_DEBUG_H */

