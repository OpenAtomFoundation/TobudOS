/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_DEV_RESET_H__
#define __AWSS_DEV_RESET_H__

#include <stdio.h>

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

int awss_report_reset_to_cloud();
int awss_report_reset();
int awss_check_reset();
int awss_stop_report_reset();
#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif
