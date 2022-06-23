/**
 * @file gateway_subdev_handle.c
 * @author {hubert} ({hubertxxu@tencent.com})
 * @brief
 * @version 1.0
 * @date 2022-05-25
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
 * 2022-05-25		1.0			hubertxxu		first commit
 * </table>
 */

#include "qcloud_iot_gateway.h"
#include "utils_list.h"

typedef struct {
    void *subdev_list;
    void *mqtt_client;
} GatewaySubDevHandle;

typedef enum {
    GATEWAY_SUBDEV_OFFLINE    = 0,  // 离线
    GATEWAY_SUBDEV_OFFLINEING = 1,  // 发起离线
    GATEWAY_SUBDEV_ONLINING   = 2,  // 发起上线
    GATEWAY_SUBDEV_ONLINE     = 3,  // 在线
} GatewaySubDevOnlineStatus;

typedef enum {
    GATEWAY_SUBDEV_UNBIND    = 0,
    GATEWAY_SUBDEV_UNBINDING = 2,
    GATEWAY_SUBDEV_BINDING   = 3,
    GATEWAY_SUBDEV_BINDED    = 4,

} GatewaySubDevBindStatus;

typedef struct {
    DeviceInfo dev_info;
    int        online_status;
    int        bind_status;
    bool       is_sub_property;
    void      *mqtt_client;
} GatewaySubDev;

typedef struct {
    GatewaySubDev *pair_sub;
    const char    *product_id;
    const char    *device_name;
} SubDevPair;

typedef enum {
    DEV_ARRAY_HANDLE_DESCRIBE    = 0,
    DEV_ARRAY_HANDEL_ONLINE      = 1,
    DEV_ARRAY_HANDLE_OFFLINE     = 2,
    DEV_ARRAY_HANDLE_BIND        = 3,
    DEV_ARRAY_HANDLE_UNBIND      = 4,
    DEV_ARRAY_HANDLE_CHANGE_BIND = 5,
} DevArrayHandleType;

typedef struct {
    GatewaySubDevHandle *gw_subdev_handle;
    DevArrayHandleType   type;
} DevArrayHandle;

static GatewaySubDevHandle *sg_gw_subdev_handle = NULL;

GatewaySubDev *iot_gateway_find_sub(GatewaySubDevHandle *sub_handle, const char *sub_pid, const char *sub_dname);

static UtilsListResult _find_subdev_by_name_str(void *list, void *node, void *val, void *usr_data);

// -----------------------------------------------------------------------
// sub device control && report
// ------------------------------------------------------------------------

int ext_subdev_control(const char *product_id, const char *device_name, int power_switch, int brightness)
{
    Log_d("control device [%s/%s] power_switch : %d, brightness : %d", product_id, device_name, power_switch,
          brightness);
    return QCLOUD_RET_SUCCESS;
}

int ext_subdev_report(const char *product_id, const char *device_name, int power_switch, int brightness)
{
    static int           token_num        = 0;
    GatewaySubDevHandle *gw_subdev_handle = sg_gw_subdev_handle;
    POINTER_SANITY_CHECK(gw_subdev_handle, QCLOUD_ERR_FAILURE);
    POINTER_SANITY_CHECK(product_id, QCLOUD_ERR_FAILURE);
    POINTER_SANITY_CHECK(device_name, QCLOUD_ERR_FAILURE);

    GatewaySubDev *sub = iot_gateway_find_sub(gw_subdev_handle, product_id, device_name);
    if (!sub || sub->bind_status != GATEWAY_SUBDEV_BINDED || sub->online_status != GATEWAY_SUBDEV_ONLINE) {
        if (sub) {
            Log_e("subdev [%s/%s] illegal, sub->bind_status : %d, sub->online_status : %d", sub->dev_info.product_id,
                  sub->dev_info.device_name, sub->bind_status, sub->online_status);
        } else {
            Log_e("subdev [%s/%s] illegal.", product_id, device_name);
        }
        return QCLOUD_ERR_FAILURE;
    }

    char property_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    char report_buf[256];
    HAL_Snprintf(property_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/up/property/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.device_name));
    int len = HAL_Snprintf(report_buf, sizeof(report_buf),
                           "{\"method\":\"report\",\"clientToken\":\"property-%s-%u\",\"params\":{\"power_switch\":%d, "
                           "\"brightness\":%d}}",
                           device_name, token_num++, power_switch, brightness);

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = 0;
    pub_params.payload       = (void *)report_buf;
    pub_params.payload_len   = len;
    return IOT_MQTT_Publish(gw_subdev_handle->mqtt_client, property_topic, &pub_params);
}

