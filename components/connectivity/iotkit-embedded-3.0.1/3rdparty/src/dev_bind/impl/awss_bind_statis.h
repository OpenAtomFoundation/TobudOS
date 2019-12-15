/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_BIND_STATIS_H__
#define __AWSS_BIND_STATIS_H__

#ifndef AWSS_SUPPORT_DEV_BIND_STATIS
    #define AWSS_SUPPORT_DEV_BIND_STATIS
#endif

enum {
    AWSS_DB_STATIS_START,
    AWSS_DB_STATIS_SUC,
};

#ifdef AWSS_SUPPORT_DEV_BIND_STATIS
struct awss_statis_dev_bind_t {
    uint32_t dev_bind_cnt;        /* the count of token sync */
    uint32_t dev_bind_suc;        /* the successful count of token sync */
    uint32_t dev_bind_time_mean;  /* the mean time of token sync */
    uint32_t dev_bind_time_max;   /* the max time of token sync */
    uint32_t dev_bind_time_min;   /* the min time of token sync */
    uint32_t dev_bind_start;      /* the start time to sync token */
    uint32_t dev_bind_end;        /* the end time of token sync */
};  /* statistics for token sync */


int awss_bind_report_statis(const char *module);
void awss_bind_update_statis(int type);
void awss_bind_clear_statis();
void awss_bind_disp_statis();


#define AWSS_DB_UPDATE_STATIS(type) awss_bind_update_statis(type)
#define AWSS_DB_DISP_STATIS()       awss_bind_disp_statis()
#define AWSS_DB_CLEAR_STATIS()      awss_bind_clear_statis()
#define AWSS_DB_REPORT_STATIS(m)    awss_bind_report_statis(m)
#else
#define AWSS_DB_UPDATE_STATIS(type)
#define AWSS_DB_DISP_STATIS()
#define AWSS_DB_CLEAR_STATIS()
#define AWSS_DB_REPORT_STATIS(m)
#endif
#endif
