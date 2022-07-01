/**
 * @file gateway_scene.c
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

#include "service_mqtt.h"
#include "qcloud_iot_gateway_scene.h"

typedef enum {
    GATEWAY_SCENE_DOWN_MESSAGE_TYPE_SCENE_HANDLES = 0,
    GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RUN_SCENE,
    GATEWAY_SCENE_DOWN_MESSAGE_TYPE_DELETE_SCENE,
    GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RELOAD_SCENE_HANDLES_REPLY,
    GATEWAY_SCENE_DOWN_MESSAGE_TYPE_REPORT_INNER_SCENE_LIST_REPLY,
} GatewaySceneDownMessageType;

static const char *sg_gateway_scene_method_str[] = {
    [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_SCENE_HANDLES]                 = "gateway_scene_handles",
    [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RUN_SCENE]                     = "gateway_run_scene",
    [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_DELETE_SCENE]                  = "gateway_delete_scene",
    [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RELOAD_SCENE_HANDLES_REPLY]    = "gateway_reload_scene_handles_reply",
    [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_REPORT_INNER_SCENE_LIST_REPLY] = "gateway_report_inner_scene_list_reply",
};

typedef struct {
    IoTGatewaySceneCallback callback;
    void                   *usr_data;
} GatewaySceneContext;

/**
 * @brief Handle file mange down stream message.
 *
 * @param[in] type @see FileManageDownMessageType
 * @param[in] message @see MQTTMessage
 * @param[in] callback @see IotFileManageCallback
 * @param[in,out] usr_data user define in IOT_FileManage_Init
 */
static void _parse_update_payload_and_callback(GatewaySceneDownMessageType type, const MQTTMessage *message,
                                               const IoTGatewaySceneCallback *callback, void *usr_data)
{
    int            rc, result_code;
    UtilsJsonValue scene_id, scene_name, scene_update_time, scene_handles, scene_result, status;

    // callback
    switch (type) {
        case GATEWAY_SCENE_DOWN_MESSAGE_TYPE_SCENE_HANDLES:
            if (!callback->gateway_scene_handles_callback) {
                return;
            }
            rc = utils_json_value_get("params.SceneId", sizeof("params.sceneId") - 1, message->payload_str,
                                      message->payload_len, &scene_id);
            rc |= utils_json_value_get("params.SceneName", sizeof("params.SceneName") - 1, message->payload_str,
                                       message->payload_len, &scene_name);
            rc |= utils_json_value_get("params.SceneUpdateTime", sizeof("params.SceneUpdateTime") - 1,
                                       message->payload_str, message->payload_len, &scene_update_time);
            rc |= utils_json_value_get("params.Handles", sizeof("params.Handles") - 1, message->payload_str,
                                       message->payload_len, &scene_handles);

            if (rc) {
                goto error;
            }
            callback->gateway_scene_handles_callback(scene_id, scene_name, scene_update_time, scene_handles, usr_data);
            break;
        case GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RUN_SCENE:
            if (!callback->gateway_run_scene_callback) {
                return;
            }
            rc = utils_json_value_get("params.SceneId", sizeof("params.sceneId") - 1, message->payload_str,
                                      message->payload_len, &scene_id);
            if (rc) {
                goto error;
            }
            callback->gateway_run_scene_callback(scene_id, usr_data);
            break;
        case GATEWAY_SCENE_DOWN_MESSAGE_TYPE_DELETE_SCENE:
            if (!callback->gateway_delete_scene_callback) {
                return;
            }
            rc = utils_json_value_get("params.SceneId", sizeof("params.sceneId") - 1, message->payload_str,
                                      message->payload_len, &scene_id);
            if (rc) {
                goto error;
            }
            callback->gateway_delete_scene_callback(scene_id, usr_data);
            break;
        case GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RELOAD_SCENE_HANDLES_REPLY:
            if (!callback->gateway_reload_scene_reply_callback) {
                return;
            }
            rc = utils_json_get_int32("code", sizeof("code") - 1, message->payload_str, message->payload_len,
                                      &result_code);
            rc |= utils_json_value_get("status", sizeof("status") - 1, message->payload_str, message->payload_len,
                                       &status);
            rc |= utils_json_value_get("sceneResult", sizeof("sceneResult") - 1, message->payload_str,
                                       message->payload_len, &scene_result);
            if (rc) {
                goto error;
            }
            callback->gateway_reload_scene_reply_callback(result_code, status, scene_result, usr_data);
            break;
        default:
            break;
    }
    return;
error:
    Log_e("invalid format of payload!");
    return;
}

/**
 * @brief Gateway scene callback
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see GatewaySceneContext
 */