static void _data_template_property_message_handler(void *client, const MQTTMessage *message, void *usr_data)
{
    int            rc  = 0;
    GatewaySubDev *sub = (GatewaySubDev *)usr_data;
    char           buf[256];

    Log_d("receive subdev [%s/%s] property message:%.*s", sub->dev_info.product_id, sub->dev_info.device_name,
          message->payload_len, message->payload_str);

    UtilsJsonValue client_token, method;
    rc = utils_json_value_get("clientToken", strlen("clientToken"), (const char *)message->payload_str,
                              message->payload_len, &client_token);
    if (rc) {
        return;
    }
    rc = utils_json_value_get("method", strlen("method"), message->payload_str, message->payload_len, &method);
    if (rc) {
        return;
    }

    // only parse control
    if (strncmp(method.value, "control", method.value_len)) {
        return;
    }

    // parse params
    int power_switch = -1, brightness = -1;
    utils_json_get_int32("params.power_switch", strlen("params.power_switch"), message->payload_str,
                         message->payload_len, &power_switch);
    utils_json_get_int32("params.brightness", strlen("params.brightness"), message->payload_str, message->payload_len,
                         &brightness);

    rc = ext_subdev_control(sub->dev_info.product_id, sub->dev_info.device_name, power_switch, brightness);

    char property_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(property_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/up/property/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.device_name));
    int len = HAL_Snprintf(buf, sizeof(buf), "{\"method\":\"control_reply\",\"clientToken\":\"%.*s\",\"code\":%d}",
                           client_token.value_len, client_token.value, rc);

    PublishParams pub_params = DEFAULT_PUB_PARAMS;
    pub_params.qos           = 0;
    pub_params.payload       = (void *)buf;
    pub_params.payload_len   = len;
    IOT_MQTT_Publish(client, property_topic, &pub_params);
    ext_subdev_report(sub->dev_info.product_id, sub->dev_info.device_name, power_switch, brightness);
}

// ----------------------------------------------------
// subdev handle
// -----------------------------------------------------

static UtilsListResult _find_subdev_by_name_str(void *list, void *node, void *val, void *usr_data)
{
    GatewaySubDev *sub       = (GatewaySubDev *)val;
    SubDevPair    *march_sub = (SubDevPair *)usr_data;

    if (!strncmp(sub->dev_info.product_id, march_sub->product_id, MAX_SIZE_OF_PRODUCT_ID) &&
        !strncmp(sub->dev_info.device_name, march_sub->device_name, MAX_SIZE_OF_DEVICE_NAME)) {
        march_sub->pair_sub = sub;
        return LIST_TRAVERSE_BREAK;
    }
    return LIST_TRAVERSE_CONTINUE;
}

GatewaySubDev *iot_gateway_find_sub(GatewaySubDevHandle *sub_handle, const char *sub_pid, const char *sub_dname)
{
    POINTER_SANITY_CHECK(sub_handle, NULL);
    POINTER_SANITY_CHECK(sub_pid, NULL);
    POINTER_SANITY_CHECK(sub_dname, NULL);

    SubDevPair sub_info = {NULL, sub_pid, sub_dname};

    utils_list_process(sub_handle->subdev_list, LIST_HEAD, _find_subdev_by_name_str, &sub_info);

    return sub_info.pair_sub;
}

