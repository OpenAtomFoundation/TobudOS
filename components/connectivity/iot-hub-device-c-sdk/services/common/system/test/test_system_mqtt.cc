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
 * @file test_broadcast.cc
 * @brief
 * @author fancyxu (fancyxu@tencent.com)
 * @version 1.0
 * @date 2021-07-18
 *
 * @par Change Log:
 * <table>
 * <tr><th>Date       <th>Version <th>Author    <th>Description
 * <tr><td>2021-07-18 <td>1.0     <td>fancyxu   <td>first commit
 * <tr><td>2022-01-26 <td>1.1     <td>hubert    <td>add serverip  test
 * </table>
 */

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "mqtt_client_test.h"
#include "qcloud_iot_common.h"

namespace mqtt_client_unittest {

/**
 * @brief Test system mqtt.
 *
 */
TEST_F(MqttClientTest, system_mqtt) {
  uint32_t time_stamp = 0;
  char server_ip[256] = {0};
  ASSERT_EQ(IOT_Sys_GetTime(client, &time_stamp), 0);
  ASSERT_EQ(IOT_Sys_SyncNTPTime(client), 0);
  ASSERT_EQ(IOT_Sys_GetServerIp(client, server_ip), 0);
}

}  // namespace mqtt_client_unittest
