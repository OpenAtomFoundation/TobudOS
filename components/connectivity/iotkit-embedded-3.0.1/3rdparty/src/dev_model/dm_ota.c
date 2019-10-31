/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#include "iotx_dm_internal.h"

#if defined(OTA_ENABLED) && !defined(BUILD_AOS)

static dm_ota_ctx_t g_dm_ota_ctx;

static dm_ota_ctx_t *_dm_ota_get_ctx(void)
{
    return &g_dm_ota_ctx;
}

int dm_ota_init(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();
    memset(ctx, 0, sizeof(dm_ota_ctx_t));

    HAL_GetProductKey(ctx->product_key);
    HAL_GetDeviceName(ctx->device_name);

    return SUCCESS_RETURN;
}

int dm_ota_sub(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();
    void *handle = NULL;

    /* Init OTA Handle */
    handle = IOT_OTA_Init(ctx->product_key, ctx->device_name, NULL);
    if (handle == NULL) {
        return FAIL_RETURN;
    }

    ctx->ota_handle = handle;

    return SUCCESS_RETURN;
}

int dm_ota_deinit(void)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (ctx->ota_handle) {
        IOT_OTA_Deinit(ctx->ota_handle);
        ctx->ota_handle = NULL;
    }

    return SUCCESS_RETURN;
}
#ifdef DEVICE_MODEL_GATEWAY
#ifdef DEVICE_MODEL_SUBDEV_OTA
int dm_ota_switch_device(int devid)
{
    char pk[IOTX_PRODUCT_KEY_LEN + 1] = {0};
    char dn[IOTX_DEVICE_NAME_LEN + 1] = {0};
    char ds[IOTX_DEVICE_SECRET_LEN + 1] = {0};
    int ret = dm_mgr_search_device_by_devid(devid, pk, dn, ds);
    void *ota_handle = NULL;
    int res = -1;
    dm_ota_ctx_t *ctx = NULL;

    if (SUCCESS_RETURN != ret) {
        dm_log_err("could not find device by id, ret is %d", ret);
        return FAIL_RETURN;
    }
    dm_log_info("do subdevice ota, pk, dn is %s, %s", pk, dn);

    ota_handle = NULL;
    res = dm_ota_get_ota_handle(&ota_handle);

    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* if currently a device is doing OTA, do not interrupt */
    if (IOT_OTA_IsFetching(ota_handle)) {
        dm_log_info("OTA is processing, can not switch to another device");
        return FAIL_RETURN;
    }

    dm_ota_deinit();
    ctx = _dm_ota_get_ctx();
    memset(ctx, 0, sizeof(dm_ota_ctx_t));

    memcpy(ctx->product_key, pk, strlen(pk) + 1);
    memcpy(ctx->device_name, dn, strlen(dn) + 1);
    ret = dm_ota_sub();
    if (ret < 0) {
        dm_log_err("dm_ota_sub ret is %d, %s, %s\n", ret, pk, dn);
    }
    return ret;
}
#endif
#endif

int dm_ota_get_ota_handle(void **handle)
{
    dm_ota_ctx_t *ctx = _dm_ota_get_ctx();

    if (handle == NULL || *handle != NULL) {
        return FAIL_RETURN;
    }

    if (ctx->ota_handle == NULL) {
        return FAIL_RETURN;
    }

    *handle = ctx->ota_handle;

    return SUCCESS_RETURN;
}
#endif
