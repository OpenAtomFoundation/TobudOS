#ifndef __DEV_RESET_API_H__
#define __DEV_RESET_API_H__

#include "infra_types.h"
#include "infra_defs.h"

typedef enum {
    IOTX_DEVRST_EVT_RECEIVED
} iotx_devrst_evt_type_t;

typedef struct {
    int msgid;
    char *payload;
    uint32_t payload_len;
} iotx_devrst_evt_recv_msg_t;

typedef void (*iotx_devrst_evt_handle_t)(iotx_devrst_evt_type_t evt, void *msg);

/**
 * @brief   report reset message to cloud.
 *
 * @param meta_info. device meta info, only product_key and device_name needed.
 * @param extended. reserved.
 * 
 * @retval  -1 : failure
 * @retval  0 : sucess
 */
int IOT_DevReset_Report(iotx_dev_meta_info_t *meta_info, iotx_devrst_evt_handle_t handle, void *extended);

#endif

