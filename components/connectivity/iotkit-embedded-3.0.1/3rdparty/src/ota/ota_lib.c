/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iotx_ota_internal.h"

const char *otalib_JsonValueOf(const char *json, uint32_t json_len, const char *key, uint32_t *val_len)
{
    int length;
    const char *val;
    val = json_get_value_by_name((char *)json, json_len, (char *)key, &length, NULL);
    if (NULL != val) {
        *val_len = (uint32_t) length;
    }
    return val;
}

void *otalib_MD5Init(void)
{
    iot_md5_context *ctx = OTA_MALLOC(sizeof(iot_md5_context));
    if (NULL == ctx) {
        return NULL;
    }

    utils_md5_init(ctx);
    utils_md5_starts(ctx);

    return ctx;
}

void otalib_MD5Update(void *md5, const char *buf, size_t buf_len)
{
    utils_md5_update(md5, (unsigned char *)buf, buf_len);
}

void otalib_MD5Finalize(void *md5, char *output_str)
{
    int i;
    unsigned char buf_out[16];
    utils_md5_finish(md5, buf_out);

    for (i = 0; i < 16; ++i) {
        output_str[i * 2] = infra_hex2char(buf_out[i] >> 4);
        output_str[i * 2 + 1] = infra_hex2char(buf_out[i]);
    }
    output_str[32] = '\0';
}

void otalib_MD5Deinit(void *md5)
{
    if (NULL != md5) {
        OTA_FREE(md5);
    }
}

void *otalib_Sha256Init(void)
{
    iot_sha256_context *ctx = OTA_MALLOC(sizeof(iot_sha256_context));
    if (NULL == ctx) {
        return NULL;
    }

    utils_sha256_init(ctx);
    utils_sha256_starts(ctx);

    return ctx;
}

void otalib_Sha256Update(void *sha256, const char *buf, size_t buf_len)
{
    utils_sha256_update(sha256, (unsigned char *)buf, buf_len);
}

void otalib_Sha256Finalize(void *sha256, char *output_str)
{
    int i;
    unsigned char buf_out[32];
    utils_sha256_finish(sha256, buf_out);

    for (i = 0; i < 32; ++i) {
        output_str[i * 2] = infra_hex2char(buf_out[i] >> 4);
        output_str[i * 2 + 1] = infra_hex2char(buf_out[i]);
    }
    output_str[64] = '\0';
}

void otalib_Sha256Deinit(void *sha256)
{
    utils_sha256_free(sha256);
    if (NULL != sha256) {
        OTA_FREE(sha256);
    }
}
/* Get the specific @key value, and copy to @dest */
/* 0, successful; -1, failed */
int otalib_GetFirmwareFixlenPara(const char *json_doc,
                                 size_t json_doc_len,
                                 const char *key,
                                 char *dest,
                                 size_t dest_len)
{
    const char *pvalue;
    uint32_t val_len;

    if (NULL == (pvalue = otalib_JsonValueOf(json_doc, json_doc_len, key, &val_len))) {
        OTA_LOG_ERROR("Not '%s' key in json doc of OTA", key);
        return -1;
    }

    if (val_len > dest_len) {
        OTA_LOG_ERROR("value length of the key is too long");
        return -1;
    }

    memcpy(dest, pvalue, val_len);

    return 0;
}


/* Get variant length parameter of firmware, and copy to @dest */
/* 0, successful; -1, failed */
int otalib_GetFirmwareVarlenPara(const char *json_doc,
                                 size_t json_doc_len,
                                 const char *key,
                                 char **dest)
{
    const char *pvalue;
    uint32_t val_len;

    if (NULL == (pvalue = otalib_JsonValueOf(json_doc, json_doc_len, key, &val_len))) {
        OTA_LOG_ERROR("Not %s key in json doc of OTA", key);
        return -1;
    }

    if (NULL == (*dest = OTA_MALLOC(val_len + 1))) {
        OTA_LOG_ERROR("allocate for dest failed");
        return -1;
    }

    memcpy(*dest, pvalue, val_len);
    (*dest)[val_len] = '\0';

    return 0;
}