static void _gateway_scene_message_callback(void *client, const MQTTMessage *message, void *usr_data)
{
    const char *gateway_scene_method_str[] = {
        [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_SCENE_HANDLES]                 = "gateway_scene_handles",
        [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RUN_SCENE]                     = "gateway_run_scene",
        [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_DELETE_SCENE]                  = "gateway_delete_scene",
        [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_RELOAD_SCENE_HANDLES_REPLY]    = "gateway_reload_scene_handles_reply",
        [GATEWAY_SCENE_DOWN_MESSAGE_TYPE_REPORT_INNER_SCENE_LIST_REPLY] = "gateway_report_inner_scene_list_reply",
    };

    int rc, i = 0;

    GatewaySceneContext *gateway_scene_context = (GatewaySceneContext *)usr_data;
    UtilsJsonValue       method;

    Log_d("receive gateway scene message:%.*s", message->payload_len, message->payload_str);

    rc = utils_json_value_get("method", sizeof("method") - 1, message->payload_str, message->payload_len, &method);
    if (rc) {
        Log_e("invalid file manage message!");
        return;
    }

    for (i = GATEWAY_SCENE_DOWN_MESSAGE_TYPE_SCENE_HANDLES;
         i <= GATEWAY_SCENE_DOWN_MESSAGE_TYPE_REPORT_INNER_SCENE_LIST_REPLY; i++) {
        if (!strncmp(method.value, gateway_scene_method_str[i], method.value_len)) {
            Log_d("callback file manage message!");
            _parse_update_payload_and_callback(i, message, &gateway_scene_context->callback,
                                               gateway_scene_context->usr_data);
        }
    }
}

/**
 * @brief   gateway scene init, register handler to server list.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IoTGatewaySceneCallback
 * @param[in] usr_data usr data used in callback
 * @return 0 for success, or err code (<0) @see IotReturnCode
 */
int IOT_GatewayScene_Init(void *client, IoTGatewaySceneCallback callback, void *usr_data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    int rc = service_mqtt_init(client);
    if (rc) {
        return rc;
    }

    GatewaySceneContext *gateway_scene_context = (GatewaySceneContext *)HAL_Malloc(sizeof(GatewaySceneContext));
    if (!gateway_scene_context) {
        return QCLOUD_ERR_MALLOC;
    }
    gateway_scene_context->callback = callback;
    gateway_scene_context->usr_data = usr_data;

    ServiceRegisterParams params = {
        .type           = SERVICE_TYPE_GATEWAY_SCENE,
        .method_list    = sg_gateway_scene_method_str,
        .method_num     = sizeof(sg_gateway_scene_method_str) / sizeof(sg_gateway_scene_method_str[0]),
        .message_handle = _gateway_scene_message_callback,
        .usr_data       = gateway_scene_context,
        .user_data_free = HAL_Free,
    };

    rc = service_mqtt_service_register(client, &params);
    if (rc) {
        HAL_Free(gateway_scene_context);
    }
    return rc;
}

/**
 * @brief Gateway scene deinit, unregister handler from server list.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_GatewayScene_Deinit(void *client)
{
    POINTER_SANITY_CHECK_RTN(client);

    service_mqtt_service_unregister(client, SERVICE_TYPE_GATEWAY_SCENE);
    service_mqtt_deinit(client);
}

/**
 * @brief reload gateway scene from cloud.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] buf publish message buffer
 * @param[in] buf_len buffer len
 * @return packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_GatewayScene_Reload(void *client, char *buf, int buf_len)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_FAILURE);
    STRING_PTR_SANITY_CHECK(buf, QCLOUD_ERR_FAILURE);

    static int  index   = 0;
    DeviceInfo *devInfo = IOT_MQTT_GetDeviceInfo(client);
    HAL_Snprintf(buf, buf_len,
                 "{\"method\":\"gateway_reload_scene_handles\",\"clientToken\":\"gateway-reload-scene-handles-%s-%d\", "
                 "\"params\":{\"SceneId\":\"*\"}}",
                 devInfo->product_id, index++);
    return service_mqtt_publish(client, QOS0, buf, buf_len);
}

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
                                     int list_count)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_FAILURE);
    STRING_PTR_SANITY_CHECK(buf, QCLOUD_ERR_FAILURE);

    static int  index   = 0;
    DeviceInfo *devInfo = IOT_MQTT_GetDeviceInfo(client);

    int format_len = HAL_Snprintf(
        buf, buf_len,
        "{\"method\":\"gateway_report_inner_scene_list\",\"clientToken\":\"gateway_report_inner_scene_list-%s-%d\", "
        "\"timestamp\":%ld,\"params\":{\"InnerSceneList\":[",
        devInfo->product_id, index++, IOT_Timer_CurrentSec());

    for (int i = 0; i < list_count; i++) {
        format_len += HAL_Snprintf(buf + format_len, buf_len - format_len,
                                   "{\"InnerSceneId\":\"%s\", \"InnerSceneName\":\"%s\"},", list[i].inner_scene_id,
                                   list[i].inner_scene_name);
        if (format_len >= buf_len) {
            Log_e("buffer is too small(%d)", buf_len);
            return QCLOUD_ERR_FAILURE;
        }
    }
    buf[format_len - 1] = ']';
    buf[format_len]     = '}';
    buf[format_len + 1] = '}';
    buf[format_len + 2] = '\0';
    return service_mqtt_publish(client, QOS0, buf, buf_len);
}