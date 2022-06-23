/**
 * @copyright
 *
 * Tencent is pleased to support the open source community by making IoT Hub available.
 * Copyright(C) 2018 - 2022 THL A29 Limited, a Tencent company.All rights reserved.
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
 * @file qcloud_iot_gateway.h
 * @brief
 * @author willssong (willssong@tencent.com)
 * @version 1.0
 * @date 2022-01-23
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2022-01-23 <td>1.0     <td>willssong   <td>first commit
 * </table>
 */

#ifndef IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_GATEWAY_H_
#define IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_GATEWAY_H_

#include "qcloud_iot_common.h"

/**
 * @brief Gateway reply result code.
 * @ref https://cloud.tencent.com/document/product/1081/47442#.E9.94.99.E8.AF.AF.E7.A0.81
 *
 */
typedef enum {
    IOT_GATEWAY_RET_SUCCESS                 = 0,
    IOT_GATEWAY_ERR_UNBIND                  = -1,
    IOT_GATEWAY_ERR_FAIL                    = -2,
    IOT_GATEWAY_ERR_PARAM                   = 801,
    IOT_GATEWAY_ERR_SUBDEV_INVALID          = 802,
    IOT_GATEWAY_ERR_SIGN_FAIL               = 803,
    IOT_GATEWAY_ERR_SIGN_METHOD_UNSUPPORTED = 804,
    IOT_GATEWAY_ERR_SIGN_EXPIRED            = 805,
    IOT_GATEWAY_ERR_SUBDEV_BIND_ALREADY     = 806,
    IOT_GATEWAY_ERR_SUBDEV_TYPE_NORMAL      = 807,
    IOT_GATEWAY_ERR_OPERATION_UNSUPPORTED   = 808,
    IOT_GATEWAY_ERR_BIND_REPEAT             = 809,
    IOT_GATEWAY_ERR_SUBDEV_UNSUPPORTED      = 810
} IotGatewayResult;

/**
 * @brief Callback of gateway.
 *
 */
typedef struct {
    void (*bind_unbind_reply_callback)(UtilsJsonValue sub_devices, bool is_bind, void *usr_data);
    void (*unbind_all_callback)(void *usr_data);
    void (*online_offline_reply_callback)(UtilsJsonValue sub_devices, bool is_online, void *usr_data);
    void (*search_device_callback)(bool is_on, void *usr_data);
    void (*describe_subdevices_reply_callback)(UtilsJsonValue sub_devices, void *usr_data);
    void (*change_subdevices_status_callback)(UtilsJsonValue sub_devices, bool is_bind, void *usr_data);
} IotGatewayMessageCallback;

/**
 * @brief Subscribe gateway topic.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] callback @see IotGatewayMessageCallback
 * @param[in] usr_data usr data using in callback
 * @return @see IotReturnCode
 */
int IOT_Gateway_Init(void *client, IotGatewayMessageCallback callback, void *usr_data);

/**
 * @brief Unsubscribe gateway topic.
 *
 * @param[in,out] client pointer to mqtt client
 */
void IOT_Gateway_Deinit(void *client);

/**
 * @brief Get gateway init usr data.
 *
 * @param [in,out] client pointer to mqtt client
 * @return usr data or NULL
 */
void *IOT_Gateway_GetUsrData(void *client);

/**
 * @brief Publish subdevice online/offline message. @ref https://cloud.tencent.com/document/product/1081/47442
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to hold publish message
 * @param[in] buf_len buffer length
 * @param[in] sub_dev_list subdevice list
 * @param[in] num num of subdevice, @note only one subdevice is supported now.
 * @param[in] is_online 1: online; 0: offline
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_SubOnOffLine(void *client, char *buf, int buf_len, const DeviceInfo *sub_dev_list[], int num,
                             bool is_online);

/**
 * @brief Publish subdevice bind/unbind message. @ref https://cloud.tencent.com/document/product/1081/47441
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] buf buffer to hold publish message
 * @param[in] buf_len buffer length
 * @param[in] sub_dev_list subdevice list
 * @param[in] num num of subdevice, @note only one subdevice is supported now.
 * @param[in] is_bind 1: bind; 0: unbind
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_BindUnbind(void *client, char *buf, int buf_len, const DeviceInfo *sub_dev_list[], int num,
                           bool is_bind);

/**
 * @brief Publish subdevice describe message.
 * @ref https://cloud.tencent.com/document/product/1081/47441#.E6.9F.A5.E8.AF.A2.E6.8B.93.E6.89.91.E5.85.B3.E7.B3.BB
 *
 * @param[in,out] client pointer to mqtt client
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_Describe(void *client);

/**
 * @brief Publish search device reply message.
 * @ref
 * https://cloud.tencent.com/document/product/1081/47441#.E9.80.9A.E7.9F.A5.E7.BD.91.E5.85.B3.E5.BC.80.E5.90.AF.E6.90.9C.E7.B4.A2.E7.8A.B6.E6.80.81
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] is_on 1: on; 0: off
 * @param[in] result 0: success; 1: fail
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_SearchDeviceReply(void *client, bool is_on, int result);

/**
 * @brief Publish unbind all reply message.
 *
 * @param[in,out] client pointer to mqtt client
 * @param[in] result 0: success; 1: fail
 * @return @see packet id (>=0) when success, or err code (<0) @see IotReturnCode
 */
int IOT_Gateway_UnbindAllReply(void *client, int result);

#endif  // IOT_HUB_DEVICE_C_SDK_INCLUDE_SERVICES_COMMON_QCLOUD_IOT_GATEWAY_H_
