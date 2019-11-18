/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "wifi_provision_internal.h"

#ifdef AWSS_SUPPORT_STATIS

#define DROUTE_START    g_awss_statis.droute.conn_router_start
#define DROUTE_END      g_awss_statis.droute.conn_router_end
#define DROUTE_CNT      g_awss_statis.droute.conn_router_cnt
#define DROUTE_SUC      g_awss_statis.droute.conn_router_suc
#define DROUTE_TMIN     g_awss_statis.droute.conn_router_time_min
#define DROUTE_TMAX     g_awss_statis.droute.conn_router_time_max
#define DROUTE_TMEAN    g_awss_statis.droute.conn_router_time_mean

#ifdef  AWSS_SUPPORT_SMARTCONFIG
#ifdef  AWSS_SUPPORT_SMARTCONFIG_WPS
#define WPS_CNT         g_awss_statis.wps.wps_parse_cnt
#define WPS_CRC_ERR     g_awss_statis.wps.wps_parse_crc_err
#define WPS_PW_ERR      g_awss_statis.wps.wps_parse_passwd_err
#define WPS_SUC         g_awss_statis.wps.wps_parse_suc
#endif
#define SM_CNT          g_awss_statis.sm.sm_parse_cnt
#define SM_CRC_ERR      g_awss_statis.sm.sm_parse_crc_err
#define SM_PW_ERR       g_awss_statis.sm.sm_parse_passwd_err
#define SM_SUC          g_awss_statis.sm.sm_parse_suc
#define SM_START        g_awss_statis.sm.sm_parse_start
#define SM_END          g_awss_statis.sm.sm_parse_end
#define SM_TMIN         g_awss_statis.sm.sm_time_min
#define SM_TMAX         g_awss_statis.sm.sm_time_max
#define SM_TMEAN        g_awss_statis.sm.sm_time_mean
#endif

#ifdef  AWSS_SUPPORT_AHA
#define PAP_CNT         g_awss_statis.pap.aha_cnt
#define PAP_SUC         g_awss_statis.pap.aha_suc
#define PAP_TMIN        g_awss_statis.pap.aha_time_min
#define PAP_TMAX        g_awss_statis.pap.aha_time_max
#define PAP_TMEAN       g_awss_statis.pap.aha_time_mean
#define PAP_START       g_awss_statis.pap.aha_start
#define PAP_END         g_awss_statis.pap.aha_end
#define PAP_SSTART      g_awss_statis.pap.aha_scan_start
#define PAP_SSTOP       g_awss_statis.pap.aha_scan_end
#define PAP_SAP         g_awss_statis.pap.aha_switch_ap
#define PAP_PW_ERR      g_awss_statis.pap.aha_passwd_err
#endif

#ifdef  AWSS_SUPPORT_DEV_AP
#define DAP_CNT         g_awss_statis.dap.dev_ap_cnt
#define DAP_SUC         g_awss_statis.dap.dev_ap_suc
#define DAP_TMIN        g_awss_statis.dap.dev_ap_time_min
#define DAP_TMAX        g_awss_statis.dap.dev_ap_time_max
#define DAP_TMEAN       g_awss_statis.dap.dev_ap_time_mean
#define DAP_START       g_awss_statis.dap.dev_ap_start
#define DAP_END         g_awss_statis.dap.dev_ap_end
#define DAP_PW_ERR      g_awss_statis.dap.dev_ap_passwd_err
#endif

#ifdef  AWSS_SUPPORT_ADHA
#define ROUTE_CNT       g_awss_statis.route.adha_cnt
#define ROUTE_SUC       g_awss_statis.route.adha_suc
#define ROUTE_TMIN      g_awss_statis.route.adha_time_min
#define ROUTE_TMAX      g_awss_statis.route.adha_time_max
#define ROUTE_TMEAN     g_awss_statis.route.adha_time_mean
#define ROUTE_START     g_awss_statis.route.adha_start
#define ROUTE_END       g_awss_statis.route.adha_end
#endif

