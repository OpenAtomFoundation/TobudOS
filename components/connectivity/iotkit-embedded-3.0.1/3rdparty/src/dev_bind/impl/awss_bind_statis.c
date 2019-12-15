/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "dev_bind_internal.h"

#ifdef AWSS_SUPPORT_DEV_BIND_STATIS
static struct awss_statis_dev_bind_t g_db_statis = {0};
static uint32_t awss_statis_db_report_id = 0;
static uint32_t awss_statis_db_trace_id = 0;
static void *awss_statis_db_mutex = NULL;

#define DB_CNT    g_db_statis.dev_bind_cnt
#define DB_SUC    g_db_statis.dev_bind_suc
#define DB_TMEAN  g_db_statis.dev_bind_time_mean
#define DB_TMIN   g_db_statis.dev_bind_time_min
#define DB_TMAX   g_db_statis.dev_bind_time_max
#define DB_START  g_db_statis.dev_bind_start
#define DB_END    g_db_statis.dev_bind_end

#define AWSS_STATIS_DB_BUF_LEN (512)

int awss_bind_report_statis(const char *module)
{
    const char *elem_fmt = "[%s max:%u min:%u mean:%u cnt:%u suc:%u]";
    int log_buf_len = AWSS_STATIS_DB_BUF_LEN + strlen(AWSS_STATIS_FMT) + 21;
    char statis_topic[TOPIC_LEN_MAX] = {0};
    char *log_content = NULL;
    char id_str[21] = {0};
    char *log_buf = NULL;
    int len = 0;
    int ret;

    log_content = os_zalloc(AWSS_STATIS_DB_BUF_LEN + 1);
    if (log_content == NULL) {
        goto BIND_STATIS_ERR;
    }
    log_buf = os_zalloc(log_buf_len + 1);
    if (log_buf == NULL) {
        goto BIND_STATIS_ERR;
    }

    if (awss_build_topic(TOPIC_POST_STATIS, statis_topic, TOPIC_LEN_MAX) == NULL) {
        awss_err("awss build statis topic fail\n");
        goto BIND_STATIS_ERR;
    }

    if (awss_statis_db_mutex) {
        HAL_MutexLock(awss_statis_db_mutex);
    }
    do {
        if (DB_CNT == 0) {
            break;
        }

        len += HAL_Snprintf(log_buf + len, log_buf_len - len, elem_fmt, "SyncToken",
                            DB_TMAX, DB_TMIN, DB_TMEAN, DB_CNT, DB_SUC);

        HAL_Snprintf(log_content, AWSS_STATIS_DB_BUF_LEN, AWSS_STATIS_FMT, (uint32_t)HAL_UptimeMs(), "BIND_TRACE",
                     module == NULL ? "default" : module, awss_statis_db_trace_id, log_buf);

        HAL_Snprintf(id_str, sizeof(id_str), "%u", ++ awss_statis_db_report_id);

        awss_build_packet(AWSS_CMP_PKT_TYPE_REQ, id_str, ILOP_VER, METHOD_LOG_POST, log_content, 0,
                          log_buf, &log_buf_len);

        awss_debug("%s\n", log_buf);

        ret = awss_cmp_mqtt_send(statis_topic, log_buf, strlen(log_buf), 0);

        awss_info("bind report statis %s\n", ret == 0 ? "success" : "fail");
    } while (0);

    if (awss_statis_db_mutex) {
        HAL_MutexUnlock(awss_statis_db_mutex);
    }

    HAL_Free(log_buf);
    HAL_Free(log_content);

    return 0;

BIND_STATIS_ERR:
    if (log_content) {
        HAL_Free(log_content);
    }
    if (log_buf) {
        HAL_Free(log_buf);
    }
    return -1;
}

void awss_bind_clear_statis()
{
    if (awss_statis_db_mutex) {
        HAL_MutexLock(awss_statis_db_mutex);
    }

    memset(&g_db_statis, 0, sizeof(g_db_statis));

    awss_statis_db_trace_id = 0;
    awss_statis_db_report_id = 0;

    if (awss_statis_db_mutex) {
        HAL_MutexUnlock(awss_statis_db_mutex);
        HAL_MutexDestroy(awss_statis_db_mutex);
    }
    awss_statis_db_mutex = NULL;
}

void awss_bind_update_statis(int type)
{
    uint32_t time = HAL_UptimeMs();

    if (awss_statis_db_mutex == NULL) {
        awss_statis_db_mutex = HAL_MutexCreate();
        if (awss_statis_db_mutex == NULL) {
            awss_debug("a-statis am fail\n");
            return;
        }
    }

    HAL_MutexLock(awss_statis_db_mutex);

    if (type == AWSS_DB_STATIS_START) {
        awss_statis_db_trace_id ++;
    }

    switch (type) {
        case AWSS_DB_STATIS_START:
            DB_CNT ++;
            DB_START = time;
            break;
        case AWSS_DB_STATIS_SUC:
            DB_END = time;
            DB_SUC ++;
            time = (uint32_t)(DB_END - DB_START);
            if (DB_SUC > 0) {
                DB_TMEAN = (DB_TMEAN + time) / DB_SUC;
            } else {
                DB_SUC = 1;
                DB_TMEAN = time;
            }
            if (DB_TMIN == 0 || DB_TMIN > time) {
                DB_TMIN = time;
            }
            if (DB_TMAX == 0 || DB_TMAX < time) {
                DB_TMAX = time;
            }
            break;
        default:
            break;
    }
    HAL_MutexUnlock(awss_statis_db_mutex);
}

void awss_bind_disp_statis()
{
    if (awss_statis_db_mutex) {
        HAL_MutexLock(awss_statis_db_mutex);
    }

    awss_debug("--------------------------DEV BIND STATIS-----------------------------");
    awss_debug("name\t\tmax\tmin\tmean\tcnt\tsuc");
    awss_debug("SyncToken      \t%u\t%u\t%u\t%u\t%u\t",
               DB_TMAX, DB_TMIN, DB_TMEAN, DB_CNT, DB_SUC);
    awss_debug("----------------------------------------------------------------------");

    if (awss_statis_db_mutex) {
        HAL_MutexUnlock(awss_statis_db_mutex);
    }
}
#endif
