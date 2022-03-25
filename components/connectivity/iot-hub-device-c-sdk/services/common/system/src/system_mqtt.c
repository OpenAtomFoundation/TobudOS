/**
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
 * @file system_mqtt.c
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-24
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-24 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2022-01-26 <td>1.1     <td>hubert    <td>add serverip
 * </table>
 */

#include "qcloud_iot_system.h"

#define MAX_SYSTEM_INFO_SIZE 256

#define IP_MAX_NUM      10
#define ONE_IP_MAX_LEN  24
#define IP_LIST_MAX_LEN (IP_MAX_NUM * ONE_IP_MAX_LEN)

#define SYS_MQTT_GET_RESOURCE_TIME     "{\"type\":\"get\",\"resource\":[\"time\"]}"
#define SYS_MQTT_GET_RESOURCE_SERVERIP "{\"type\":\"get\",\"resource\":[\"serverip\"]}"

typedef enum {
    RESOURCE_TIME = 0,
    RESOURCE_IP,
} SysResourceType;

/**
 * @brief data structure for system time service
 */
typedef struct {
    bool result_recv_ok;
    // time
    struct {
        uint32_t time;
        uint64_t ntptime1;
        uint64_t ntptime2;
    } time;
    // server ip
    struct {
        char ip_list[IP_LIST_MAX_LEN];
        int  ip_len;
    } server_ip;
    SysResourceType wait_type;
    uint64_t        result_recv_time;
} SystemResultInfo;

/**
 * @brief
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] message message from topic
 * @param[in,out] usr_data pointer to @see SystemResultInfo
 */
static void _system_mqtt_message_callback(void *client, const MQTTMessage *message, void *usr_data)
{
    int               rc     = 0;
    SystemResultInfo *result = (SystemResultInfo *)usr_data;
    UtilsJsonValue    value;

    Log_d("Receive system result message:%.*s", message->payload_len, message->payload_str);
    if (strstr(message->payload_str, "\"time\"")) {
        // get time
        rc = utils_json_value_get("time", strlen("time"), message->payload_str, message->payload_len, &value);
        if (rc) {
            return;
        }
        rc = utils_json_value_data_get(value, UTILS_JSON_VALUE_TYPE_UINT32, &result->time.time);
        if (rc) {
            return;
        }

        // get ntptime1
        rc = utils_json_value_get("ntptime1", strlen("ntptime1"), message->payload_str, message->payload_len, &value);
        if (rc) {
            return;
        }
        rc = utils_json_value_data_get(value, UTILS_JSON_VALUE_TYPE_UINT64, &result->time.ntptime1);
        if (rc) {
            return;
        }

        // get ntptime2
        rc = utils_json_value_get("ntptime2", strlen("ntptime2"), message->payload_str, message->payload_len, &value);
        if (rc) {
            return;
        }
        rc = utils_json_value_data_get(value, UTILS_JSON_VALUE_TYPE_UINT64, &result->time.ntptime2);
        if (rc) {
            result->time.ntptime1 = result->time.time * 1000;
            result->time.ntptime2 = result->time.time * 1000;
        }
    }
    if (strstr(message->payload_str, "\"serverip\"")) {
        // get serverip
        rc = utils_json_value_get("serverip", strlen("serverip"), message->payload_str, message->payload_len, &value);
        if (rc) {
            return;
        }
        result->server_ip.ip_len = value.value_len > IP_LIST_MAX_LEN ? IP_LIST_MAX_LEN : value.value_len;
        memset(result->server_ip.ip_list, 0, IP_LIST_MAX_LEN);
        strncpy(result->server_ip.ip_list, value.value, result->server_ip.ip_len);
    }

    result->result_recv_time = HAL_Timer_CurrentMs();
    result->result_recv_ok   = true;
}

/**
 * @brief Check and subscribe system result topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
static int _system_mqtt_result_topic_check_and_sub(void *client, const char *topic)
{
    int rc = 0;
    // 1. check topic is sub ready
    if (IOT_MQTT_IsSubReady(client, topic)) {
        return rc;
    }

    // 2. subscribe
    SubscribeParams   sub_params  = DEFAULT_SUB_PARAMS;
    SystemResultInfo *system_info = (SystemResultInfo *)HAL_Malloc(sizeof(SystemResultInfo));
    if (!system_info) {
        return QCLOUD_ERR_MALLOC;
    }

    system_info->result_recv_ok = false;

    sub_params.on_message_handler = _system_mqtt_message_callback;
    sub_params.qos                = QOS1;
    sub_params.user_data          = system_info;
    sub_params.user_data_free     = HAL_Free;

    rc = IOT_MQTT_SubscribeSync(client, topic, &sub_params);
    if (rc) {
        HAL_Free(system_info);
    }
    return rc;
}

/**
 * @brief Publish get time message to system topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic system result topic
 * @return >= 0 for success
 */