#ifndef AWSS_DISABLE_ENROLLEE
#define ZC_CNT          g_awss_statis.zconfig.zc_cnt
#define ZC_SUC          g_awss_statis.zconfig.zc_suc
#define ZC_PW_ERR       g_awss_statis.zconfig.zc_passwd_err
#endif

#define AWSS_STATIS_BUF_LEN (768)

static void *awss_statis_mutex = NULL;
static uint32_t awss_statis_trace_id = 0;
static uint32_t awss_statis_report_id = 0;
static struct awss_statis_t g_awss_statis = {0};

int awss_report_statis(const char *module)
{
    const char *elem_fmt = "[%s max:%u min:%u mean:%u cnt:%u suc:%u crc-err:%u pw-err:%u],";
    int log_buf_len = AWSS_STATIS_BUF_LEN + strlen(AWSS_STATIS_FMT) + 21;
    char statis_topic[TOPIC_LEN_MAX] = {0};
    char *log_content = NULL;
    char id_str[21] = {0};
    char *log_buf = NULL;
    int len = 0;
    int ret;

    log_content = os_zalloc(AWSS_STATIS_BUF_LEN + 1);
    if (log_content == NULL)
        goto STATIS_ERR;
    log_buf = os_zalloc(log_buf_len + 1);
    if (log_buf == NULL)
        goto STATIS_ERR;

    if (awss_build_topic(TOPIC_POST_STATIS, statis_topic, TOPIC_LEN_MAX) == NULL) {
        awss_err("awss build statis topic fail\n");
        goto STATIS_ERR;
    }

    if (awss_statis_mutex)
        HAL_MutexLock(awss_statis_mutex);

    if (DROUTE_CNT > 0) {
        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "ConnRouter",
                DROUTE_TMAX, DROUTE_TMIN, DROUTE_TMEAN, DROUTE_CNT, DROUTE_SUC, 0, 0);
    }

#ifdef AWSS_SUPPORT_SMARTCONFIG
    if (SM_CNT > 0) {
        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "Smartconfig",
                SM_TMAX, SM_TMIN, SM_TMEAN, SM_CNT, SM_SUC, SM_CRC_ERR, SM_PW_ERR);
    }
#ifdef  AWSS_SUPPORT_SMARTCONFIG_WPS
    if (WPS_CNT > 0) {
        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "Smartconfig-wps",
                0, 0, 0, WPS_CNT, WPS_SUC, WPS_CRC_ERR, WPS_PW_ERR);
    }
#endif
#endif
#ifdef AWSS_SUPPORT_AHA
    if (PAP_CNT > 0) {
        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "Aha",
                PAP_TMAX, PAP_TMIN, PAP_TMEAN, PAP_CNT, PAP_SUC, 0, PAP_PW_ERR);
    }
#endif
#ifdef AWSS_SUPPORT_ADHA
    if (ROUTE_CNT > 0) {
        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "Adha",
                ROUTE_TMAX, ROUTE_TMIN, ROUTE_TMEAN, ROUTE_CNT, ROUTE_SUC, 0, 0);
    }
#endif
#ifndef AWSS_DISABLE_ENROLLEE
    if (ZC_CNT > 0) {
        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "Zconfig",
                0, 0, 0, ZC_CNT, ZC_SUC, 0, ZC_PW_ERR);
    }
#endif
#ifdef AWSS_SUPPORT_DEV_AP
    if (DAP_CNT > 0) {
        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "Dev-ap",
                DAP_TMAX, DAP_TMIN, DAP_TMEAN, DAP_CNT, DAP_SUC, 0, DAP_PW_ERR);
    }
