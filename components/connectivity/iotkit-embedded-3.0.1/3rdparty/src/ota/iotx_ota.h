/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __IOTX_OTA_H__
#define __IOTX_OTA_H__

int iotx_ota_get_config(void *handle, const char *configScope, const char *getType,
                        const char *attributeKeys);

int iotx_req_image(void *handle, const char *version);

#endif  /* #ifndef __IOTX_OTA_H__ */


