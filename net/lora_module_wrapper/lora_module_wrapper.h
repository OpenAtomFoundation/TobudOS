/*----------------------------------------------------------------------------
 * Tencent is pleased to support the open source community by making TencentOS
 * available.
 *
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * If you have downloaded a copy of the TencentOS binary from Tencent, please
 * note that the TencentOS binary is licensed under the BSD 3-Clause License.
 *
 * If you have downloaded a copy of the TencentOS source code from Tencent,
 * please note that TencentOS source code is licensed under the BSD 3-Clause
 * License, except for the third-party components listed below which are
 * subject to different license terms. Your integration of TencentOS into your
 * own projects may require compliance with the BSD 3-Clause License, as well
 * as the other licenses applicable to the third-party components included
 * within TencentOS.
 *---------------------------------------------------------------------------*/

#ifndef _LORA_MODULE_WRAPPER_H_
#define  _LORA_MODULE_WRAPPER_H_

#include <stdint.h>
#include <stdio.h>

typedef void (*lora_recv_callback_t)(uint8_t *data, uint8_t len);

typedef struct lora_module_st {
    int (*init)(void);

    int (*join_otaa)(const char *deveui, const char *appkey);

    int (*join_abp)(const char *deveui, const char *devaddr, const char *nwkskey, const char *appskey);

    int (*send)(const void *buf, size_t len);

    int (*close)(void);

    lora_recv_callback_t recv_callback;
} lora_module_t;

/**
 * @brief Register a lora module.
 *
 * @attention None
 *
 * @param[in]   module      lora module
 *
 * @return  errcode
 */
int tos_lora_module_register(lora_module_t *module);

/**
 * @brief Initialize the lora module.
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_lora_module_init(void);

/**
 * @brief Join a lora gateway.
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_lora_module_join_otaa(const char *deveui, const char *appkey);

/**
 * @brief Join a lora gateway.
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_lora_module_join_abp(const char *deveui, const char *devaddr, const char *nwkskey, const char *appskey);

/**
 * @brief Send data by lora module.
 *
 * @attention None
 *
 * @param[in]   buf     data to send
 * @param[in]   len     length of the data
 *
 * @return  errcode
 */
int tos_lora_module_send(const void *buf, size_t len);

/**
 * @brief Register a Receive callback method by lora module.
 *
 * @attention None
 *
 * @param[in]   mcps_indication     receive callback method
 *
 * @return  errcode
 */
int tos_lora_module_recvcb_register(lora_recv_callback_t recv_callback);

/**
 * @brief Close the lora module.
 *
 * @attention None
 *
 * @return  errcode
 */
int tos_lora_module_close(void);

#endif

