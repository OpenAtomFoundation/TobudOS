#ifndef _BSP_RTC_H
#define _BSP_RTC_H
/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_rtc.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : RTC驱动头文件。
其他	   : 无
论坛 	   : www.openedv.com
日志	   : 初版V1.0 2019/1/3 左忠凯创建
***************************************************************/
#include "bsp.h"

/* 相关宏定义 */
#define SECONDS_IN_A_DAY 		(86400) /* 一天86400秒	 		*/
#define SECONDS_IN_A_HOUR 		(3600)	/* 一个小时3600秒 		*/
#define SECONDS_IN_A_MINUTE 	(60)	/* 一分钟60秒  		 	*/
#define DAYS_IN_A_YEAR 			(365)	/* 一年365天 			*/
#define YEAR_RANGE_START 		(1970)	/* 开始年份1970年 		*/
#define YEAR_RANGE_END 			(2099)	/* 结束年份2099年 		*/


/* 时间日期结构体 */
struct rtc_datetime
{
    unsigned short year;  /* 范围为:1970 ~ 2099 		*/
    unsigned char month;  /* 范围为:1 ~ 12				*/
    unsigned char day;    /* 范围为:1 ~ 31 (不同的月，天数不同).*/
    unsigned char hour;   /* 范围为:0 ~ 23 			*/
    unsigned char minute; /* 范围为:0 ~ 59				*/
    unsigned char second; /* 范围为:0 ~ 59				*/
};


/* 函数声明 */
void rtc_init(void);
void rtc_enable(void);
void rtc_disable(void);
unsigned int rtc_coverdate_to_seconds(struct rtc_datetime *datetime);
unsigned int rtc_getseconds(void);
void rtc_setdatetime(struct rtc_datetime *datetime);
void rtc_getdatetime(struct rtc_datetime *datetime)
;

#endif