static UtilsListResult _remove_subdev_by_name_str(void *list, void *node, void *val, void *usr_data)
{
    GatewaySubDev *sub       = (GatewaySubDev *)val;
    SubDevPair    *march_sub = (SubDevPair *)usr_data;

    if (!strncmp(sub->dev_info.product_id, march_sub->product_id, MAX_SIZE_OF_PRODUCT_ID) &&
        !strncmp(sub->dev_info.device_name, march_sub->device_name, MAX_SIZE_OF_DEVICE_NAME)) {
        march_sub->pair_sub = sub;
        Log_w("remove sub[%s/%s]", sub->dev_info.product_id, sub->dev_info.device_name);
        // 1. unsub property topic
        char property_topic[MAX_SIZE_OF_CLOUD_TOPIC];
        HAL_Snprintf(property_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/down/property/%s/%s",
                     STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.product_id),
                     STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.device_name));

        IOT_MQTT_Unsubscribe(sub->mqtt_client, property_topic);
        // 2. offline?

        // 3. remove
        utils_list_remove(list, node);
        return LIST_TRAVERSE_BREAK;
    }
    return LIST_TRAVERSE_CONTINUE;
}

void iot_gateway_remove_subdev_from_sublist(GatewaySubDevHandle *sub_handle, DeviceInfo *sub_dev_info)
{
    SubDevPair pair_sub = {
        .pair_sub = NULL, .device_name = sub_dev_info->device_name, .product_id = sub_dev_info->product_id};
    utils_list_process(sub_handle->subdev_list, LIST_HEAD, _remove_subdev_by_name_str, &pair_sub);
}

int iot_gateway_add_sub_to_sublist(GatewaySubDevHandle *sub_handle, DeviceInfo *sub_dev_info, int bind_status)
{
    GatewaySubDev *new_sub = iot_gateway_find_sub(sub_handle, sub_dev_info->product_id, sub_dev_info->device_name);
    if (new_sub) {
        new_sub->bind_status = bind_status;
        Log_w("subdev[%s/%s] already exists.", sub_dev_info->product_id, sub_dev_info->device_name);
        return QCLOUD_RET_SUCCESS;
    }
    new_sub = HAL_Malloc(sizeof(GatewaySubDev));
    POINTER_SANITY_CHECK(new_sub, QCLOUD_ERR_MALLOC);
    memset(new_sub, 0, sizeof(GatewaySubDev));
    memcpy(&(new_sub->dev_info), sub_dev_info, sizeof(DeviceInfo));
    void *node = utils_list_push(sub_handle->subdev_list, new_sub);
    if (!node) {
        HAL_Free(new_sub);
        Log_e("add subdev[%s/%s] error.", sub_dev_info->product_id, sub_dev_info->device_name);
        return QCLOUD_ERR_FAILURE;
    }
    new_sub->bind_status = bind_status;
    new_sub->mqtt_client = sub_handle->mqtt_client;
    Log_i("add subdev[%s/%s] in subdev list[total subdev : %d].", sub_dev_info->product_id, sub_dev_info->device_name,
          utils_list_len_get(sub_handle->subdev_list));
    return QCLOUD_RET_SUCCESS;
}

int iot_gateway_bind_subdev(DeviceInfo *sub_dev_info)
{
    GatewaySubDevHandle *gw_subdev_handle = sg_gw_subdev_handle;
    POINTER_SANITY_CHECK(sub_dev_info, QCLOUD_ERR_FAILURE);
    POINTER_SANITY_CHECK(gw_subdev_handle, QCLOUD_ERR_FAILURE);

    // 1. check if in subdev list
    GatewaySubDev *sub = iot_gateway_find_sub(gw_subdev_handle, sub_dev_info->product_id, sub_dev_info->device_name);
    if (sub && sub->bind_status == GATEWAY_SUBDEV_BINDED) {
        return QCLOUD_RET_SUCCESS;
    }
    // 2. add to subdev list
    iot_gateway_add_sub_to_sublist(gw_subdev_handle, sub_dev_info, GATEWAY_SUBDEV_BINDING);

    // 3. sync cloud
    char              bind_buf[256];
    const DeviceInfo *bind_dev_list[] = {sub_dev_info};
    return IOT_Gateway_BindUnbind(gw_subdev_handle->mqtt_client, bind_buf, 256, bind_dev_list, 1, true);
}

