/**
 * @file    freertos_debug.c
 * @author  LightningSemi WLAN Team
 * Copyright (C) 2018 LightningSemi Technology Co., Ltd. All rights reserved.
 */

#include "./FreeRTOS_Adapter/freertos_debug.h"
#include "utils/sys_utils.h"

#if OS_RESOURCE_TRACE

extern int g_xr_msgqueue_cnt;
extern int g_xr_mutex_cnt;
extern int g_xr_semaphore_cnt;
extern int g_xr_thread_cnt;
extern int g_xr_timer_cnt;

void xr_os_resource_info(void)
{
	OS_LOG(1, "<<< xr os resource info >>>\r\n");
	OS_LOG(1, "g_xr_msgqueue_cnt  %d\r\n", g_xr_msgqueue_cnt);
	OS_LOG(1, "g_xr_mutex_cnt     %d\r\n", g_xr_mutex_cnt);
	OS_LOG(1, "g_xr_semaphore_cnt %d\r\n", g_xr_semaphore_cnt);
	OS_LOG(1, "g_xr_thread_cnt    %d\r\n", g_xr_thread_cnt);
	OS_LOG(1, "g_xr_timer_cnt     %d\r\n", g_xr_timer_cnt);
}

#endif /* OS_RESOURCE_TRACE */

void print_hex_dump_words(const void *addr, unsigned int len)
{
	unsigned int i;
	const unsigned int *p = addr;

	if ((unsigned int)p & 0x03) 
	{
		OS_SYSLOG("addr should be align 4B!\r\n");
		p =  (const void *)(((unsigned int)p) & ~0x03);
		return ;
	}
	len = DIV_ROUND_UP(len, 4);

	for (i = 0; i < len; i++) 
	{
		if ((i & 0x03) == 0x0){
			OS_SYSLOG("\n[%p]: ", p);
		}
		OS_SYSLOG("0x%08x ", *p++);
	}
	OS_SYSLOG("\r\n");
}

void print_hex_dump_bytes(const void *addr, unsigned int len)
{
	unsigned int i;
	const unsigned char *p = addr;
	len++;

	for (i = 1; i < len; ++i) 
	{
		OS_SYSLOG("%02x ", *p++);
		if (i % 16 == 0) {
			OS_SYSLOG("\r\n");
		}
	}
	OS_SYSLOG("\r\n");
}
