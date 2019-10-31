/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_MAIN_H__
#define __AWSS_MAIN_H__

#include "awss_log.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

#define DEFAULT_SSID                   zc_default_ssid
#define DEFAULT_PASSWD                 zc_default_passwd
#define ADHA_SSID                      zc_adha_ssid
#define ADHA_PASSWD                    zc_adha_passwd

extern const char *zc_default_ssid;
extern const char *zc_default_passwd;
extern const char *zc_adha_ssid;
extern const char *zc_adha_passwd;

int __awss_start(void);
int __awss_stop(void);

int awss_cancel_aha_monitor(void);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