int iot_gateway_subdev_on_or_offline(GatewaySubDevHandle *sub_handle, DeviceInfo *sub_dev_info, bool is_online)
{
    char              sub_dev_online_buffer[512];
    const DeviceInfo *dev_online_list[] = {sub_dev_info};
    // check device in dev list && binded
    GatewaySubDev *sub_dev = iot_gateway_find_sub(sub_handle, sub_dev_info->product_id, sub_dev_info->device_name);

    if (sub_dev && sub_dev->bind_status == GATEWAY_SUBDEV_BINDED && sub_dev->online_status != GATEWAY_SUBDEV_ONLINE) {
        sub_dev->online_status = GATEWAY_SUBDEV_ONLINING;
        return IOT_Gateway_SubOnOffLine(sub_handle->mqtt_client, sub_dev_online_buffer, 512, dev_online_list, 1,
                                        is_online);
    }
    return QCLOUD_ERR_FAILURE;
}

int iot_gateway_subdev_subscribe_property(GatewaySubDevHandle *sub_handle, DeviceInfo *sub_dev_info)
{
    POINTER_SANITY_CHECK(sub_handle, QCLOUD_ERR_FAILURE);
    POINTER_SANITY_CHECK(sub_dev_info, QCLOUD_ERR_FAILURE);
    int rc;
    // find subdev && check binded && online
    GatewaySubDev *sub = iot_gateway_find_sub(sub_handle, sub_dev_info->product_id, sub_dev_info->device_name);
    POINTER_SANITY_CHECK(sub, QCLOUD_ERR_FAILURE);
    if (sub->bind_status != GATEWAY_SUBDEV_BINDED || sub->online_status != GATEWAY_SUBDEV_ONLINE ||
        sub->is_sub_property == true) {
        Log_e("subdev [%s/%s] illegal. bind status : %d online status : %d is_sub_property : %s",
              sub_dev_info->product_id, sub_dev_info->device_name, sub->bind_status, sub->online_status,
              sub->is_sub_property ? "true" : "false");
        return QCLOUD_ERR_FAILURE;
    }

    char property_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(property_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/down/property/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(sub->dev_info.device_name));
    // subscribe normal topics and wait result
    SubscribeParams sub_params    = DEFAULT_SUB_PARAMS;
    sub_params.on_message_handler = _data_template_property_message_handler;
    sub_params.qos                = QOS1;
    sub_params.user_data          = sub;
    sub_params.user_data_free     = NULL;

    rc = IOT_MQTT_SubscribeSync(sub_handle->mqtt_client, property_topic, &sub_params);
    if (rc > 0) {
        sub->is_sub_property = true;
    }
    return rc;
}

int qcloud_gateway_subdev_unsubscribe_property(GatewaySubDevHandle *sub_handle, DeviceInfo *sub_dev_info)
{
    POINTER_SANITY_CHECK(sub_handle, QCLOUD_ERR_FAILURE);
    POINTER_SANITY_CHECK(sub_dev_info, QCLOUD_ERR_FAILURE);

    char property_topic[MAX_SIZE_OF_CLOUD_TOPIC];
    HAL_Snprintf(property_topic, MAX_SIZE_OF_CLOUD_TOPIC, "$thing/down/property/%s/%s",
                 STRING_PTR_PRINT_SANITY_CHECK(sub_dev_info->product_id),
                 STRING_PTR_PRINT_SANITY_CHECK(sub_dev_info->device_name));
    return IOT_MQTT_Unsubscribe(sub_handle->mqtt_client, property_topic);
}

// ----------------------------------------------------
// gateway callback function
// -----------------------------------------------------

