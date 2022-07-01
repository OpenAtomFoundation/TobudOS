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
 * @file test_data_template.cc
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-09-29
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-09-29 <td>1.0     <td>fancyxu   <td>first commit
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_common.h"
#include "qcloud_iot_explorer.h"

namespace mqtt_client_unittest {

static void _bind_unbind_reply_callback(UtilsJsonValue sub_devices, bool is_bind, void *usr_data) { Log_d("recv msg"); }
static void _unbind_all_callback(void *usr_data) { Log_d("recv msg"); }
static void _online_offline_reply_callback(UtilsJsonValue sub_devices, bool is_online, void *usr_data) {
  Log_d("recv msg");
}
static void _search_device_callback(bool is_on, void *usr_data) { Log_d("recv msg"); }
static void _describe_subdevices_reply_callback(UtilsJsonValue sub_devices, void *usr_data) { Log_d("recv msg"); }
static void _change_subdevices_status_callback(UtilsJsonValue sub_devices, bool is_bind, void *usr_data) {
  Log_d("recv msg");
}
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2 * !!(condition)]))
/**
 * @brief Test data template action.
 *
 */
TEST_F(MqttClientTest, gateway) {
  char buf[1024];
  IotGatewayMessageCallback callback = {
      .bind_unbind_reply_callback = _bind_unbind_reply_callback,
      .unbind_all_callback = _unbind_all_callback,
      .online_offline_reply_callback = _online_offline_reply_callback,
      .search_device_callback = _search_device_callback,
      .describe_subdevices_reply_callback = _describe_subdevices_reply_callback,
      .change_subdevices_status_callback = _change_subdevices_status_callback,
  };

  DeviceInfo sub_dev0;
  HAL_GetDevInfo(&sub_dev0);

  const DeviceInfo *subdev[1] = {0};
  subdev[0] = &sub_dev0;

  ASSERT_EQ(IOT_Gateway_Init(client, callback, client), 0);
  ASSERT_GE(IOT_Gateway_BindUnbind(client, buf, sizeof(buf), subdev, sizeof(subdev) / sizeof(DeviceInfo *), true), 0);
  IOT_MQTT_Yield(client, 100);
  ASSERT_GE(IOT_Gateway_Describe(client), 0);
  IOT_MQTT_Yield(client, 100);
  ASSERT_GE(IOT_Gateway_SubOnOffLine(client, buf, sizeof(buf), subdev, sizeof(subdev) / sizeof(DeviceInfo *), 1), 0);
  IOT_MQTT_Yield(client, 100);
  ASSERT_GE(IOT_Gateway_SubOnOffLine(client, buf, sizeof(buf), subdev, sizeof(subdev) / sizeof(DeviceInfo *), 0), 0);
  IOT_MQTT_Yield(client, 100);
  ASSERT_GE(IOT_Gateway_BindUnbind(client, buf, sizeof(buf), subdev, sizeof(subdev) / sizeof(DeviceInfo *), false), 0);
  IOT_MQTT_Yield(client, 100);

  ASSERT_GE(IOT_Gateway_SearchDeviceReply(client, true, 0), 0);
  ASSERT_GE(IOT_Gateway_UnbindAllReply(client, 0), 0);
  IOT_Gateway_Deinit(client);
}

}  // namespace mqtt_client_unittest
