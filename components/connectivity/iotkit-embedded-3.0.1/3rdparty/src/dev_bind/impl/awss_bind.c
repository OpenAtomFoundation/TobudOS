/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "dev_bind_internal.h"
#ifdef WIFI_PROVISION_ENABLED
#ifndef AWSS_DISABLE_REGISTRAR
#include "awss_enrollee.h"
#endif
#endif

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
int awss_start_bind();

static void *awss_bind_mutex = NULL;
int awss_report_cloud()
{
    if (awss_bind_mutex == NULL) {
        awss_bind_mutex = HAL_MutexCreate();
        if (awss_bind_mutex == NULL) {
            return -1;
        }
    }

    HAL_MutexLock(awss_bind_mutex);
    awss_cmp_online_init();
    HAL_MutexUnlock(awss_bind_mutex);
    
#ifdef DEVICE_MODEL_ENABLED
    if(awss_check_reset()) {
        return awss_report_reset_to_cloud();
    }
#endif
    awss_start_bind();
    return 0;
}

int awss_start_bind()
{
    static int awss_bind_inited = 0;

    if (awss_bind_mutex == NULL) {
        awss_bind_mutex = HAL_MutexCreate();
        if (awss_bind_mutex == NULL)
            return -1;
    }

    HAL_MutexLock(awss_bind_mutex);
    if(awss_bind_inited == 1) {
        HAL_MutexUnlock(awss_bind_mutex);
        return 0;
    }
    awss_bind_inited = 1;

    awss_report_token();
    awss_cmp_local_init(AWSS_LC_INIT_BIND);
#ifndef DEV_BIND_DISABLE_NOTIFY
    awss_dev_bind_notify_stop();
    awss_dev_bind_notify();
#endif
#ifdef WIFI_PROVISION_ENABLED
#ifndef AWSS_DISABLE_REGISTRAR
    awss_registrar_init();
#endif
    AWSS_DISP_STATIS();
    AWSS_REPORT_STATIS("dev_bind");
#endif
    AWSS_DB_DISP_STATIS();
    AWSS_DB_REPORT_STATIS("dev_bind");
    HAL_MutexUnlock(awss_bind_mutex);
    return 0;
}

int awss_bind_deinit()
{
    if (awss_bind_mutex) {
        HAL_MutexLock(awss_bind_mutex);
    }

#ifdef DEVICE_MODEL_ENABLED
    awss_stop_report_reset();
#endif
    awss_stop_report_token();
    awss_cmp_online_deinit();

    awss_dev_bind_notify_stop();

    awss_cmp_local_deinit(1);
#ifdef WIFI_PROVISION_ENABLED
#ifndef AWSS_DISABLE_REGISTRAR
    awss_registrar_deinit();
#endif
    AWSS_CLEAR_STATIS();
#endif
    AWSS_DB_CLEAR_STATIS();

    if (awss_bind_mutex) {
        HAL_MutexUnlock(awss_bind_mutex);
        HAL_MutexDestroy(awss_bind_mutex);
    }

    awss_bind_mutex = NULL;

    return 0;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
