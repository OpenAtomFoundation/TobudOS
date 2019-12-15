/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef _DM_OTA_H_
#define _DM_OTA_H_

typedef struct {
    void *ota_handle;
    char product_key[IOTX_PRODUCT_KEY_LEN + 1];
    char device_name[IOTX_DEVICE_NAME_LEN + 1];
} dm_ota_ctx_t;

int dm_ota_init(void);
int dm_ota_sub(void);
int dm_ota_deinit(void);
int dm_ota_get_ota_handle(void **handle);
#ifdef DEVICE_MODEL_GATEWAY
    #ifdef DEVICE_MODEL_SUBDEV_OTA
        int dm_ota_switch_device(int devid);
    #endif
#endif
#endif