static int _system_mqtt_get_resource_publish(void *client, const char *topic, SysResourceType type)
{
    char pub_topic_name[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(pub_topic_name, sizeof(pub_topic_name), "$sys/operation/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    SystemResultInfo *result = IOT_MQTT_GetSubUsrData(client, topic);
    if (!result) {
        return QCLOUD_ERR_FAILURE;
    }
    result->wait_type      = type;
    result->result_recv_ok = false;

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = QOS0;
    switch (type) {
        case RESOURCE_TIME:
            pub_params.payload     = SYS_MQTT_GET_RESOURCE_TIME;
            pub_params.payload_len = strlen(SYS_MQTT_GET_RESOURCE_TIME);
            break;
        case RESOURCE_IP:
            pub_params.payload     = SYS_MQTT_GET_RESOURCE_SERVERIP;
            pub_params.payload_len = strlen(SYS_MQTT_GET_RESOURCE_SERVERIP);
        default:
            break;
    }
    return IOT_MQTT_Publish(client, pub_topic_name, &pub_params);
}

/**
 * @brief Wait system result, timeout @see QCLOUD_IOT_MQTT_WAIT_ACK_TIMEOUT
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] topic system result topic
 * @param[out] time time from system result topic
 * @return @see IotReturnCode
 */
static int _system_mqtt_result_wait(void *client, const char *topic, void *user_data)
{
    int               rc = 0;
    Timer             wait_result_timer;
    SystemResultInfo *result;

    HAL_Timer_CountdownMs(&wait_result_timer, QCLOUD_IOT_MQTT_WAIT_ACK_TIMEOUT);

    result = IOT_MQTT_GetSubUsrData(client, topic);
    if (!result) {
        return QCLOUD_ERR_FAILURE;
    }
    result->result_recv_ok = false;
    while (!rc && !HAL_Timer_Expired(&wait_result_timer)) {
        rc = IOT_MQTT_Yield(client, QCLOUD_IOT_MQTT_YIELD_TIMEOUT);
        if (result->result_recv_ok) {
            switch (result->wait_type) {
                case RESOURCE_TIME:
                    *((uint32_t *)user_data) = result->time.time;
                    break;
                case RESOURCE_IP:
                    strncpy(user_data, result->server_ip.ip_list, result->server_ip.ip_len);
                    result->server_ip.ip_len = 0;
                default:
                    break;
            }
            return QCLOUD_RET_SUCCESS;
        }
    }
    return QCLOUD_ERR_MQTT_REQUEST_TIMEOUT;
}

static int _system_mqtt_get_resource(void *client, SysResourceType type, void *user_data)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(user_data, QCLOUD_ERR_INVAL);

    int  rc = 0;
    char system_result_topic[MAX_SIZE_OF_CLOUD_TOPIC];

    HAL_Snprintf(system_result_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$sys/operation/result/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    rc = _system_mqtt_result_topic_check_and_sub(client, system_result_topic);
    if (rc) {
        return rc;
    }

    rc = _system_mqtt_get_resource_publish(client, system_result_topic, type);
    if (rc < 0) {
        return rc;
    }

    return _system_mqtt_result_wait(client, system_result_topic, user_data);
}
/**
 * @brief Get time from system result topic
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] time time from system result topic
 * @return @see IotReturnCode
 */
int IOT_Sys_GetTime(void *client, uint32_t *time)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(time, QCLOUD_ERR_INVAL);
    return _system_mqtt_get_resource(client, RESOURCE_TIME, time);
}

/**
 * @brief Get ntp time from system result topic and set to system.
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see IotReturnCode
 */
int IOT_Sys_SyncNTPTime(void *client)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);

    int      rc       = 0;
    uint32_t time_get = 0;
    uint64_t local_publish_before, local_ntptime = 0;

    SystemResultInfo *result = NULL;

    char system_result_topic[MAX_SIZE_OF_CLOUD_TOPIC];

    HAL_Snprintf(system_result_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$sys/operation/result/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(IOT_MQTT_GetDeviceInfo(client)->device_name));

    rc = _system_mqtt_result_topic_check_and_sub(client, system_result_topic);
    if (rc) {
        return rc;
    }

    // prepare for publish
    result = IOT_MQTT_GetSubUsrData(client, system_result_topic);
    if (!result) {
        return -1;
    }
    local_publish_before = HAL_Timer_CurrentMs();

    // publish and wait
    rc = _system_mqtt_get_resource_publish(client, system_result_topic, RESOURCE_TIME);
    if (rc < 0) {
        return rc;
    }

    rc = _system_mqtt_result_wait(client, system_result_topic, &time_get);
    if (rc) {
        return rc;
    }

    local_ntptime =
        (result->time.ntptime2 + result->time.ntptime1 + result->result_recv_time - local_publish_before) / 2;

    rc = HAL_Timer_SetSystimeSec(time_get);
    if (rc) {
        Log_e("set systime sec failed, timestamp %d sec,  please check permission or other ret:%d", time_get, rc);
    } else {
        Log_i("set systime sec success, timestamp %d sec", time_get);
    }

    rc = HAL_Timer_SetSystimeMs(local_ntptime);
    if (rc) {
        Log_e("set systime ms failed, timestamp %lld, please check permission or other ret :%d", local_ntptime, rc);
    } else {
        Log_i("set systime ms success, timestamp %lld ms", local_ntptime);
    }
    return rc;
}

/**
 * @brief Get serverip from system result topic
 *
 * @param[in,out] client pointer to mqtt client
 * @param[out] server_ip serverip from system result topic
 * @return @see IotReturnCode
 */
int IOT_Sys_GetServerIp(void *client, char *server_ip)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_INVAL);
    POINTER_SANITY_CHECK(server_ip, QCLOUD_ERR_INVAL);

    return _system_mqtt_get_resource(client, RESOURCE_IP, server_ip);
}