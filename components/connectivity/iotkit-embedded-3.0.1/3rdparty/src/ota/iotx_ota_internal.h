/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _IOTX_OTA_INTERNAL_H_
#define _IOTX_OTA_INTERNAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "infra_httpc.h"
#include "infra_string.h"
#include "infra_md5.h"
#include "infra_sha256.h"
#include "infra_json_parser.h"
#include "ota_api.h"
#include "iotx_ota_config.h"
#include "ota_wrapper.h"

#ifdef INFRA_MEM_STATS
#include "infra_mem_stats.h"
#define OTA_MALLOC(size)            LITE_malloc(size, MEM_MAGIC, "ota")
#define OTA_FREE(ptr)               LITE_free(ptr)
#define OTA_API_MALLOC(size)        LITE_malloc(size, MEM_MAGIC, "ota.api")
#define OTA_API_FREE(ptr)           LITE_free(ptr)
#else
#define OTA_MALLOC(size)            HAL_Malloc(size)
#define OTA_FREE(ptr)               {HAL_Free((void *)ptr);ptr = NULL;}
#define OTA_API_MALLOC(size)        HAL_Malloc(size)
#define OTA_API_FREE(ptr)           {HAL_Free((void *)ptr);ptr = NULL;}
#endif

#define OTA_SNPRINTF                HAL_Snprintf

#ifdef INFRA_LOG
#include "infra_log.h"
#define OTA_LOG_CRIT(...)       log_crit("ota", __VA_ARGS__)
#define OTA_LOG_ERROR(...)      log_err("ota", __VA_ARGS__)
#define OTA_LOG_WRN(...)        log_warning("ota", __VA_ARGS__)
#define OTA_LOG_INFO(...)       log_info("ota", __VA_ARGS__)
#define OTA_LOG_DEBUG(...)      log_debug("ota", __VA_ARGS__)
#else
#define OTA_LOG_CRIT(...)       do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define OTA_LOG_ERROR(...)      do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define OTA_LOG_WRN(...)        do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define OTA_LOG_INFO(...)       do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#define OTA_LOG_DEBUG(...)      do{HAL_Printf(__VA_ARGS__);HAL_Printf("\r\n");}while(0)
#endif

typedef enum {
    IOTX_OTA_TOPIC_TYPE_DEVICE_REQUEST  = 1,
    IOTX_OTA_TOPIC_TYPE_DEVICE_UPGRATE = 2,
    IOTX_OTA_TOPIC_TYPE_CONFIG_GET     = 3,
    IOTX_OTA_TOPIC_TYPE_CONFIG_PUSH  = 4,
    IOTX_OTA_TOPIC_TYPE_MAX
} iotx_ota_topic_types_t;

typedef int (*ota_cb_fpt)(void *pcontext, const char *msg, uint32_t msg_len, iotx_ota_topic_types_t type);
/* is_fetch = 0; start fetch */
/* is_fetch = 1; stop fetch */
typedef void(*ota_fetch_cb_fpt)(void *user_data, int is_fetch, uint32_t size_file, char *purl, char *version);
/* is_fetch = 0; start fetch */
/* is_fetch = 1; stop fetch */
typedef void(*cota_fetch_cb_fpt)(void *user_data, int is_fetch, char *configId, uint32_t configSize, char *sign, \
                                 char *signMethod, char *url, char *getType);

int iotx_ota_set_fetch_callback(void *pt, ota_fetch_cb_fpt fetch_cb, void *user_data);
int iotx_ota_set_cota_fetch_callback(void *pt, cota_fetch_cb_fpt fetch_cb, void *user_data);

const char *otalib_JsonValueOf(const char *json, uint32_t json_len, const char *key, uint32_t *val_len);
void *otalib_MD5Init(void);
void otalib_MD5Update(void *md5, const char *buf, size_t buf_len);
void otalib_MD5Finalize(void *md5, char *output_str);
void otalib_MD5Deinit(void *md5);
void *otalib_Sha256Init(void);
void otalib_Sha256Update(void *sha256, const char *buf, size_t buf_len);
void otalib_Sha256Finalize(void *sha256, char *output_str);
void otalib_Sha256Deinit(void *sha256);
int otalib_GetFirmwareFixlenPara(const char *json_doc,
                                 size_t json_doc_len,
                                 const char *key,
                                 char *dest,
                                 size_t dest_len);
int otalib_GetFirmwareVarlenPara(const char *json_doc,
                                 size_t json_doc_len,
                                 const char *key,
                                 char **dest);
int otalib_GetParams(const char *json_doc, uint32_t json_len, char **url, char **version, char *md5,
                     uint32_t *file_size);
int otalib_GetConfigParams(const char *json_doc, uint32_t json_len, char **configId, uint32_t *configSize, char **sign,
                           char **signMethod, char **url, char **getType);
int otalib_GenInfoMsg(char *buf, size_t buf_len, uint32_t id, const char *version);
int otalib_GenReportMsg(char *buf, size_t buf_len, uint32_t id, int progress, const char *msg_detail);

void *ofc_Init(char *url);
int32_t ofc_Fetch(void *handle, char *buf, uint32_t buf_len, uint32_t timeout_s);
int ofc_Deinit(void *handle);

#endif /* _IOTX_OTA_INTERNAL_H_ */


