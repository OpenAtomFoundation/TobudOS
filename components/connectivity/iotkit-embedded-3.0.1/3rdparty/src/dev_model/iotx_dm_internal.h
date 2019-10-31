/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOTX_DM_INTERNAL_H_
#define _IOTX_DM_INTERNAL_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "infra_config.h"
#include "infra_types.h"
#include "infra_defs.h"
#include "infra_list.h"
#include "infra_cjson.h"
#include "infra_report.h"
#include "infra_string.h"
#if defined(DEVICE_MODEL_GATEWAY)
    #include "infra_sha1.h"
#endif


#ifndef _IN_
    #define _IN_
#endif

#ifndef _OU_
    #define _OU_
#endif

#ifndef DM_READ_ONLY
    #define DM_READ_ONLY
#endif

#include <stdlib.h>

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)
    #include "iotx_ota.h"
    #include "ota_api.h"
#endif

/* CM Header File */
#include "iotx_cm.h"

/* ALCS Header File */
#ifdef ALCS_ENABLED
    #include "CoAPExport.h"
    #include "iotx_alcs.h"
    #include "dm_server_adapter.h"
    #include "dm_server.h"
#endif

/* DM Header File */
#include "dm_wrapper.h"
#include "iotx_dm_config.h"
#include "iotx_dm.h"
#include "dm_utils.h"
#include "dm_shadow.h"
#include "dm_tsl_alink.h"
#include "dm_message_cache.h"
#include "dm_opt.h"
#include "dm_ota.h"
#include "dm_cota.h"
#include "dm_fota.h"
#include "dm_ipc.h"
#include "dm_message.h"
#include "dm_msg_process.h"
#include "dm_manager.h"
#include "dm_client_adapter.h"
#include "dm_client.h"
#include "dm_intf.h"

#ifdef INFRA_MEM_STATS
    #include "infra_mem_stats.h"
    #define DM_malloc(size)            LITE_malloc(size, MEM_MAGIC, "dm")
    #define DM_free(ptr)               LITE_free(ptr)
#else
    #define DM_malloc(size)            HAL_Malloc(size)
    #define DM_free(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#if defined(COAP_COMM_ENABLED) && !defined(MQTT_COMM_ENABLED)
    #define DM_URI_OFFSET 1
#else
    #define DM_URI_OFFSET 0
#endif

#ifdef INFRA_LOG
    #include "infra_log.h"
    #define dm_log_emerg(...)     log_emerg("DM", __VA_ARGS__)
    #define dm_log_crit(...)      log_crit("DM", __VA_ARGS__)
    #define dm_log_err(...)       log_err("DM", __VA_ARGS__)
    #define dm_log_warning(...)   log_warning("DM", __VA_ARGS__)
    #define dm_log_info(...)      log_info("DM", __VA_ARGS__)
    #define dm_log_debug(...)     log_debug("DM", __VA_ARGS__)
#else
    #define dm_log_emerg(...)
    #define dm_log_crit(...)
    #define dm_log_err(...)
    #define dm_log_warning(...)
    #define dm_log_info(...)
    #define dm_log_debug(...)
    #define HEXDUMP_INFO(...)
    #define HEXDUMP_DEBUG(...)
#endif

#ifdef LOG_REPORT_TO_CLOUD
#define LOG_POLL_SIZE (CONFIG_MQTT_TX_MAXLEN - 174)
#define REPORT_LEN (LOG_POLL_SIZE - 110)
#define OVERFLOW_LEN (LOG_POLL_SIZE - 10)

typedef enum {
    READY,
    RUNNING,
    DONE
} REPORT_STATE;
unsigned int add_tail();
int reset_log_poll();
int remove_log_poll();
unsigned int push_log(const char *perform_data, int perform_data_size);
#endif

#endif
