/*
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright (C) 2018-2020 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef IOT_GATEWAY_COMMON_H_
#define IOT_GATEWAY_COMMON_H_

#include "qcloud_iot_export.h"

#define GATEWAY_PAYLOAD_BUFFER_LEN 1024
#define GATEWAY_RECEIVE_BUFFER_LEN 1024
#define GATEWAY_LOOP_MAX_COUNT     100

/* The format of operation of gateway topic */
#define GATEWAY_TOPIC_OPERATION_FMT "$gateway/operation/%s/%s"

/* The format of operation result of gateway topic */
#define GATEWAY_TOPIC_OPERATION_RESULT_FMT "$gateway/operation/result/%s/%s"

/* The format of gateway client id */
#define GATEWAY_CLIENT_ID_FMT "%s/%s"

/* The format of operation result of gateway topic */
#define GATEWAY_PAYLOAD_STATUS_FMT \
    "{\"type\":\"%s\",\"payload\":{\"devices\":[{\"product_id\":\"%s\",\"device_name\":\"%s\"}]}}"

/* Subdevice    seesion status */
typedef enum _SubdevSessionStatus {
    /* Initial */
    SUBDEV_SEESION_STATUS_INIT,

    /* Online */
    SUBDEV_SEESION_STATUS_ONLINE,

    /* Offline */
    SUBDEV_SEESION_STATUS_OFFLINE,

    /* Maximum number of seesion status type */
    SUBDEV_SEESION_STATUS_MAX
} SubdevSessionStatus;

/* The structure of subdevice session */
typedef struct _SubdevSession {
    char                   product_id[MAX_SIZE_OF_PRODUCT_ID + 1];
    char                   device_name[MAX_SIZE_OF_DEVICE_NAME + 1];
    SubdevSessionStatus    session_status;
    struct _SubdevSession *next;
} SubdevSession;

/* The structure of common reply data */
typedef struct _ReplyData {
    int32_t result;
    char    client_id[MAX_SIZE_OF_CLIENT_ID + 1];
} ReplyData;

/* The structure of gateway data */
typedef struct _GatewayData {
    int32_t   sync_status;
    ReplyData online;
    ReplyData offline;
} GatewayData;

/* The structure of gateway context */
typedef struct _Gateway {
    void *           mqtt;
    SubdevSession *  session_list;
    GatewayData      gateway_data;
    MQTTEventHandler event_handle;
    int              is_construct;
    char             recv_buf[GATEWAY_RECEIVE_BUFFER_LEN];
} Gateway;

SubdevSession *subdev_add_session(Gateway *gateway, char *product_id, char *device_name);

SubdevSession *subdev_find_session(Gateway *gateway, char *product_id, char *device_name);

int subdev_remove_session(Gateway *gateway, char *product_id, char *device_name);

int gateway_subscribe_unsubscribe_topic(Gateway *gateway, char *topic_filter, SubscribeParams *params,
                                        int is_subscribe);

int gateway_subscribe_unsubscribe_default(Gateway *gateway, GatewayParam *param);

int gateway_publish_sync(Gateway *gateway, char *topic, PublishParams *params, int32_t *result);

#endif /* IOT_GATEWAY_COMMON_H_ */
