/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-27 03:25:58
 * @LastEditTime : 2020-01-15 22:11:21
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _LOG_H_
#define _LOG_H_

#define LOG_IS_SALOF    1

#if LOG_IS_SALOF
    #include "salof.h"

    #define LOG_D(fmt, ...)   LOG_DEBUG(fmt, ##__VA_ARGS__)
    #define LOG_I(fmt, ...)   LOG_INFO(fmt, ##__VA_ARGS__)
    #define LOG_W(fmt, ...)   LOG_WARN(fmt, ##__VA_ARGS__)
    #define LOG_E(fmt, ...)   LOG_ERR(fmt, ##__VA_ARGS__)
    #define log_init   salof_init
#else
    #include <stdio.h>
    #define LOG_D(fmt, ...)   { printf(fmt, ##__VA_ARGS__); printf("\n");}
    #define LOG_I(fmt, ...)   { printf(fmt, ##__VA_ARGS__); printf("\n");}
    #define LOG_W(fmt, ...)   { printf(fmt, ##__VA_ARGS__); printf("\n");}
    #define LOG_E(fmt, ...)   { printf(fmt, ##__VA_ARGS__); printf("\n");} 
    #define log_init()
#endif


#endif /* _LOG_H_ */