#endif

    do {
        if (len == 0)  /* no need to report log */
            break;
        log_buf[len - 1] = '\0';  /* remove end of ',' */

        HAL_Snprintf(log_content, AWSS_STATIS_BUF_LEN, AWSS_STATIS_FMT, (uint32_t)HAL_UptimeMs(), "AWSS_TRACE",
                module == NULL ? "default" : module, awss_statis_trace_id, log_buf);

        HAL_Snprintf(id_str, sizeof(id_str), "%u", ++ awss_statis_report_id);

        awss_build_packet(AWSS_CMP_PKT_TYPE_REQ, id_str, ILOP_VER, METHOD_LOG_POST, log_content, 0,
                log_buf, &log_buf_len);

        awss_debug("%s\n", log_buf);

        ret = awss_cmp_mqtt_send(statis_topic, log_buf, strlen(log_buf), 0);

        awss_info("awss report statis %s\n", ret == 0 ? "success" : "fail");
    } while (0);

    if (awss_statis_mutex)
        HAL_MutexUnlock(awss_statis_mutex);

    HAL_Free(log_buf);
    HAL_Free(log_content);

    return 0;

STATIS_ERR:
    if (log_content) HAL_Free(log_content);
    if (log_buf) HAL_Free(log_buf);
    return -1;
}

void awss_disp_statis()
{
    if (awss_statis_mutex)
        HAL_MutexLock(awss_statis_mutex);

    awss_debug("--------------------------------AWSS STATIS-----------------------------------");
    awss_debug("name\t\tmax\tmin\tmean\tcnt\tsuc\tcrc-err\tpasswd-err");
    awss_debug("ConnRouter      \t%u\t%u\t%u\t%u\t%u\t%u\t%u\t",
            DROUTE_TMAX, DROUTE_TMIN, DROUTE_TMEAN, DROUTE_CNT, DROUTE_SUC, 0, 0);
#ifdef AWSS_SUPPORT_SMARTCONFIG
    awss_debug("Smartconfig     \t%u\t%u\t%u\t%u\t%u\t%u\t%u\t",
            SM_TMAX, SM_TMIN, SM_TMEAN, SM_CNT, SM_SUC, SM_CRC_ERR, SM_PW_ERR);
#ifdef  AWSS_SUPPORT_SMARTCONFIG_WPS
    awss_debug("Smartconfig-wps \t%u\t%u\t%u\t%u\t%u\t%u\t%u\t",
            0, 0, 0, WPS_CNT, WPS_SUC, WPS_CRC_ERR, WPS_PW_ERR);
#endif
#endif
#ifdef AWSS_SUPPORT_AHA
    awss_debug("Aha             \t%u\t%u\t%u\t%u\t%u\t%u\t%u\t",
            PAP_TMAX, PAP_TMIN, PAP_TMEAN, PAP_CNT, PAP_SUC, 0, PAP_PW_ERR);
#endif
#ifdef AWSS_SUPPORT_ADHA
    awss_debug("Adha            \t%u\t%u\t%u\t%u\t%u\t%u\t%u\t",
            ROUTE_TMAX, ROUTE_TMIN, ROUTE_TMEAN, ROUTE_CNT, ROUTE_SUC, 0, 0);
#endif
#ifndef AWSS_DISABLE_ENROLLEE
    awss_debug("Zconfig         \t%u\t%u\t%u\t%u\t%u\t%u\t%u\t",
            0, 0, 0, ZC_CNT, ZC_SUC, 0, ZC_PW_ERR);
#endif
#ifdef AWSS_SUPPORT_DEV_AP
    awss_debug("Dev-ap          \t%u\t%u\t%u\t%u\t%u\t%u\t%u\t",
            DAP_TMAX, DAP_TMIN, DAP_TMEAN, DAP_CNT, DAP_SUC, 0, DAP_PW_ERR);
#endif
    awss_debug("------------------------------------------------------------------------------");

    if (awss_statis_mutex)
        HAL_MutexUnlock(awss_statis_mutex);
}

