/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-27 03:25:58
 * @LastEditTime : 2020-02-20 01:16:25
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _LOG_H_
#define _LOG_H_

#define LOG_IS_SALOF    1

#define LOG_LEVEL       DEBUG_LEVEL         //WARN_LEVEL DEBUG_LEVEL INFO_LEVEL

#if LOG_IS_SALOF
    #include "salof.h"

    #define LOG_D(fmt, ...)   LOG_DEBUG(fmt, ##__VA_ARGS__)
    #define LOG_I(fmt, ...)   LOG_INFO(fmt, ##__VA_ARGS__)
    #define LOG_W(fmt, ...)   LOG_WARN(fmt, ##__VA_ARGS__)
    #define LOG_E(fmt, ...)   LOG_ERR(fmt, ##__VA_ARGS__)
    #define log_init   salof_init
#else
    #include <stdio.h>
    
    #define BASE_LEVEL      (0)
    #define ASSERT_LEVEL    (BASE_LEVEL + 1)
    #define ERR_LEVEL       (ASSERT_LEVEL + 1)
    #define WARN_LEVEL      (ERR_LEVEL + 1)
    #define INFO_LEVEL      (WARN_LEVEL + 1)
    #define DEBUG_LEVEL     (INFO_LEVEL + 1)

#if LOG_LEVEL < DEBUG_LEVEL
    #define LOG_D(fmt, ...)
#else
    #define LOG_D(fmt, ...)     { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if LOG_LEVEL < INFO_LEVEL
    #define LOG_I(fmt, ...)
#else
    #define LOG_I(fmt, ...)      { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if LOG_LEVEL < WARN_LEVEL
    #define LOG_W(fmt, ...)
#else
    #define LOG_W(fmt, ...)      { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if LOG_LEVEL < ERR_LEVEL
    #define LOG_E(fmt, ...)
#else
    #define LOG_E(fmt, ...)       { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

#if LOG_LEVEL < ASSERT_LEVEL
    #define LOG_ASSERT(fmt, ...)
    #define ASSERT(x)
#else
    #define LOG_ASSERT(fmt, ...)    { printf(fmt, ##__VA_ARGS__); printf("\n");}
    #define ASSERT(x)     if((x)==0) LOG_ASSERT("%s, %d\n",__FILE__,__LINE__)
#endif

#if LOG_LEVEL < BASE_LEVEL
    #define LOG(fmt, ...)
#else
    #define LOG(fmt, ...)           { printf(fmt, ##__VA_ARGS__); printf("\n");}
#endif

    #define log_init()
#endif

#endif /* _LOG_H_ */
