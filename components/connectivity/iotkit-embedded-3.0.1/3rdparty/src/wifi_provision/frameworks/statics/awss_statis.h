/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __AWSS_STATIS_H__
#define __AWSS_STATIS_H__

#include <stdint.h>
#ifdef AWSS_SUPPORT_DEV_AP
#include "awss_dev_ap.h"
#endif

enum {
    AWSS_STATIS_CONN_ROUTER_IDX = 0,  /* Statistic for connection with router */
    AWSS_STATIS_SM_IDX,               /* Statistic for smartconfig with bcast */
    AWSS_STATIS_WPS_IDX,              /* Statistic for smartconfig with wps */
    AWSS_STATIS_PAP_IDX,              /* Statistic for phone as AP */
    AWSS_STATIS_DAP_IDX,              /* Statistic for device AP */
    AWSS_STATIS_ROUTE_IDX,            /* Statistic for route solution */
    AWSS_STATIS_ZCONFIG_IDX,          /* Statistic for zero config */
};

enum {
    AWSS_STATIS_TYPE_TIME_START = 0,  /* Begining of operation */
    AWSS_STATIS_TYPE_TIME_SUC,        /* Success of operation */
    AWSS_STATIS_TYPE_PASSWD_ERR,      /* Failure of parsing PASSWD of router*/
    AWSS_STATIS_TYPE_CRC_ERR,         /* Failure of CRC check */
    AWSS_STATIS_TYPE_SCAN_START,      /* Start of scan operation */
    AWSS_STATIS_TYPE_SCAN_STOP,       /* Stop of scan operation */
    AWSS_STATIS_TYPE_SWITCHAP         /* Increase count of switch ap */
};

#ifndef AWSS_SUPPORT_STATIS
#define AWSS_SUPPORT_STATIS
#endif

#ifdef AWSS_SUPPORT_STATIS

struct awss_statis_conn_router_t {
    uint32_t conn_router_cnt;        /* the count of connect router */
    uint32_t conn_router_suc;        /* the success count of connect router */
    uint32_t conn_router_time_mean;  /* the mean time of success connection with router */
    uint32_t conn_router_time_max;   /* the max time of success connection with router */
    uint32_t conn_router_time_min;   /* the min time of success connection with router */
    uint32_t conn_router_start;      /* the start time to connect router */
    uint32_t conn_router_end;        /* the end time of connect router */
};  /* statistics for connection with router */

struct awss_statis_sm_t {
    uint32_t sm_parse_cnt;        /* the count of smartconfig */
    uint32_t sm_parse_crc_err;    /* the count of crc error */
    uint32_t sm_parse_passwd_err; /* the count of passwd error */
    uint32_t sm_parse_suc;        /* the count of smartconfig success */
    uint32_t sm_parse_start;      /* the start time to smartconfig */
    uint32_t sm_parse_end;        /* the ene time of smartconfig */
    uint32_t sm_time_mean;        /* the mean time of smartconfig */
    uint32_t sm_time_max;         /* the max time of smartconfig */
    uint32_t sm_time_min;         /* the min time of smartconfig */
};  /* smartconfig-bcast statistic  */

struct awss_statis_wps_t {
    uint32_t wps_parse_cnt;        /* the count of smartconfig-wps */
    uint32_t wps_parse_crc_err;    /* the count of crc error */
    uint32_t wps_parse_passwd_err; /* the count of passwd error */
    uint32_t wps_parse_suc;        /* the count of smartconfig-wps success */
};  /* smartconfig-wps statistic  */

struct awss_statis_phone_ap_t {
    uint32_t aha_cnt;
    uint32_t aha_suc;
    uint32_t aha_time_mean;        /* mean time of phone as AP solution */
    uint32_t aha_time_max;         /* max time of phone as AP solution */
    uint32_t aha_time_min;         /* min time of phone as AP solution */
    uint32_t aha_start;
    uint32_t aha_end;
    uint32_t aha_scan_start;
    uint32_t aha_scan_end;
    uint32_t aha_switch_ap;
    uint32_t aha_passwd_err;
};  /* Phone as AP */

struct awss_statis_dev_ap_t {
    uint32_t dev_ap_cnt;
    uint32_t dev_ap_suc;
    uint32_t dev_ap_time_mean;     /* mean time of device AP solution */
    uint32_t dev_ap_time_max;      /* max time of device AP solution */
    uint32_t dev_ap_time_min;      /* min time of device AP solution */
    uint32_t dev_ap_start;
    uint32_t dev_ap_end;
    uint32_t dev_ap_passwd_err;
};  /* device work as AP */

struct awss_statis_route_t {
    uint32_t adha_cnt;
    uint32_t adha_suc;
    uint32_t adha_time_mean;       /* mean time of discovery of route solution */
    uint32_t adha_time_max;        /* min time of discovery of route solution */
    uint32_t adha_time_min;        /* max time of discovery of route solution */
    uint32_t adha_start;
    uint32_t adha_end;
};  /* discovery of route solution */

struct awss_statis_zconfig_t {
    uint32_t zc_cnt;
    uint32_t zc_suc;
    uint32_t zc_passwd_err;
};  /* zero configure solution */

struct awss_statis_t {
    struct awss_statis_conn_router_t droute;
#ifdef AWSS_SUPPORT_SMARTCONFIG
    struct awss_statis_sm_t sm;
#ifdef AWSS_SUPPORT_SMARTCONFIG_WPS
    struct awss_statis_wps_t wps;
#endif
#endif
#ifdef AWSS_SUPPORT_AHA
    struct awss_statis_phone_ap_t pap;
#endif
#ifdef AWSS_SUPPORT_DEV_AP
    struct awss_statis_dev_ap_t dap;
#endif
#ifdef AWSS_SUPPORT_ADHA
    struct awss_statis_route_t route;
#endif
#ifndef AWSS_DISABLE_ENROLLEE
    struct awss_statis_zconfig_t zconfig;
#endif
};

int awss_report_statis(const char *module);
void awss_update_statis(int awss_statis_idx, int type);
void awss_clear_statis();
void awss_disp_statis();

#define AWSS_UPDATE_STATIS(idx, type) awss_update_statis(idx, type)
#define AWSS_CLEAR_STATIS()           awss_clear_statis()
#define AWSS_DISP_STATIS()            awss_disp_statis()
#define AWSS_REPORT_STATIS(m)         awss_disp_statis(m)
#else
#define AWSS_UPDATE_STATIS(idx, type)
#define AWSS_CLEAR_STATIS()
#define AWSS_DISP_STATIS()
#define AWSS_REPORT_STATIS(m)
#endif

#endif