void awss_clear_statis()
{
    if (awss_statis_mutex)
        HAL_MutexLock(awss_statis_mutex);

    memset(&g_awss_statis, 0, sizeof(g_awss_statis));

    awss_statis_trace_id = 0;
    awss_statis_report_id = 0;

    if (awss_statis_mutex) {
        HAL_MutexUnlock(awss_statis_mutex);
        HAL_MutexDestroy(awss_statis_mutex);
    }
    awss_statis_mutex = NULL;
}

void awss_update_statis(int awss_statis_idx, int type)
{
    uint32_t time = HAL_UptimeMs();

    if (awss_statis_mutex == NULL) {
        awss_statis_mutex = HAL_MutexCreate();
        if (awss_statis_mutex == NULL) {
            awss_debug("a-statis am fail\n");
            return;
        }
    }

    HAL_MutexLock(awss_statis_mutex);

    if (type == AWSS_STATIS_TYPE_TIME_START)
        awss_statis_trace_id ++;

    switch (awss_statis_idx) {
        case AWSS_STATIS_CONN_ROUTER_IDX:
            switch (type) {
                case AWSS_STATIS_TYPE_TIME_START:
                    DROUTE_CNT ++;
                    DROUTE_START = time;
                    break;
                case AWSS_STATIS_TYPE_TIME_SUC:
                    DROUTE_SUC ++;
                    DROUTE_END = time;
                    time = (uint32_t)(DROUTE_END - DROUTE_START);
                    if (DROUTE_SUC > 0) {
                        DROUTE_TMEAN = (DROUTE_TMEAN + time) / (DROUTE_SUC);
                    } else {
                        DROUTE_TMEAN = time;
                        DROUTE_SUC = 1;
                    }
                    if (DROUTE_TMIN == 0 || DROUTE_TMIN > time)
                        DROUTE_TMIN = time;
                    if (DROUTE_TMAX == 0 || DROUTE_TMAX < time)
                        DROUTE_TMAX = time;
                    break;
                default:
                    break;
            }
            break;
#ifdef AWSS_SUPPORT_SMARTCONFIG
#ifdef AWSS_SUPPORT_SMARTCONFIG_WPS
        case AWSS_STATIS_WPS_IDX:
            switch (type) {
                case AWSS_STATIS_TYPE_TIME_START:
                    WPS_CNT ++;
                    break;
                case AWSS_STATIS_TYPE_TIME_SUC:
                    WPS_SUC ++;
                    break;
                case AWSS_STATIS_TYPE_PASSWD_ERR:
                    WPS_PW_ERR ++;
                    break;
                case AWSS_STATIS_TYPE_CRC_ERR:
                    WPS_CRC_ERR ++;
                    break;
                default:
                    break;
            }
            break;
#endif
        case AWSS_STATIS_SM_IDX:
            switch (type) {
                case AWSS_STATIS_TYPE_TIME_START:
                    SM_CNT ++;
                    SM_START = time;
                    break;
                case AWSS_STATIS_TYPE_TIME_SUC:
                    SM_SUC ++;
                    SM_END = time;
                    time = (uint32_t)(SM_END - SM_START);
                    if (SM_SUC > 0) {
                        SM_TMEAN = (SM_TMEAN + time) / (SM_SUC);
                    } else {
                        SM_TMEAN = time;
                        SM_SUC = 1;
                    }

                    if (SM_TMIN == 0 || SM_TMIN > time)
                        SM_TMIN = time;
                    if (SM_TMAX == 0 || SM_TMAX < time)
                        SM_TMAX = time;
                    break;
                case AWSS_STATIS_TYPE_PASSWD_ERR:
                    SM_PW_ERR ++;
                    break;
                case AWSS_STATIS_TYPE_CRC_ERR:
                    SM_CRC_ERR ++;
                    break;
                default:
                    break;
            }
            break;
#endif
#ifdef AWSS_SUPPORT_AHA
        case AWSS_STATIS_PAP_IDX:
            switch (type) {
                case AWSS_STATIS_TYPE_TIME_START:
                    PAP_CNT ++;
                    PAP_START = time;
                    break;
                case AWSS_STATIS_TYPE_TIME_SUC:
                    PAP_SUC ++;
                    PAP_END = time;
                    time = (uint32_t)(PAP_END - PAP_START);
                    if (PAP_SUC > 0) {
                        PAP_TMEAN = (PAP_TMEAN + time) / (PAP_SUC);
                    } else {
                        PAP_TMEAN = time;
                        PAP_SUC = 1;
                    }
                    if (PAP_TMIN == 0 || PAP_TMIN > time)
                        PAP_TMIN = time;
                    if (PAP_TMAX == 0 || PAP_TMAX < time)
                        PAP_TMAX = time;
                    break;
                case AWSS_STATIS_TYPE_PASSWD_ERR:
                    PAP_PW_ERR ++;
                    break;
                case AWSS_STATIS_TYPE_SCAN_START:
                    PAP_SSTART = time;
                    break;
                case AWSS_STATIS_TYPE_SCAN_STOP:
                    PAP_SSTOP = time;
                    break;
                case AWSS_STATIS_TYPE_SWITCHAP:
                    PAP_SAP ++;
                    break;
                default:
                    break;
            }
            break;
#endif
#ifdef AWSS_SUPPORT_DEV_AP
        case AWSS_STATIS_DAP_IDX:
            switch (type) {
                case AWSS_STATIS_TYPE_TIME_START:
                    DAP_CNT ++;
                    DAP_START = time;
                    break;
                case AWSS_STATIS_TYPE_TIME_SUC:
                    DAP_SUC ++;
                    DAP_END = time;
                    time = (uint32_t)(DAP_END - DAP_START);
                    if (DAP_SUC > 0) {
                        DAP_TMEAN = (DAP_TMEAN + time) / (DAP_SUC);
                    } else {
                        DAP_TMEAN = time;
                        DAP_SUC = 1;
                    }
                    if (DAP_TMIN == 0 || DAP_TMIN > time)
                        DAP_TMIN = time;
                    if (DAP_TMAX == 0 || DAP_TMAX < time)
                        DAP_TMAX = time;
                    break;
                case AWSS_STATIS_TYPE_PASSWD_ERR:
                    DAP_PW_ERR ++;
                    break;
                default:
                    break;
            }
            break;
#endif
#ifdef AWSS_SUPPORT_ADHA
        case AWSS_STATIS_ROUTE_IDX:
            switch (type) {
                case AWSS_STATIS_TYPE_TIME_START:
                    ROUTE_CNT ++;
                    ROUTE_START = time;
                    break;
                case AWSS_STATIS_TYPE_TIME_SUC:
                    ROUTE_SUC ++;
                    ROUTE_END = time;
                    time = (uint32_t)(ROUTE_END - ROUTE_START);
                    if (ROUTE_SUC > 0) {
                        ROUTE_TMEAN = (ROUTE_TMEAN + time) / (ROUTE_SUC);
                    } else {
                        ROUTE_TMEAN = time;
                        ROUTE_SUC = 1;
                    }

                    if (ROUTE_TMIN == 0 || ROUTE_TMIN > time)
                        ROUTE_TMIN = time;
                    if (ROUTE_TMAX == 0 || ROUTE_TMAX < time)
                        ROUTE_TMAX = time;
                    break;
                default:
                    break;
            }
            break;
#endif
#ifndef AWSS_DISABLE_ENROLLEE
        case AWSS_STATIS_ZCONFIG_IDX:
            switch (type) {
                case AWSS_STATIS_TYPE_TIME_START:
                    ZC_CNT ++;
                    break;
                case AWSS_STATIS_TYPE_TIME_SUC:
                    ZC_SUC ++;
                    break;
                case AWSS_STATIS_TYPE_PASSWD_ERR:
                    ZC_PW_ERR ++;
                    break;
                default:
                    break;
            }
            break;
#endif
        default:
            break;
    }
    HAL_MutexUnlock(awss_statis_mutex);
}

#endif