UtilsJsonArrayIterResult _parse_subdev_array_cb(const char *subdev, int subdev_len, void *usr_data)
{
    int rc = 0;

    UtilsJsonValue device_name;
    UtilsJsonValue product_id;
    utils_json_value_get("device_name", strlen("device_name"), subdev, subdev_len, &device_name);
    utils_json_value_get("product_id", strlen("product_id"), subdev, subdev_len, &product_id);

    DeviceInfo dev_info;
    memset(&dev_info, 0, sizeof(DeviceInfo));
    strncpy(dev_info.product_id, product_id.value, product_id.value_len);
    strncpy(dev_info.device_name, device_name.value, device_name.value_len);
    DevArrayHandle *dev_array_handle = (DevArrayHandle *)usr_data;

    switch (dev_array_handle->type) {
        case DEV_ARRAY_HANDLE_CHANGE_BIND:
        case DEV_ARRAY_HANDLE_DESCRIBE:
            // subdev has be bind in cloud. add this to subdev list
            rc = iot_gateway_add_sub_to_sublist(dev_array_handle->gw_subdev_handle, &dev_info, GATEWAY_SUBDEV_BINDED);
            if (!rc) {
                iot_gateway_subdev_on_or_offline(dev_array_handle->gw_subdev_handle, &dev_info, true);
            }
            break;
        case DEV_ARRAY_HANDEL_ONLINE: {
            int            result = -1;
            GatewaySubDev *sub_dev =
                iot_gateway_find_sub(dev_array_handle->gw_subdev_handle, dev_info.product_id, dev_info.device_name);
            utils_json_get_int32("result", strlen("result"), subdev, subdev_len, &result);
            if (sub_dev && 0 == result) {
                sub_dev->online_status = GATEWAY_SUBDEV_ONLINE;
                iot_gateway_subdev_subscribe_property(dev_array_handle->gw_subdev_handle, &dev_info);
            } else if (sub_dev) {
                sub_dev->online_status = result;
            }
        } break;
        case DEV_ARRAY_HANDLE_BIND: {
            int            result = -1;
            GatewaySubDev *sub_dev =
                iot_gateway_find_sub(dev_array_handle->gw_subdev_handle, dev_info.product_id, dev_info.device_name);
            if (!sub_dev) {
                break;
            }

            utils_json_get_int32("result", strlen("result"), subdev, subdev_len, &result);
            if (IOT_GATEWAY_RET_SUCCESS == result || IOT_GATEWAY_ERR_BIND_REPEAT == result) {
                sub_dev->bind_status = GATEWAY_SUBDEV_BINDED;
                iot_gateway_subdev_on_or_offline(dev_array_handle->gw_subdev_handle, &dev_info, true);
            } else {
                sub_dev->bind_status = result;
            }
        } break;
        case DEV_ARRAY_HANDLE_UNBIND: {
            iot_gateway_remove_subdev_from_sublist(dev_array_handle->gw_subdev_handle, &dev_info);
        } break;

        default:
            break;
    }

    return UTILS_JSON_ARRAY_ITER_CONTINUE;
}

static void _gateway_bind_unbind_reply_callback(UtilsJsonValue sub_devices, bool is_bind, void *usr_data)
{
    GatewaySubDevHandle *gw_subdev_handle = (GatewaySubDevHandle *)usr_data;
    Log_d("bind status : %d  %.*s", is_bind, sub_devices.value_len, sub_devices.value);
    DevArrayHandle dev_array_handle = {.gw_subdev_handle = gw_subdev_handle,
                                       .type             = is_bind ? DEV_ARRAY_HANDLE_BIND : DEV_ARRAY_HANDLE_UNBIND};
    utils_json_array_parse(sub_devices.value, sub_devices.value_len, _parse_subdev_array_cb, &dev_array_handle);
}

static void _gateway_unbind_all_callback(void *usr_data)
{
    // GatewaySubDevHandle *gw_subdev_handle = (GatewaySubDevHandle *)usr_data;
    Log_d("unbind all.");
}

static void _gateway_online_offline_reply_callback(UtilsJsonValue sub_devices, bool is_online, void *usr_data)
{
    GatewaySubDevHandle *gw_subdev_handle = (GatewaySubDevHandle *)usr_data;
    Log_d("online status : %d %.*s", is_online, sub_devices.value_len, sub_devices.value);
    DevArrayHandle dev_array_handle = {.gw_subdev_handle = gw_subdev_handle,
                                       .type = is_online ? DEV_ARRAY_HANDEL_ONLINE : DEV_ARRAY_HANDLE_OFFLINE};
    utils_json_array_parse(sub_devices.value, sub_devices.value_len, _parse_subdev_array_cb, &dev_array_handle);
}

