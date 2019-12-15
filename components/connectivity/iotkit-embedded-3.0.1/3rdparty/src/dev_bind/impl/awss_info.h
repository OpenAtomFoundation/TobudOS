/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_INFO_H__
#define __AWSS_INFO_H__

#include <stdio.h>

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

int process_get_device_info(void *ctx, void *resource, void *remote, void *request, char is_mcast, int type);
int awss_process_mcast_get_connectap_info(void *ctx, void *resource, void *remote, void *request);
int awss_process_ucast_get_connectap_info(void *ctx, void *resource, void *remote, void *request);

int awss_stop_connectap_monitor();
int awss_start_connectap_monitor();

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