int otalib_GetParams(const char *json_doc, uint32_t json_len, char **url, char **version, char *md5,
                     uint32_t *file_size)
{
#define OTA_FILESIZE_STR_LEN    (16)
    char file_size_str[OTA_FILESIZE_STR_LEN + 1] = {0};

    /* get version */
    if (0 != otalib_GetFirmwareVarlenPara(json_doc, json_len, "version", version)) {
        OTA_LOG_ERROR("get value of version key failed");
        return -1;
    }

    /* get URL */
    if (0 != otalib_GetFirmwareVarlenPara(json_doc, json_len, "url", url)) {
        OTA_LOG_ERROR("get value of url key failed");
        return -1;
    }

    /* get md5 */
    if (0 != otalib_GetFirmwareFixlenPara(json_doc, json_len, "md5", md5, 32)) {
        OTA_LOG_ERROR("get value of md5 key failed");
        return -1;
    }

    /* get file size */
    if (0 != otalib_GetFirmwareFixlenPara(json_doc, json_len, "size", file_size_str, OTA_FILESIZE_STR_LEN)) {
        OTA_LOG_ERROR("get value of size key failed");
        return -1;
    }
    file_size_str[OTA_FILESIZE_STR_LEN] = '\0';
    *file_size = atoi(file_size_str);

    return 0;

#undef OTA_FILESIZE_STR_LEN
}

int otalib_GetConfigParams(const char *json_doc, uint32_t json_len, char **configId, uint32_t *configSize, char **sign,
                           char **signMethod, char **url, char **getType)
{
#define OTA_FILESIZE_STR_LEN    (16)
    char file_size_str[OTA_FILESIZE_STR_LEN + 1];

    /* get configId */
    if (0 != otalib_GetFirmwareVarlenPara(json_doc, json_len, "configId", configId)) {
        OTA_LOG_ERROR("get value of configId key failed");
        return -1;
    }

    /* get configSize */
    if (0 != otalib_GetFirmwareFixlenPara(json_doc, json_len, "configSize", file_size_str, OTA_FILESIZE_STR_LEN)) {
        OTA_LOG_ERROR("get value of size key failed");
        return -1;
    }
    file_size_str[OTA_FILESIZE_STR_LEN] = '\0';
    *configSize = atoi(file_size_str);

    /* get sign */
    if (0 != otalib_GetFirmwareVarlenPara(json_doc, json_len, "sign", sign)) {
        OTA_LOG_ERROR("get value of sign key failed");
        return -1;
    }

    /* get signMethod */
    if (0 != otalib_GetFirmwareVarlenPara(json_doc, json_len, "signMethod", signMethod)) {
        OTA_LOG_ERROR("get value of signMethod key failed");
        return -1;
    }

    /* get url */
    if (0 != otalib_GetFirmwareVarlenPara(json_doc, json_len, "url", url)) {
        OTA_LOG_ERROR("get value of url key failed");
        return -1;
    }

    /* get getType */
    if (0 != otalib_GetFirmwareVarlenPara(json_doc, json_len, "getType", getType)) {
        OTA_LOG_ERROR("get value of getType key failed");
        return -1;
    }
    return 0;

#undef OTA_FILESIZE_STR_LEN
}

/* Generate firmware information according to @id, @version */
/* and then copy to @buf. */
/* 0, successful; -1, failed */
int otalib_GenInfoMsg(char *buf, size_t buf_len, uint32_t id, const char *version)
{
    int ret;
    ret = HAL_Snprintf(buf,
                       buf_len,
                       "{\"id\":%d,\"params\":{\"version\":\"%s\"}}",
                       id,
                       version);

    if (ret < 0) {
        OTA_LOG_ERROR("HAL_Snprintf failed");
        return -1;
    }

    return 0;
}

/* Generate report information according to @id, @msg */
/* and then copy to @buf. */
/* 0, successful; -1, failed */
int otalib_GenReportMsg(char *buf, size_t buf_len, uint32_t id, int progress, const char *msg_detail)
{
    int ret;
    if (NULL == msg_detail) {
        ret = HAL_Snprintf(buf,
                           buf_len,
                           "{\"id\":%d,\"params\":{\"step\":\"%d\",\"desc\":\"\"}}",
                           id,
                           progress);
    } else {
        ret = HAL_Snprintf(buf,
                           buf_len,
                           "{\"id\":%d,\"params\":{\"step\":\"%d\",\"desc\":\"%s\"}}",
                           id,
                           progress,
                           msg_detail);
    }


    if (ret < 0) {
        OTA_LOG_ERROR("HAL_Snprintf failed");
        return -1;
    } else if (ret >= buf_len) {
        OTA_LOG_ERROR("msg is too long");
        return IOT_OTAE_STR_TOO_LONG;
    }

    return 0;
}