static void _gateway_search_device_callback(bool is_on, void *usr_data)
{
    // GatewaySubDevHandle *gw_subdev_handle = (GatewaySubDevHandle *)usr_data;
    Log_d("search : %d", is_on);
}

static void _gateway_describe_subdevices_reply_callback(UtilsJsonValue sub_devices, void *usr_data)
{
    GatewaySubDevHandle *gw_subdev_handle = (GatewaySubDevHandle *)usr_data;
    DevArrayHandle       dev_array_handle = {.gw_subdev_handle = gw_subdev_handle, .type = DEV_ARRAY_HANDLE_DESCRIBE};
    Log_d("describe %.*s", sub_devices.value_len, sub_devices.value);
    utils_json_array_parse(sub_devices.value, sub_devices.value_len, _parse_subdev_array_cb, &dev_array_handle);
}

static void _gateway_change_subdevices_status_callback(UtilsJsonValue sub_devices, bool is_bind, void *usr_data)
{
    GatewaySubDevHandle *gw_subdev_handle = (GatewaySubDevHandle *)usr_data;
    Log_d("change sub devices is bind %d %.*s", is_bind, sub_devices.value_len, sub_devices.value);
    DevArrayHandle dev_array_handle = {.gw_subdev_handle = gw_subdev_handle,
                                       .type = is_bind ? DEV_ARRAY_HANDLE_CHANGE_BIND : DEV_ARRAY_HANDLE_UNBIND};
    utils_json_array_parse(sub_devices.value, sub_devices.value_len, _parse_subdev_array_cb, &dev_array_handle);
}

// -----------------------------------------------------------------
// export api
// -----------------------------------------------------------------

int iot_gateway_init(void *client, int sub_dev_max)
{
    int rc = 0;
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_FAILURE);
    // 1. init gateway subdev handle
    GatewaySubDevHandle *gw_subdev_handle = HAL_Malloc(sizeof(GatewaySubDevHandle));
    POINTER_SANITY_CHECK(gw_subdev_handle, QCLOUD_ERR_MALLOC);
    UtilsListFunc func            = DEFAULT_LIST_FUNCS;
    gw_subdev_handle->subdev_list = utils_list_create(func, sub_dev_max);
    if (!gw_subdev_handle->subdev_list) {
        goto _exit;
    }
    gw_subdev_handle->mqtt_client = client;
    sg_gw_subdev_handle           = gw_subdev_handle;
    // 2. sub gateway topic
    IotGatewayMessageCallback callback = {
        .bind_unbind_reply_callback         = _gateway_bind_unbind_reply_callback,
        .change_subdevices_status_callback  = _gateway_change_subdevices_status_callback,
        .describe_subdevices_reply_callback = _gateway_describe_subdevices_reply_callback,
        .online_offline_reply_callback      = _gateway_online_offline_reply_callback,
        .search_device_callback             = _gateway_search_device_callback,
        .unbind_all_callback                = _gateway_unbind_all_callback,
    };
    rc = IOT_Gateway_Init(client, callback, gw_subdev_handle);
    if (rc < 0) {
        goto _exit;
    }

    // 3. sync subdev list from cloud
    return IOT_Gateway_Describe(client);

_exit:
    utils_list_destroy(gw_subdev_handle->subdev_list);
    HAL_Free(gw_subdev_handle);
    return QCLOUD_ERR_FAILURE;
}

int iot_gateway_deinit(void *client)
{
    POINTER_SANITY_CHECK(client, QCLOUD_ERR_FAILURE);
    GatewaySubDevHandle *gw_subdev_handle = IOT_Gateway_GetUsrData(client);
    IOT_Gateway_Deinit(client);
    POINTER_SANITY_CHECK(gw_subdev_handle, QCLOUD_ERR_FAILURE);
    utils_list_destroy(gw_subdev_handle->subdev_list);
    HAL_Free(gw_subdev_handle);
    return QCLOUD_RET_SUCCESS;
}
