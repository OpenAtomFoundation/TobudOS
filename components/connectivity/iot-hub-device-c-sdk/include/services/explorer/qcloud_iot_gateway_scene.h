/**
 * @file qcloud_iot_gateway_scene.h
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-06-15
 *
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2021 THL A29 Limited, a Tencent company.All rights reserved.
 *
 * Licensed under the MIT License(the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @par Change Log:
 * <table>
 * Date				Version		Author			Description
 * 2022-06-15		1.0			hubertxxu		first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_GATEWAY_SCENE_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_GATEWAY_SCENE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "qcloud_iot_common.h"

#define MAX_LENGTH_INNER_SCENE_ID   (64)
#define MAX_LENGTH_INNER_SCENE_NAME (64)
#define MAX_LENGTH_INNER_SCENE_LIST (20)

/**
 * @brief callback of gateway scene
 *
 */
typedef struct {
    int (*gateway_scene_handles_callback)(UtilsJsonValue scene_id, UtilsJsonValue scene_name,
                                          UtilsJsonValue scene_update_time, UtilsJsonValue scene_handles,
                                          void *usr_data);
    int (*gateway_run_scene_callback)(UtilsJsonValue scene_id, void *usr_data);
    int (*gateway_delete_scene_callback)(UtilsJsonValue scene_id, void *usr_data);
    int (*gateway_reload_scene_reply_callback)(int result_code, UtilsJsonValue status, UtilsJsonValue scene_result,
                                               void *usr_data);
} IoTGatewaySceneCallback;

/**
 * @brief gateway scene inner list
 *
 */
typedef struct {
    char inner_scene_id[MAX_LENGTH_INNER_SCENE_ID];
    char inner_scene_name[MAX_LENGTH_INNER_SCENE_NAME];
} IoTGatewaySceneInnerList;

/**
 * @brief   gateway scene init, register handler to server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IoTGatewaySceneCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_GatewayScene_Init(void *client, IoTGatewaySceneCallback callback, void *usr_data);

/**
 * @brief Gateway scene deinit, unregister handler from server list.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_GatewayScene_Deinit(void *client);

/**
 * @brief reload gateway scene from cloud.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_GatewayScene_Reload(void *client, char *buf, int buf_len);

/**
 * @brief report gateway local scene
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @param list local scene list
 * @param list_count local scene list count
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_GatewayScene_ReportInnerList(void *client, char *buf, int buf_len, IoTGatewaySceneInnerList *list,
                                     int list_count);

#ifdef __cplusplus
}
#endif

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_EXPLORER_QCLOUD_IOT_GATEWAY_SCENE_H_