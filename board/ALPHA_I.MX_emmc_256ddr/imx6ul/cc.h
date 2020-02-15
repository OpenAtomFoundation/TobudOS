#ifndef __CC_H
#define __CC_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 cc.h
作者	   : 左忠凯
版本	   : V1.0
描述	   : 有关变量类型的定义，NXP官方SDK的一些移植文件会用到。
其他	   : 无
日志	   : 初版V1.0 2019/1/3 左忠凯创建
***************************************************************/

/*
 * 自定义一些数据类型供库文件使用
 */
#define     __I     volatile 
#define     __O     volatile 
#define     __IO    volatile

#define ON 		1
#define OFF		0

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned long     long uint64_t;
typedef	  signed char  	 	   s8;		
typedef	  signed short 	  int  s16;
typedef	  signed int 		   s32;
typedef	  signed long long int s64;
typedef	unsigned char 		   u8;
typedef	unsigned short int     u16;
typedef	unsigned int 		   u32;
typedef	unsigned long long int u64;


#endif
